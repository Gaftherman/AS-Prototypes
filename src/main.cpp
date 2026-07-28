#include <iostream>
#include <string>
#include <vector>

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptarray/scriptarray.h>

#include "addon_registry.h"

// Callback for AngelScript compiler messages and warnings
void MessageCallback(const asSMessageInfo *msg, void *param) {
    (void)param;
    const char *type = "ERROR";
    if (msg->type == asMSGTYPE_WARNING) {
        type = "WARNING";
    } else if (msg->type == asMSGTYPE_INFORMATION) {
        type = "INFO";
    }

    std::cerr << "[" << type << "] File '" << msg->section << "' (" 
              << msg->row << ", " << msg->col << "): " 
              << msg->message << "\n";
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "AngelScript CLI Runner (asrun)\n";
        std::cout << "Usage: asrun <path/to/script.as> [script_args...]\n\n";
        std::cout << "Executes AngelScript files with standard add-ons registered.\n";
        return 1;
    }

    std::string scriptPath = argv[1];

    // 1. Create script engine
    asIScriptEngine *engine = asCreateScriptEngine();
    if (!engine) {
        std::cerr << "Failed to create AngelScript engine.\n";
        return 1;
    }

    // 2. Set message callback for compiler errors and warnings
    int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
    if (r < 0) {
        std::cerr << "Failed to set engine message callback.\n";
        engine->Release();
        return 1;
    }

    // 3. Register all standard add-ons & console functions
    if (!AddonRegistry::RegisterAllAddons(engine)) {
        std::cerr << "Failed to register AngelScript add-ons.\n";
        engine->Release();
        return 1;
    }

    // 4. Build script module using CScriptBuilder
    CScriptBuilder builder;
    r = builder.StartNewModule(engine, "MainModule");
    if (r < 0) {
        std::cerr << "Failed to start new script module.\n";
        engine->Release();
        return 1;
    }

    r = builder.AddSectionFromFile(scriptPath.c_str());
    if (r < 0) {
        std::cerr << "Could not load or find script file: " << scriptPath << "\n";
        engine->Release();
        return 1;
    }

    r = builder.BuildModule();
    if (r < 0) {
        std::cerr << "Script compilation failed. Check message output above.\n";
        engine->Release();
        return 1;
    }

    // 5. Find entry function: void main() or int main()
    asIScriptModule *mod = engine->GetModule("MainModule");
    asIScriptFunction *func = mod->GetFunctionByDecl("void main()");
    if (!func) func = mod->GetFunctionByDecl("int main()");
    if (!func) func = mod->GetFunctionByDecl("void main(array<string>@)");

    if (!func) {
        std::cerr << "Entry point 'void main()' or 'int main()' not found in script.\n";
        engine->Release();
        return 1;
    }

    // 6. Create context and execute script
    asIScriptContext *ctx = engine->CreateContext();
    if (!ctx) {
        std::cerr << "Failed to create execution context.\n";
        engine->Release();
        return 1;
    }

    ctx->Prepare(func);

    // If main accepts arguments array<string>@
    if (func->GetParamCount() == 1) {
        asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<string>");
        CScriptArray *argArray = CScriptArray::Create(arrayType, argc - 2);
        for (int i = 2; i < argc; ++i) {
            std::string argStr = argv[i];
            argArray->SetValue(i - 2, &argStr);
        }
        ctx->SetArgObject(0, argArray);
        argArray->Release();
    }

    r = ctx->Execute();

    int exitCode = 0;
    if (r == asEXECUTION_FINISHED) {
        if (func->GetReturnTypeId() == asTYPEID_INT32) {
            exitCode = static_cast<int>(ctx->GetReturnDWord());
        }
    } else if (r == asEXECUTION_EXCEPTION) {
        std::cerr << "\n--- SCRIPT EXCEPTION ---\n";
        std::cerr << "Exception: " << ctx->GetExceptionString() << "\n";
        if (ctx->GetExceptionFunction()) {
            std::cerr << "Function:  " << ctx->GetExceptionFunction()->GetDeclaration() << "\n";
        }
        std::cerr << "Line:      " << ctx->GetExceptionLineNumber() << "\n";
        exitCode = -1;
    } else {
        std::cerr << "Execution failed with code: " << r << "\n";
        exitCode = -1;
    }

    ctx->Release();
    engine->ShutDownAndRelease();

    return exitCode;
}

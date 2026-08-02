#ifndef NDEBUG
#include <doctest/doctest.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptarray/scriptarray.h>

#include "addon_registry.h"

#include "addons/ASConsole.hpp"

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

void PauseConsole() {
    std::cout << "\n----------------------------------------\n";
    std::cout << "Presiona ENTER para salir...";
    std::cout.flush();
    std::string dummy;
    std::getline(std::cin, dummy);
}

// Helper to execute a single script file
int ExecuteSingleScript(asIScriptEngine* engine, const std::string& scriptPath, const std::vector<std::string>& cleanArgs) {
    CScriptBuilder builder;
    int r = builder.StartNewModule(engine, "MainModule");
    if (r < 0) {
        std::cerr << "Failed to start new script module for: " << scriptPath << "\n";
        return 1;
    }

    r = builder.AddSectionFromFile(scriptPath.c_str());
    if (r < 0) {
        std::cerr << "Could not load or find script file: " << scriptPath << "\n";
        return 1;
    }

    r = builder.BuildModule();
    if (r < 0) {
        std::cerr << "Script compilation failed for: " << scriptPath << "\n";
        return 1;
    }

    asIScriptModule *mod = engine->GetModule("MainModule");
    asIScriptFunction *func = mod->GetFunctionByDecl("void main()");
    if (!func) func = mod->GetFunctionByDecl("int main()");
    if (!func) func = mod->GetFunctionByDecl("void main(array<string>@)");

    if (!func) {
        std::cerr << "Entry point 'void main()' or 'int main()' not found in script: " << scriptPath << "\n";
        return 1;
    }

    asIScriptContext *ctx = engine->CreateContext();
    if (!ctx) {
        std::cerr << "Failed to create execution context.\n";
        return 1;
    }

    ctx->Prepare(func);

    if (func->GetParamCount() == 1) {
        asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<string>");
        size_t scriptArgCount = cleanArgs.size() > 2 ? cleanArgs.size() - 2 : 0;
        CScriptArray *argArray = CScriptArray::Create(arrayType, static_cast<asUINT>(scriptArgCount));
        for (size_t i = 2; i < cleanArgs.size(); ++i) {
            std::string argStr = cleanArgs[i];
            argArray->SetValue(static_cast<asUINT>(i - 2), &argStr);
        }
        ctx->SetArgObject(0, argArray);
        argArray->Release();
    }

    r = ctx->Execute();

    ASConsole::ResetColor(); // Reset colors if a module has set them -TODO maybe use AS module shutdown callbacks

    int exitCode = 0;
    if (r == asEXECUTION_FINISHED) {
        if (func->GetReturnTypeId() == asTYPEID_INT32) {
            exitCode = static_cast<int>(ctx->GetReturnDWord());
        }
    } else if (r == asEXECUTION_EXCEPTION) {
        std::cerr << "\n--- SCRIPT EXCEPTION [" << scriptPath << "] ---\n";
        std::cerr << "Exception: " << ctx->GetExceptionString() << "\n";
        if (ctx->GetExceptionFunction()) {
            std::cerr << "Function:  " << ctx->GetExceptionFunction()->GetDeclaration() << "\n";
        }
        std::cerr << "Line:      " << ctx->GetExceptionLineNumber() << "\n";
        exitCode = -1;
    } else {
        std::cerr << "Execution failed for " << scriptPath << " with code: " << r << "\n";
        exitCode = -1;
    }

    ctx->Release();
    engine->DiscardModule("MainModule");
    return exitCode;
}

int main(int argc, char **argv) {
    bool shouldPause = true;

    // Check for flags
    std::vector<std::string> cleanArgs;

    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
#ifndef NDEBUG
        if( arg == "--test" )
        {
            std::filesystem::current_path( "../../" );
            std::cout << "Set working directory to ";
            ASConsole::InitWindows();
            ASConsole::SetColor( ASConsole::Color::ForeGround, 0, 255, 0 );
            std::cout << std::filesystem::current_path();
            ASConsole::ResetColor();
            std::cout << std::endl;
            doctest::Context context;
            context.addFilter("reporters", "custom_console");
            return context.run();
        }
#endif
        if (arg == "--no-pause") {
            shouldPause = false;
        } else {
            cleanArgs.push_back(arg);
        }
    }

    if (cleanArgs.size() < 2) {
        std::cout << "========================================\n";
        std::cout << "   AngelScript CLI Runner (asrun)\n";
        std::cout << "========================================\n\n";
        std::cout << "Uso: asrun <archivo.as> [--no-pause] [--test] [argumentos_del_script...]\n\n";
        std::cout << "Tambien puedes arrastrar un archivo .as directamente sobre el ejecutable.\n";
        
        if (shouldPause) PauseConsole();
        return 1;
    }

    std::string scriptPath = cleanArgs[1];

    asIScriptEngine *engine = asCreateScriptEngine();
    if (!engine) {
        std::cerr << "Failed to create AngelScript engine.\n";
        if (shouldPause) PauseConsole();
        return 1;
    }

    int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
    if (r < 0) {
        std::cerr << "Failed to set engine message callback.\n";
        engine->Release();
        if (shouldPause) PauseConsole();
        return 1;
    }

    if (!AddonRegistry::RegisterAllAddons(engine)) {
        std::cerr << "Failed to register AngelScript add-ons.\n";
        engine->Release();
        if (shouldPause) PauseConsole();
        return 1;
    }

    int exitCode = ExecuteSingleScript(engine, scriptPath, cleanArgs);

    engine->ShutDownAndRelease();

    if (shouldPause) {
        PauseConsole();
    }

    return exitCode;
}

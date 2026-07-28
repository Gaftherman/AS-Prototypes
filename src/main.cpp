#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptarray/scriptarray.h>

#include "addon_registry.h"

namespace fs = std::filesystem;

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

// Function to find test script paths in ../../Tests, ../Tests, Tests, etc.
std::vector<fs::path> FindTestScriptFiles() {
    std::vector<fs::path> paths;
    std::vector<fs::path> candidates = {
        "../../Tests",
        "../Tests",
        "Tests",
        "../../tests",
        "../tests",
        "tests"
    };

    for (const auto& candidate : candidates) {
        if (fs::exists(candidate) && fs::is_directory(candidate)) {
            for (const auto& entry : fs::recursive_directory_iterator(candidate)) {
                if (entry.is_regular_file() && entry.path().extension() == ".as") {
                    paths.push_back(entry.path());
                }
            }
            if (!paths.empty()) break;
        }
    }
    return paths;
}

// Helper to execute a single script file
int ExecuteSingleScript(asIScriptEngine* engine, const std::string& scriptPath, const std::vector<std::string>& cleanArgs) {
    CScriptBuilder builder;
    int r = builder.StartNewModule(engine, "MainModule");
    if (r < 0) {
        std::cerr << "Failed to start new script module for: " << scriptPath << "\n";
        Tests::Fails++;
        return 1;
    }

    r = builder.AddSectionFromFile(scriptPath.c_str());
    if (r < 0) {
        std::cerr << "Could not load or find script file: " << scriptPath << "\n";
        Tests::Fails++;
        return 1;
    }

    r = builder.BuildModule();
    if (r < 0) {
        std::cerr << "Script compilation failed for: " << scriptPath << "\n";
        Tests::Fails++;
        return 1;
    }

    asIScriptModule *mod = engine->GetModule("MainModule");
    asIScriptFunction *func = mod->GetFunctionByDecl("void main()");
    if (!func) func = mod->GetFunctionByDecl("int main()");
    if (!func) func = mod->GetFunctionByDecl("void main(array<string>@)");

    if (!func) {
        std::cerr << "Entry point 'void main()' or 'int main()' not found in script: " << scriptPath << "\n";
        Tests::Fails++;
        return 1;
    }

    asIScriptContext *ctx = engine->CreateContext();
    if (!ctx) {
        std::cerr << "Failed to create execution context.\n";
        Tests::Fails++;
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
        Tests::Fails++;
        exitCode = -1;
    } else {
        std::cerr << "Execution failed for " << scriptPath << " with code: " << r << "\n";
        Tests::Fails++;
        exitCode = -1;
    }

    ctx->Release();
    engine->DiscardModule("MainModule");
    return exitCode;
}

// doctest suite for running test scripts from Tests directory
TEST_CASE("AngelScript Test Directory Runner") {
    auto testFiles = FindTestScriptFiles();
    if (!testFiles.empty()) {
        asIScriptEngine *engine = asCreateScriptEngine();
        REQUIRE(engine != nullptr);
        engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
        REQUIRE(AddonRegistry::RegisterAllAddons(engine) == true);

        for (const auto& filePath : testFiles) {
            SUBCASE(filePath.string().c_str()) {
                std::cout << "[TEST] Running script: " << filePath.string() << "\n";
                int result = ExecuteSingleScript(engine, filePath.string(), {});
                CHECK(result == 0);
            }
        }
        engine->ShutDownAndRelease();
    }
}

int main(int argc, char **argv) {
    bool shouldPause = true;
    bool isDebug = false;

#ifndef NDEBUG
    isDebug = true;
#endif

    // Check for flags
    std::vector<std::string> cleanArgs;
    bool runTestsFlag = false;

    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--no-pause") {
            shouldPause = false;
        } else if (arg == "--test" || arg == "--doctest") {
            runTestsFlag = true;
        } else {
            cleanArgs.push_back(arg);
        }
    }

    // Run doctest if --test flag is passed or in Debug mode when no specific script is passed
    if (runTestsFlag || (isDebug && cleanArgs.size() < 2)) {
        doctest::Context context;
        context.applyCommandLine(argc, argv);
        int res = context.run();
        
        // If debug mode and no script was provided, run all tests in Tests directory directly
        if (cleanArgs.size() < 2) {
            auto testFiles = FindTestScriptFiles();
            if (!testFiles.empty()) {
                std::cout << "\n[Debug Mode] Found " << testFiles.size() << " test script(s) in Tests directory:\n";
                asIScriptEngine *engine = asCreateScriptEngine();
                if (engine) {
                    engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
                    if (AddonRegistry::RegisterAllAddons(engine)) {
                        for (const auto& testFile : testFiles) {
                            std::cout << "\n--> Running: " << testFile.string() << "\n";
                            ExecuteSingleScript(engine, testFile.string(), cleanArgs);
                        }
                    }
                    engine->ShutDownAndRelease();
                }
            } else {
                std::cout << "\n[Debug Mode] No test scripts found in ../../Tests/ or Tests/\n";
            }
        }

        if (Tests::Fails > 0) {
            std::cerr << Tests::Fails << " tests failed out of " << (Tests::Fails + Tests::Passes) << "\n";
        } else if (Tests::Passes > 0) {
            std::cout << Tests::Passes << " tests passed" << "\n";
        }

        if (shouldPause) PauseConsole();
        return res;
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

    if (Tests::Fails > 0) {
        std::cerr << Tests::Fails << " tests failed out of " << (Tests::Fails + Tests::Passes) << "\n";
    } else if (Tests::Passes > 0) {
        std::cout << Tests::Passes << " tests passed" << "\n";
    }

    if (shouldPause) {
        PauseConsole();
    }

    return exitCode;
}

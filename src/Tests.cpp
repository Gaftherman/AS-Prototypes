#ifndef NDEBUG
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "addon_registry.h"
#include "as_predefined.h"

namespace fs = std::filesystem;

namespace Tests
{
    inline int Fails = 0;
    inline int Passes = 0;

    inline bool Expect( const CString& title, bool expected, bool condition )
    {
        asIScriptContext* ctx = asGetActiveContext();

        if( ctx != nullptr && expected == condition )
        {
            std::cout << "Passed test \"" << title.c_str() << "\"" << "\n";
            Tests::Passes++;
            return true;
        }
        std::cerr << "Failed test \"" << title.c_str() << "\"" << "\n";
        Tests::Fails++;
        return false;
    }

    bool Register( asIScriptEngine* engine )
    {
        engine->SetDefaultNamespace( "Tests" );

        REGISTER_GLOBAL_PROPERTY("int Passes", &::Tests::Passes, "Number of passed test assertions.");
        REGISTER_GLOBAL_PROPERTY("int Fails", &::Tests::Fails, "Number of failed test assertions.");
        REGISTER_GLOBAL_FUNCTION("bool Expect( const string&in title, bool expected, bool condition )", asFUNCTION(&::Tests::Expect), asCALL_CDECL, "Tests an assertion condition and increments pass/fail counters." );

        engine->SetDefaultNamespace( "" );

        return true;
    }
}

extern void MessageCallback( const asSMessageInfo*, void* );
extern int ExecuteSingleScript( asIScriptEngine*, const std::string&, const std::vector<std::string>& );

// doctest suite for running test scripts from Tests directory
TEST_CASE( "AngelScript Test Directory Runner" )
{
    asIScriptEngine* engine = asCreateScriptEngine();

    REQUIRE( engine != nullptr );

    // Function to find test script paths in ../../Tests, ../Tests, Tests, etc.
    auto FindTestScriptFiles = []() -> std::vector<fs::path>
    {
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

        // Sort files by modification date: oldest first, newest LAST
        std::sort(paths.begin(), paths.end(), [](const fs::path& a, const fs::path& b) {
            std::error_code ec1, ec2;
            auto timeA = fs::last_write_time(a, ec1);
            auto timeB = fs::last_write_time(b, ec2);
            if (!ec1 && !ec2) {
                return timeA < timeB;
            }
            return a.string() < b.string();
        });

        return paths;
    };

    auto testFiles = FindTestScriptFiles();

    if( !testFiles.empty() )
    {
        engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
        REQUIRE( AddonRegistry::RegisterAllAddons(engine) == true );
        REQUIRE( Tests::Register(engine) == true );

        for( const auto& filePath : testFiles )
        {
            std::string filePathString = filePath.string();

            SUBCASE( filePathString.c_str() )
            {
                std::cout << "[TEST] Running script: " << filePathString << "\n";
                int result = ExecuteSingleScript( engine, filePathString, {} );
                CHECK( result == 0 );
            }
        }
    }
    else
    {
        std::cout << "\n[Tests] No test scripts found in ../../Tests/ or Tests/\n";
    }

    if( Tests::Fails > 0 )
    {
        std::cerr << Tests::Fails << " tests failed out of " << (Tests::Fails + Tests::Passes) << "\n";
    }
    else if( Tests::Passes > 0 )
    {
        std::cout << Tests::Passes << " tests passed" << "\n";
    }

    GenerateScriptPredefined(engine, "as.predefined");

    engine->ShutDownAndRelease();
}
#endif

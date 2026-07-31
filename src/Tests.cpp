#ifndef NDEBUG
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <filesystem>

#include "addon_registry.h"
#include "as_predefined.h"

#include "addons/ASConsole.hpp"

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
            ASConsole::SetColor( ASConsole::Color::ForeGround, 60, 255, 60 );
            ASConsole::Write( "Passed" );
            ASConsole::ResetColor();
            std::cout << " test \"" << title.c_str() << "\"" << "\n";
            Tests::Passes++;
            return true;
        }
        ASConsole::SetColor( ASConsole::Color::ForeGround, 255, 60, 60 );
        ASConsole::Write( "Failed" );
        ASConsole::ResetColor();
        std::cerr << " test \"" << title.c_str() << "\"" << "\n";
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

        constexpr std::array candidates = {
            "../../Tests",
            "../Tests",
            "Tests",
            "../../tests",
            "../tests",
            "tests"
        };

        for( const auto& candidate : candidates )
        {
            fs::path dir(candidate);

            std::error_code ec;

            if( !fs::exists( dir, ec ) || !fs::is_directory( dir, ec ) )
                continue;

            for( const auto& entry : fs::directory_iterator( dir, ec ) )
            {
                if( !entry.is_regular_file() )
                    continue;

                const auto& path = entry.path();

                if( path.extension() == ".as" )
                {
                    paths.emplace_back( path );
                }
            }

            if( !paths.empty() )
                break;
        }

        std::sort( paths.begin(), paths.end(), []( const fs::path& a, const fs::path& b )
        {
            std::error_code ec1, ec2;

            auto ta = fs::last_write_time( a, ec1 );
            auto tb = fs::last_write_time( b, ec2 );

            if( !ec1 && !ec2 )
                return ta < tb;

            return a.native() < b.native();
        } );

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
                if( result != 0 )
                    break;
            }
        }
    }
    else
    {
        ASConsole::SetColor( ASConsole::Color::ForeGround, 255, 60, 60 );
        ASConsole::WriteLine( "No test scripts found in ../../Tests/" );
        ASConsole::ResetColor();
    }

    ASConsole::SetColor( ASConsole::Color::BackGround, 100, 100, 255 );
    ASConsole::Write( ">" );
    ASConsole::ResetColor();

    if( Tests::Fails > 0 )
    {
        ASConsole::SetColor( ASConsole::Color::ForeGround, 255, 60, 60 );
        std::cerr << " " << Tests::Fails << " tests failed out of " << (Tests::Fails + Tests::Passes);
        ASConsole::WriteLineEmpty();
        ASConsole::ResetColor();
    }
    else if( Tests::Passes > 0 )
    {
        ASConsole::SetColor( ASConsole::Color::ForeGround, 60, 255, 60 );
        std::cout << " " << Tests::Passes << " tests passed";
        ASConsole::WriteLineEmpty();
        ASConsole::ResetColor();
    }

    GenerateScriptPredefined(engine, "../../as.predefined");

    engine->ShutDownAndRelease();
}
#endif

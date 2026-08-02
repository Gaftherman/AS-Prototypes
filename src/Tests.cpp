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
    inline int TotalFails = 0;
    inline int TotalPasses = 0;

    inline int Fails = 0;
    inline int Passes = 0;

    inline bool Expect( const CString& title, bool expected, bool condition )
    {
        const char* titleCStr = title.c_str();

        if( titleCStr == nullptr )
            titleCStr = "[Invalid/Null String Data]";

        asIScriptContext* ctx = asGetActiveContext();

        if( ctx != nullptr && expected == condition )
        {
            ASConsole::SetColor( ASConsole::Color::ForeGround, 60, 255, 60 );
            ASConsole::Write( "Passed" );
            ASConsole::ResetColor();
            std::cout << " test \"" << titleCStr << "\"" << "\n";
            Tests::Passes++;
            return true;
        }
        ASConsole::SetColor( ASConsole::Color::ForeGround, 255, 60, 60 );
        ASConsole::Write( "Failed" );
        ASConsole::ResetColor();
        std::cerr << " test \"" << titleCStr << "\"" << "\n";
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
#include <mutex>

struct CustomConsoleReporter : public doctest::IReporter
{
    const doctest::ContextOptions& opt;
    const doctest::TestCaseData* current_test_case = nullptr;
    std::mutex mutex;
    CustomConsoleReporter(const doctest::ContextOptions& in) : opt(in) {}
    static int get_num_active_contexts() { return 0; }
    void report_query(const doctest::QueryData&) override {}
    void test_run_start() override {}
    void test_case_start(const doctest::TestCaseData& in) override { current_test_case = &in; }
    void test_case_reenter(const doctest::TestCaseData&) override {}
    void subcase_end() override {}
    void log_message(const doctest::MessageData&) override {}
    void test_case_exception(const doctest::TestCaseException&) override {}
    void test_case_skipped(const doctest::TestCaseData&) override {}
    void test_case_end(const doctest::CurrentTestCaseStats& stats) override {
        if(stats.failure_flags & doctest::TestCaseFailureReason::Crash) {
            std::cerr << "¡Unexpected crash!\n";
        }
    }
    void log_assert(const doctest::AssertData& in) override {
        if(!in.m_failed) return;
        std::lock_guard<std::mutex> lock(mutex);
        std::cerr << " Error: " << in.m_expr << " líne " << in.m_line << "\n";
    }
    void subcase_start(const doctest::SubcaseSignature& in) override
    {
        ASConsole::SetColor( ASConsole::Color::BackGround, 50, 50, 50 );
        ASConsole::SetColor( ASConsole::Color::ForeGround, 200, 200, 50 );

        std::string lines = "================================";
        for( size_t i = 0; i < in.m_name.size(); i++ )
            lines += "=";

        ASConsole::WriteLine( lines );
        ASConsole::Write( "> Running Script AngelScript: " );
        ASConsole::Write( in.m_name.c_str() );
        ASConsole::WriteLine( " <" );
        ASConsole::WriteLine( lines );
        ASConsole::ResetColor();
    }
    void test_run_end(const doctest::TestRunStats& stats) override
    {
        std::lock_guard<std::mutex> lock(mutex);

        #define _reset() \
            ASConsole::SetColor( ASConsole::Color::BackGround, 50, 50, 50 ); \
            ASConsole::SetColor( ASConsole::Color::ForeGround, 200, 200, 50 )
        #define _cover( text, r, g, b ) \
            ASConsole::SetColor( ASConsole::Color::ForeGround, r, g, b ); \
            std::cout << text; \
            _reset()

        _reset();

        ASConsole::WriteLine( "===============================================================================" );

        if( Tests::TotalFails > 0 )
        {
            std::cout << "> ";
            _cover( "Failed", 255, 50, 50 );
            std::cout << ": ";
            _cover( Tests::TotalFails, 255, 50, 50 );
            ASConsole::WriteLine( " tests" );
        }

        if( Tests::TotalPasses > 0 )
        {
            std::cout << "> ";
            _cover( "Passed", 50, 255, 50 );
            std::cout << ": ";
            _cover( Tests::TotalPasses, 50, 255, 50 );
            ASConsole::WriteLine( " tests" );
        }

        if( stats.numTestCasesFailed > 0 )
        {
            std::cout << "> ";
            _cover( "Failed", 255, 50, 50 );
            std::cout << ": ";
            _cover( stats.numTestCasesFailed, 255, 50, 50 );
            ASConsole::WriteLine( " asserts" );
        }

        if( ( stats.numAsserts - stats.numAssertsFailed ) > 0 )
        {
            std::cout << "> ";
            _cover( "Passed", 50, 255, 50 );
            std::cout << ": ";
            _cover( ( stats.numAsserts - stats.numAssertsFailed ), 50, 255, 50 );
            std::cout << " asserts";

            if( stats.numAsserts != ( stats.numAsserts - stats.numAssertsFailed ) )
            {
                std::cout << " out of ";
                _cover( stats.numAsserts, 50, 255, 50 );
            }

            ASConsole::WriteLineEmpty();
        }

        ASConsole::WriteLine( "===============================================================================" );
        ASConsole::ResetColor();
    }
};

REGISTER_REPORTER("custom_console", 0, CustomConsoleReporter);

extern void MessageCallback( const asSMessageInfo*, void* );
extern int ExecuteSingleScript( asIScriptEngine*, const std::string&, const std::vector<std::string>& );

// doctest suite for running test scripts from Tests directory
TEST_CASE( "AngelScript Test Directory Runner" )
{
    asIScriptEngine* engine = asCreateScriptEngine();

    REQUIRE( engine != nullptr );

    // Function to find test script paths in Tests, Tests, Tests, etc.
    auto FindTestScriptFiles = []() -> std::vector<fs::path>
    {
        std::vector<fs::path> paths;

        std::error_code ec;

        for( const auto& entry : fs::directory_iterator( fs::current_path(), ec ) )
        {
            if( !entry.is_regular_file() )
                continue;

            const auto& path = entry.path();

            if( path.extension() == ".as" )
            {
                paths.emplace_back( path );
            }
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
        ASConsole::Write( "No test scripts found in \"" );
        ASConsole::Write( fs::current_path().string() );
        ASConsole::WriteLine( "\"" );
        Tests::TotalFails++;
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

    Tests::TotalFails += Tests::Fails;
    Tests::Fails = 0;
    Tests::TotalPasses += Tests::Passes;
    Tests::Passes = 0;

    GenerateScriptPredefined(engine, "../as.predefined");

    engine->ShutDownAndRelease();

    CHECK( Tests::TotalFails == 0 );
}
#endif

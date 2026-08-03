#ifndef NDEBUG
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <filesystem>
#include <mutex>

#include "addon_registry.h"
#include "as_predefined.h"

#include "addons/Console.hpp"
#include "CASDocRegistry.hpp"

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
            Console->Fore->rgb( 60, 255, 60 )
                ->Write( "Passed" )
                ->ResetColor()
                ->Write( " test \"" )
                ->Fore->rgb( 0, 255, 200 )
                ->Write( titleCStr )
                ->ResetColor()
                ->WriteLine( "\"" );
            Tests::Passes++;
            return true;
        }
        Console->Fore->rgb( 255, 60, 60 )
            ->Write( "Failed" )
            ->ResetColor()
            ->Write( " test \"" )
            ->Fore->rgb( 0, 255, 200 )
            ->Write( titleCStr )
            ->ResetColor()
            ->WriteLine( "\"" )
            ->Fore->rgb( 255, 60, 60 )
            ->WriteLine( "------" )
        ->ResetColor();
        Tests::Fails++;
        return false;
    }

    void ScriptAssert( bool condition, const CString& message )
    {
        if( condition )
            return;

        Console->Fore->rgb(255,0,0)
            ->Back->rgb( 40, 0, 0 )
            ->Write( "AngelScript assert: " );

        if( !message.empty() )
        {
            Console->Fore->rgb(255,100,0)
                ->Write( message );
        }

        Console->WriteLine();

        asIScriptContext* ctx = asGetActiveContext();

        asUINT stackSize = ctx->GetCallstackSize();

        for( asUINT n = 0; n < stackSize; n++ )
        {
            const asIScriptFunction* function = ctx->GetFunction(n);

            if( function != nullptr )
            {
                function = ctx->GetFunction(n);

                ctx->SetException( !message.empty() ? message.c_str() : "assert" );

                if( const char* cstr = function->GetDeclaration(); cstr != nullptr && cstr[0] != '\0' )
                    Console->Fore->rgb( 0, 255, 200 )->Write( "Method: " )->Fore->rgb(255, 100,0)->WriteLine( cstr );

                if( function->GetFuncType() == asFUNC_SCRIPT )
                {
                    const char* scriptSection = nullptr;

                    int line = 0;
                    if( n == 0 )
                        line = ctx->GetExceptionLineNumber( 0, &scriptSection );
                    else
                        line = ctx->GetLineNumber( n, 0, &scriptSection );

                    Console->Fore->rgb( 0, 255, 200 )->Write( "Line: " )->Fore->rgb(255, 100,0)->WriteLine( line );

                    if( const char* cstr = function->GetNamespace(); cstr != nullptr && cstr[0] != '\0' )
                    Console->Fore->rgb( 0, 255, 200 )->Write( "namespace: " )->Fore->rgb(255, 100,0)->WriteLine( cstr );

                    if( const char* cstr = function->GetObjectName(); cstr != nullptr && cstr[0] != '\0' )
                    Console->Fore->rgb( 0, 255, 200 )->Write( "class: " )->Fore->rgb(255, 100,0)->WriteLine( cstr );

                    if( scriptSection != nullptr )
                        Console->Fore->rgb( 0, 255, 200 )->Write( "file: " )->Fore->rgb(255, 100,0)->WriteLine( scriptSection );
                }

                if( n < stackSize - 1 )
                {
                    Console->Fore->rgb( 255, 0, 255 )->WriteLine( "--------------------" );
                }
            }
        }

        Console->ResetColor();

        ctx->Abort();
    }

    inline bool stop = false;

    inline void Stop()
    {
        stop = true;
    }
}

class CASDocTests : public CASDocRegistry
{
    bool Register() override
    {
        return
        SetDefaultNamespace( "Tests" ) &&
        RegisterGlobalProperty(
            "Number of passed test assertions."sv,
            "int Passes",
            &::Tests::Passes
        ) &&
        RegisterGlobalProperty(
            "Number of failed test assertions."sv,
            "int Fails",
            &::Tests::Passes
        ) &&
        RegisterGlobalFunction(
            "Tests an assertion condition and increments pass/fail counters if expect is not equal to condition."sv,
            "bool Expect( const string&in title, bool expected, bool condition )",
            asFUNCTION(&::Tests::Expect),
            asCALL_CDECL
        ) &&
        RegisterGlobalFunction(
            "Stops execution of tests. useful while wanting to test a specific file only."sv,
            "void Stop()",
            asFUNCTION(&::Tests::Stop),
            asCALL_CDECL
        ) &&
        SetDefaultNamespace( "" ) &&
        RegisterGlobalFunction(
            "Stops the execution of the script in-place if the condition is false, prints stack call and waits for user input."sv,
            "void assert( bool condition, const string&in = \"\" )",
            asFUNCTION(&::Tests::ScriptAssert),
            asCALL_CDECL
        );
    }
};

CASDocRegisterInterface(CASDocTests);

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
        Console->Back->rgb( 50, 50, 50 )
            ->Fore->rgb( 200, 200, 50 );

        std::string lines = "================================";
        for( size_t i = 0; i < in.m_name.size(); i++ )
            lines += "=";

        Console->WriteLine( lines );
        Console->Write( "> Running Script AngelScript: " );
        Console->Write( in.m_name.c_str() );
        Console->WriteLine( " <" );
        Console->WriteLine( lines );
        Console->ResetColor();
    }
    void test_run_end(const doctest::TestRunStats& stats) override
    {
        std::lock_guard<std::mutex> lock(mutex);

        #define _reset() Console->Back->rgb( 50, 50, 50 )->Fore->rgb( 200, 200, 50 )
        #define _cover( text, r, g, b ) Console->Fore->rgb(r, g, b )->Write(text); _reset()

        _reset();

        Console->WriteLine( "===============================================================================" );

        if( Tests::TotalFails > 0 )
        {
            std::cout << "> ";
            _cover( "Failed", 255, 50, 50 );
            std::cout << ": ";
            _cover( Tests::TotalFails, 255, 50, 50 );
            Console->WriteLine( " tests" );
        }

        if( Tests::TotalPasses > 0 )
        {
            std::cout << "> ";
            _cover( "Passed", 50, 255, 50 );
            std::cout << ": ";
            _cover( Tests::TotalPasses, 50, 255, 50 );
            Console->WriteLine( " tests" );
        }

        if( stats.numTestCasesFailed > 0 )
        {
            std::cout << "> ";
            _cover( "Failed", 255, 50, 50 );
            std::cout << ": ";
            _cover( stats.numTestCasesFailed, 255, 50, 50 );
            Console->WriteLine( " asserts" );
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

            Console->WriteLine();
        }

        Console->WriteLine( "===============================================================================" );
        Console->ResetColor();
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
                return ta > tb;

            return a.native() > b.native();
        } );

        return paths;
    };

    auto testFiles = FindTestScriptFiles();

    if( !testFiles.empty() )
    {
        engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
        REQUIRE( AddonRegistry::RegisterAllAddons(engine) == true );

        for( const auto& filePath : testFiles )
        {
            std::string filePathString = filePath.string();

            SUBCASE( filePathString.c_str() )
            {
                int result = ExecuteSingleScript( engine, filePathString, {} );
                CHECK( result == 0 );
                if( result != 0 || Tests::stop )
                    break;
            }
        }
    }
    else
    {
        Console->Fore->rgb( 255, 60, 60 )
            ->Write( "No test scripts found in \"" )
            ->Write( fs::current_path().string() )
            ->WriteLine( "\"" )
        ->ResetColor();

        Tests::TotalFails++;
    }

    Console->Back->rgb( 100, 100, 255 )
        ->Write( ">" )
    ->ResetColor();

    if( Tests::Fails > 0 )
    {
        Console->Fore->rgb( 255, 60, 60 )
            ->Write( " " )
            ->Write( Tests::Fails )
            ->Write( " tests failed out of " )
            ->Fore->rgb( 60, 255, 60 )
            ->WriteLine( ( Tests::Fails + Tests::Passes ) )
        ->ResetColor();
    }
    else if( Tests::Passes > 0 )
    {
        Console->Fore->rgb( 60, 255, 60 )
            ->Write( " " )
            ->Write( Tests::Passes )
            ->WriteLine( " tests passed" )
        ->ResetColor();
    }

    Tests::TotalFails += Tests::Fails;
    Tests::Fails = 0;
    Tests::TotalPasses += Tests::Passes;
    Tests::Passes = 0;

    engine->ShutDownAndRelease();

    CHECK( Tests::TotalFails == 0 );
}
#endif

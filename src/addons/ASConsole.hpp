#include "addon_registry.h"

#include <iostream>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

namespace ASConsole
{
    enum Color
    {
        ForeGround = 0,
        BackGround,
        Both
    };

    static const std::string BackgroundColorDefault = "\033[49m";
    static const std::string ForegroundColorDefault = "\033[39m";

    static CString BackgroundColor = BackgroundColorDefault;
    static CString ForegroundColor = ForegroundColorDefault;

    static void SetColor( Color target, int red, int green, int blue )
    {
        if( target == Color::ForeGround || target == Color::Both )
            ForegroundColor = "\033[38;2;" + std::to_string(red) + ";" + std::to_string(green) + ";" + std::to_string(blue) + "m";
        if( target == Color::BackGround || target == Color::Both )
            BackgroundColor = "\033[48;2;" + std::to_string(red) + ";" + std::to_string(green) + ";" + std::to_string(blue) + "m";

        std::cout << BackgroundColor << ForegroundColor << std::flush;
    }

    static void ResetColor( Color target = Color::Both )
    {
        if( target == Color::ForeGround || target == Color::Both )
            ForegroundColor = ForegroundColorDefault;
        if( target == Color::BackGround || target == Color::Both )
            BackgroundColor = BackgroundColorDefault;

        std::cout << BackgroundColor << ForegroundColor << std::flush;
    }

    static void Write( const std::string& msg )
    {
        std::cout << msg << std::flush;
    }

    static void WriteLineEmpty()
    {
        // before printing endl we need to reset the background. then set it back again after the line drop.
        std::cout << BackgroundColorDefault << std::endl << BackgroundColor;
    }

    static void WriteLine( const std::string& msg )
    {
        std::cout << msg;
        WriteLineEmpty();
    }

    static inline void InitWindows()
    {
#if defined(_WIN32) || defined(_WIN64)
        HANDLE hOut = GetStdHandle( STD_OUTPUT_HANDLE );

        if( hOut != INVALID_HANDLE_VALUE )
        {
            DWORD dwMode = 0;

            if( GetConsoleMode(hOut, &dwMode) )
            {
                dwMode |= 0x0004; 
                SetConsoleMode(hOut, dwMode);
            }
        }
#endif
    }

    static inline void Register( asIScriptEngine* engine )
    {
        InitWindows();

        engine->SetDefaultNamespace( "Console" );

        REGISTER_GLOBAL_FUNCTION( "void Write( const string&in msg )", asFUNCTION(ASConsole::Write), asCALL_CDECL, "Print to the console." );
        REGISTER_GLOBAL_FUNCTION( "void WriteLine( const string&in msg )", asFUNCTION(ASConsole::WriteLine), asCALL_CDECL, "Print line to the console." );
        REGISTER_GLOBAL_FUNCTION( "void WriteLine()", asFUNCTION(ASConsole::WriteLineEmpty), asCALL_CDECL, "Print line to the console." );

        REGISTER_ENUM( "Color", "Console color." );
        REGISTER_ENUM_VALUE( "Color", "ForeGround", static_cast<int>(ASConsole::Color::ForeGround), "Console text color." );
        REGISTER_ENUM_VALUE( "Color", "BackGround", static_cast<int>(ASConsole::Color::BackGround), "Console background color." );
        REGISTER_ENUM_VALUE( "Color", "Both", static_cast<int>(ASConsole::Color::Both), "Both console text and background colors." );

        REGISTER_GLOBAL_PROPERTY( "string ForegroundColor", &ASConsole::ForegroundColor, "Represents the text color in the console output." );
        REGISTER_GLOBAL_PROPERTY( "string BackgroundColor", &ASConsole::BackgroundColor, "Represents the background color in the console output." );

        REGISTER_GLOBAL_FUNCTION( "void SetColor( Color target, int red, int green, int blue )", asFUNCTION(ASConsole::SetColor), asCALL_CDECL, "Set the console color code based on the given RGB values." );
        REGISTER_GLOBAL_FUNCTION( "void ResetColor( Color target = Color::Both )", asFUNCTION(ASConsole::ResetColor), asCALL_CDECL, "Reset the console color code." );

        engine->SetDefaultNamespace( "" );
    }
};

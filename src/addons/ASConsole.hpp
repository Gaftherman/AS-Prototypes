#include "addon_registry.h"

#include <iostream>
#include <string>

namespace ASConsole
{
    void Print( const std::string& msg )
    {
        std::cout << msg;
        std::cout.flush();
    }

    void Println( const std::string& msg )
    {
        std::cout << msg << std::endl;
    }

    static inline void Register( asIScriptEngine* engine )
    {
        engine->SetDefaultNamespace( "Console" );
        REGISTER_GLOBAL_FUNCTION( "void Print( const string&in msg )", asFUNCTION(ASConsole::Print), asCALL_CDECL, "Print to console. uses fmt library to replace \"{}\" arguments." );
        REGISTER_GLOBAL_FUNCTION( "void PrintLine( const string&in msg )", asFUNCTION(ASConsole::Print), asCALL_CDECL, "Print line to console. uses fmt library to replace \"{}\" arguments." );
        engine->SetDefaultNamespace( "" );
    }
};

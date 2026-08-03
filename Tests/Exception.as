#include "utils/title.as"

using namespace Tests;

void throwExceptionStack()
{
    Exception::Throw( "Exception from method" );
}

namespace someNamespace
{
    namespace OtherNameSpace
    {
        void throwExceptionFromNamespace()
        {
            Exception::Throw( "Exception from nested namespace" );
        }
    }

    void throwExceptionFromNamespace()
    {
        Exception::Throw( "Exception from namespace" );
    }
}

class someClass
{
    void exThrow()
    {
        Exception::Throw( "Exception from class method" );
    }
}

someClass g_Class;

namespace SomeClassNameSpace
{
    class SomeClass2 : someClass
    {
        void exThrow()
        {
            Exception::Throw( "Exception from class method in namespace" );
        }
    }

    SomeClass2 ClassSpace;
}

void main()
{
    title( "Exception handler class" );

    try
    {
        throwExceptionStack();
        Expect( "Exception::Throw(string) catch check", true, false );
    }
    catch
    {
        Expect( "Exception::Throw(string) successfully caught", true, true );

        Expect( "Exception::Message() is not empty: " + Exception::Message(), true, !Exception::Message().isEmpty() );
        Expect( "Exception::Id() returns a valid positive index", true, Exception::Id() > 0 );
        Expect( "Exception::Line() returns a valid line number", true, Exception::Line() > 0 );

        string absolute, relative, fileName, methodName, nameSpace, objectName;
        Exception::ScriptSection( absolute, relative, fileName, methodName, nameSpace, objectName );
        
        Expect( "Exception::ScriptSection(absolute) is not empty: " + absolute, true, !absolute.isEmpty() );
        Expect( "Exception::ScriptSection(relative) is not empty: " + relative, true, !relative.isEmpty() );
        Expect( "Exception::ScriptSection(fileName) is not empty: " + fileName, true, !fileName.isEmpty() );
        Expect( "Exception::ScriptSection(methodName) matches: " + methodName, true, methodName == "void throwExceptionStack()" );
        Expect( "Exception::ScriptSection(nameSpace) is empty at global scope", true, nameSpace.isEmpty() );
        Expect( "Exception::ScriptSection(objectName) is empty for free functions", true, objectName.isEmpty() );
    }

    try
    {
        someNamespace::throwExceptionFromNamespace();
        Expect( "Throw from namespace catch check", true, false );
    }
    catch
    {
        Expect( "Throw from namespace successfully caught", true, true );
        
        string methodName, nameSpace, objectName;
        Exception::ScriptSection( void, void, void, methodName, nameSpace, objectName );
        
        Expect( "Exception::ScriptSection(nameSpace) extracts correctly: " + nameSpace, true, nameSpace == "someNamespace" );
    }

    try
    {
        someNamespace::OtherNameSpace::throwExceptionFromNamespace();
        Expect( "Throw from nested namespace catch check", true, false );
    }
    catch
    {
        Expect( "Throw from nested namespace successfully caught", true, true );
        
        string methodName, nameSpace, objectName;
        Exception::ScriptSection( void, void, void, methodName, nameSpace, objectName );
        
        Expect( "Exception::ScriptSection(nameSpace) extracts deeply nested hierarchy: " + nameSpace, true, nameSpace == "someNamespace::OtherNameSpace" );
    }

    try
    {
        g_Class.exThrow();
        Expect( "Throw from class catch check", true, false );
    }
    catch
    {
        Expect( "Throw from class successfully caught", true, true );
        
        string objectName;
        Exception::ScriptSection( void, void, void, void, void, objectName );
        
        Expect( "Exception::ScriptSection(objectName) identifies class context: " + objectName, true, objectName == "someClass" );
    }

    try
    {
        SomeClassNameSpace::ClassSpace.exThrow();
        Expect( "Throw from class in a namespace catch check", true, false );
    }
    catch
    {
        Expect( "Throw from class in a namespace successfully caught", true, true );
        
        string nameSpace, objectName;
        Exception::ScriptSection( void, void, void, void, nameSpace, objectName );
        
        Expect( "Exception::ScriptSection(nameSpace) targets parent scope: " + nameSpace, true, nameSpace == "SomeClassNameSpace" );
        Expect( "Exception::ScriptSection(objectName) targets correct subclass: " + objectName, true, objectName == "SomeClass2" );
    }

    try
    {
        Exception::Throw( "Ex" );
        Expect( "Clear() data catch check", true, false );
    }
    catch
    {
        Exception::Clear();

        // CORRECCIÓN CRÍTICA: Las variables nativas de salida deben consultarse 
        // DESPUÉS de invocar Clear() para verificar que el API de C++ devuelva strings vacíos.
        string absolute, relative, fileName, methodName, nameSpace, objectName, message;
        message = Exception::Message();
        Exception::ScriptSection( absolute, relative, fileName, methodName, nameSpace, objectName );

        Expect( "Exception::Clear() flushes absolute path", true, absolute.isEmpty() );
        Expect( "Exception::Clear() flushes relative path", true, relative.isEmpty() );
        Expect( "Exception::Clear() flushes file name", true, fileName.isEmpty() );
        Expect( "Exception::Clear() flushes method name", true, methodName.isEmpty() );
        Expect( "Exception::Clear() flushes namespace context", true, nameSpace.isEmpty() );
        Expect( "Exception::Clear() flushes object target context", true, objectName.isEmpty() );
        Expect( "Exception::Clear() flushes standard message", true, message.isEmpty() );
        Expect( "Exception::Clear() unlinks metadata dictionary handle", true, Exception::Dictionary() is null );
    }

    try
    {
        SomeClassNameSpace::ClassSpace.exThrow();
        Expect( "Call stack string catch check", true, false );
    }
    catch
    {
        string stack = Exception::CallStack();
        Expect( "Exception::CallStack() returns a descriptive trace string", true, !stack.isEmpty() );
    }

    try
    {
        // Se asume la existencia de la sintaxis abreviada de inicialización de diccionarios en tu entorno
        dictionary metadata = { { "err", "Missing quote at line 24" } };
        Exception::Throw( "Exception from AS", @metadata );
        Expect( "Exception::Throw(string, dictionary@) catch check", true, false );
    }
    catch
    {
        dictionary@ data = Exception::Dictionary();
        bool dictionaryIsValid = ( data !is null && data.exists( "err" ) );
        
        string errorString = "";
        if ( dictionaryIsValid )
        {
            data.get( "err", errorString );
        }

        Expect( "Exception::Dictionary() metadata handle fetched successfully: " + errorString, true, dictionaryIsValid && errorString == "Missing quote at line 24" );
    }
}

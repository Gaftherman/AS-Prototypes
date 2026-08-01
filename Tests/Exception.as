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

    string t = "Exception::Throw(string)";

    try
    {
        throwExceptionStack();
        Expect( t, true, false );
    }
    catch
    {
        Expect( t, true, true );

        Expect( "Exception::Message(): " + Exception::Message(), true, !Exception::Message().isEmpty() );
        string absolute, relative, fileName, methodName, nameSpace, objectName;
        Exception::ScriptSection( absolute, relative, fileName, methodName, nameSpace, objectName );
        Expect( "Exception::ScriptSection(absolute): " + absolute, true, !absolute.isEmpty() );
        Expect( "Exception::ScriptSection(relative): " + relative, true, !relative.isEmpty() );
        Expect( "Exception::ScriptSection(fileName): " + fileName, true, !fileName.isEmpty() );
        Expect( "Exception::ScriptSection(methodName): " + methodName, true, !methodName.isEmpty() );
        Expect( "Exception::ScriptSection(nameSpace): empty" + nameSpace, true, nameSpace.isEmpty() );
        Expect( "Exception::ScriptSection(objectName): empty" + objectName, true, objectName.isEmpty() );
    }

    t = "Throw from namespace";

    try
    {
        someNamespace::throwExceptionFromNamespace();
        Expect( t, true, false );
    }
    catch
    {
        Expect( t, true, true );
        string methodName, nameSpace, objectName;
        Exception::ScriptSection( void, void, void, methodName, nameSpace, objectName );
        Expect( "Exception::ScriptSection(nameSpace): " + nameSpace, true, !nameSpace.isEmpty() );
    }

    t = "Throw from nested namespace";

    try
    {
        someNamespace::OtherNameSpace::throwExceptionFromNamespace();
        Expect( t, true, false );
    }
    catch
    {
        Expect( t, true, true );
        string methodName, nameSpace, objectName;
        Exception::ScriptSection( void, void, void, methodName, nameSpace, objectName );
        Expect( "Exception::ScriptSection(nameSpace): " + nameSpace, true, !nameSpace.isEmpty() );
    }

    t = "Throw from class";

    try
    {
        g_Class.exThrow();
        Expect( t, true, false );
    }
    catch
    {
        Expect( t, true, true );
        string objectName;
        Exception::ScriptSection( void, void, void, void, void, objectName );
        Expect( "Exception::ScriptSection(objectName): " + objectName, true, !objectName.isEmpty() );
    }

    t = "Throw from class in a namespace";

    try
    {
        SomeClassNameSpace::ClassSpace.exThrow();
        Expect( t, true, false );
    }
    catch
    {
        Expect( t, true, true );
        string nameSpace, objectName;
        Exception::ScriptSection( void, void, void, void, nameSpace, objectName );
        Expect( "Exception::ScriptSection(nameSpace): " + nameSpace, true, !nameSpace.isEmpty() );
        Expect( "Exception::ScriptSection(objectName): " + objectName, true, !objectName.isEmpty() );
    }

    t = "Clear() all data ";

    try
    {
        Exception::Throw( "Ex" );
        Expect( t, true, false );
    }
    catch
    {

        string absolute, relative, fileName, methodName, nameSpace, objectName, message;

        message = Exception::Message();
        Exception::ScriptSection( absolute, relative, fileName, methodName, nameSpace, objectName );

        Exception::Clear();

        message = Exception::Message();
        Exception::ScriptSection( absolute, relative, fileName, methodName, nameSpace, objectName );

        Expect( t + "absolute: " + absolute, true, absolute.isEmpty() );
        Expect( t + "relative: " + relative, true, relative.isEmpty() );
        Expect( t + "fileName: " + fileName, true, fileName.isEmpty() );
        Expect( t + "methodName: " + methodName, true, methodName.isEmpty() );
        Expect( t + "nameSpace: " + nameSpace, true, nameSpace.isEmpty() );
        Expect( t + "objectName: " + objectName, true, objectName.isEmpty() );
        Expect( t + "message: " + message, true, message.isEmpty() );

        // -TODO Add dictionary check, line check, stack check & class check if added to source.
    }

    t = "Call stack string";

    try
    {
        SomeClassNameSpace::ClassSpace.exThrow();
        Expect( t, true, false );
    }
    catch
    {
        string stack = Exception::CallStack();
        Expect( "Exception::CallStack(): " + stack, true, !stack.isEmpty() );
    }

    t = "Exception::Throw(string, dictionary@): ";

    try
    {
        Exception::Throw( "Exception from AS", { { "err", "Missing quote at line 24" } } );
        Expect( t, true, false );
    }
    catch
    {
        dictionary@ data = Exception::Dictionary();
        Expect( t + ( data !is null ? string( data[ "err" ] ) : "" ), true, data !is null && data.exists( "err" ) );
    }
}

#include "utils/title.as"

using namespace Tests;

bool throwTest()
{
    try
    {
        SetException( "Exception from AS" );
    }
    catch
    {
#if FALSE
        Exception@ ex = GetException();

        Console::WriteLine( "ex.func: " + ex.func );
        Console::WriteLine( "ex.line: " + ex.line );
        Console::WriteLine( "ex.sect: " + ex.sect );
        Console::WriteLine( "ex.message: " + ex.message );
        Console::WriteLine( "ex.stack: " + ex.stack );
#endif

        return true;
    }

    return false;
}

void main()
{
    title( "Exception handler class" );

    Expect( "Exception catch", true, throwTest() );
}

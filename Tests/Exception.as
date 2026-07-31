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

        Console::PrintLine( "ex.func: " + ex.func );
        Console::PrintLine( "ex.line: " + ex.line );
        Console::PrintLine( "ex.sect: " + ex.sect );
        Console::PrintLine( "ex.message: " + ex.message );
        Console::PrintLine( "ex.stack: " + ex.stack );
#endif

        return true;
    }

    return false;
}

void main()
{
    Console::PrintLine("--- AngelScript Exception Test ---");

    Expect( "Exception catch", true, throwTest() );
}

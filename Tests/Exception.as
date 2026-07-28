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

        println( "ex.func: " + ex.func );
        println( "ex.line: " + ex.line );
        println( "ex.sect: " + ex.sect );
        println( "ex.message: " + ex.message );
        println( "ex.stack: " + ex.stack );
#endif

        return true;
    }

    return false;
}

void main()
{
    println("--- AngelScript Exception Test ---");

    Expect( "Exception catch", true, throwTest() );
}

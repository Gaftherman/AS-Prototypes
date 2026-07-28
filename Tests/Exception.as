void main()
{
    println("--- AngelScript Exception Test ---");

    try {
        SetException( "Exception from AS" );
        Tests::Fails++;
    } catch {
        println( "Catched exception" );
        Tests::Passes++;

        Exception@ ex = GetException();

        println( "ex.func: " + ex.func );
        println( "ex.line: " + ex.line );
        println( "ex.sect: " + ex.sect );
        println( "ex.message: " + ex.message );
        println( "ex.stack: " + ex.stack );
    }
}

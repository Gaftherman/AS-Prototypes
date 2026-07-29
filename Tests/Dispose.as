using namespace Tests;

class SomeClass
{
    ~SomeClass()
    {
        println( "SomeClass destroyed." );
    }
}

void main()
{
    println("--- AngelScript Dispose Test ---");

#if FALSE
    SomeClass@ someClassHandle = SomeClass();
    Expect( "Empty optional", false, Dispose( someClassHandle ) && someClassHandle is null );
#endif

    try
    {
        SomeClass someClass();
        // We should not be able to dispose of a non-handle object.
        // Or maybe to remove all other refeences except the given someClass input
        Dispose( someClass );
        Fails++;
    }
    catch
    {
        Passes++;
    }

    try
    {
        int i = 0;
        Dispose(i);
        Fails++;
    }
    catch
    {
        Passes++;
    }

    try
    {
        string s = "s";
        Dispose(s);
        Fails++;
    }
    catch
    {
        Passes++;
    }
}

#include "utils/title.as"

using namespace Tests;

class SomeClass
{
    string name = "";

    SomeClass() {}

    SomeClass( string n )
    {
        this.name = n;
    }

    ~SomeClass()
    {
        if( this.name != "" )
        {
            Console.Back.rgb( 50, 50, 50 )
                .Fore.rgb( 200, 200, 50 )
                .WriteLine( this.name + " destroyed." )
            .ResetColor();
        }
    }
}

void main()
{
    title( "Dispose test" );

    try
    {
        int i = 0;
        Dispose(i);
        Expect( "invalid Dispose of a primitive", true, false );
    }
    catch
    {
        Expect( "primitive exception: " + Exception::Message(), true, true );
    }

// -TODO Fix Dispose :aaagabe
#if FALSE
    SomeClass someClassConst( "static class" );

    try
    {
        Dispose(someClassConst);
        Expect( "invalid Dispose of a static class", true, false );
    }
    catch
    {
        Expect( "primitive exception: " + Exception::Message(), true, someClassConst.name != "" );
    }

    SomeClass@ someClassConstHandle = SomeClass( "handle class" );

    try
    {
        Dispose(someClassConstHandle);
        Expect( "invalid Dispose of a handle class", true, false );
    }
    catch
    {
        Expect( "primitive exception: " + Exception::Message(), true, someClassConstHandle is null || someClassConstHandle.name != "" );
    }
#endif
}

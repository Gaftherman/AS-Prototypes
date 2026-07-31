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
            Console::SetColor( Console::Color::BackGround, 50, 50, 50 );
            Console::SetColor( Console::Color::ForeGround, 200, 200, 50 );
            Console::WriteLine( this.name + " destroyed." );
            Console::ResetColor();
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
        Exception@ ex = GetException();
        Expect( "primitive exception: " + ex.message, true, true );
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
        Exception@ ex = GetException();
        Expect( "primitive exception: " + ex.message, true, someClassConst.name != "" );
    }

    SomeClass@ someClassConstHandle = SomeClass( "handle class" );

    try
    {
        Dispose(someClassConstHandle);
        Expect( "invalid Dispose of a handle class", true, false );
    }
    catch
    {
        Exception@ ex = GetException();
        Expect( "primitive exception: " + ex.message, true, someClassConstHandle is null || someClassConstHandle.name != "" );
    }
#endif
}

#include "utils/title.as"

using namespace Tests;

class SomeClass
{
    ~SomeClass()
    {
        Console::SetColor( Console::Color::BackGround, 50, 50, 50 );
        Console::SetColor( Console::Color::ForeGround, 200, 200, 50 );
        Console::WriteLine( "SomeClass destroyed." );
        Console::ResetColor();
    }
}

void main()
{
    title( "Dispose test" );

#if FALSE
    SomeClass@ someClassHandle = SomeClass();
    Expect( "Dispose of a 1-ref handlel", false, Dispose( someClassHandle ) && someClassHandle is null );
#endif

    try
    {
        SomeClass someClass();
        // We should not be able to dispose of a non-handle object.
        // Or maybe to remove all other refeences except the given someClass input
        Dispose( someClass );
        Expect( "Dispose of a static class", true, false );
    }
    catch
    {
        Expect( "Dispose of a static class", true, true );
    }

    try
    {
        int i = 0;
        Dispose(i);
        Expect( "Dispose of a primitive", true, false );
    }
    catch
    {
        Expect( "Dispose of a primitive", true, true );
    }
}

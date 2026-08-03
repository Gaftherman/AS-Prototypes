#include "utils/title.as"

using namespace Tests;

void main()
{
    title( "console_t and console_color_t class tests" );

    Expect( "Console global handle is valid and initialized", true, Console !is null );
    Expect( "Console.Fore color property handle is valid", true, Console.Fore !is null );
    Expect( "Console.Back color property handle is valid", true, Console.Back !is null );

    // These should point to a unique C++ pointer
    console_t@ writeRef = Console.Write( "Testing Write implementation... " );
    Expect( "Console.Write returns a valid handle to console_t", true, writeRef is Console );

    console_t@ writeLineRef = Console.WriteLine( "Testing WriteLine with message." );
    Expect( "Console.WriteLine with string returns a valid handle to console_t", true, writeLineRef is Console );

    console_t@ emptyLineRef = Console.WriteLine();
    Expect( "Console.WriteLine empty overload returns a valid handle to console_t", true, emptyLineRef is Console );

    console_t@ foreRgbRef = Console.Fore.rgb( 0, 255, 0 );
    Expect( "Console.Fore.rgb returns the main console_t handle for chaining", true, foreRgbRef is Console );
    Console.WriteLine( "This text should be rendered in green color!" );

    console_t@ backRgbRef = Console.Back.rgb( 0, 0, 255 );
    Expect( "Console.Back.rgb returns the main console_t handle for chaining", true, backRgbRef is Console );
    Console.WriteLine( "This text should have a blue background!" );

    console_t@ foreResetRef = Console.Fore.Reset();
    Expect( "Console.Fore.Reset returns the main console_t handle", true, foreResetRef is Console );

    console_t@ backResetRef = Console.Back.Reset();
    Expect( "Console.Back.Reset returns the main console_t handle", true, backResetRef is Console );
    Console.WriteLine( "Console colors have been restored to default individually." );

    console_t@ resetAllRef = Console.ResetColor();
    Expect( "Console.ResetColor without arguments returns the main console_t handle", true, resetAllRef is Console );

    console_t@ resetSpecificRef = Console.ResetColor( Console.Fore );
    Expect( "Console.ResetColor passing Fore handle returns the main console_t handle", true, resetSpecificRef is Console );

    console_t@ resetSpecificRef2 = Console.ResetColor( Console.Back );
    Expect( "Console.ResetColor passing Back handle returns the main console_t handle", true, resetSpecificRef2 is Console );

    Console.WriteLine( "Starting fluid chain execution test..." );

    Console.Fore.rgb(3, 150, 213).Write( "[Notice] " ).ResetColor( ).Write( "T" ).Write( "h" ).Write( "i" ).Write( "s" ).Write( " " ).Write( "i" ).Write( "s" ).Write( " " ).Write( "a" ).Write( " " ).Write( "m" ).Write( "a" ).Write( "s" ).Write( "s" ).Write( "i" ).Write( "v" ).Write( "e" ).Write( " " ).Write( "l" ).Write( "o" ).Write( "n" ).Write( "g" ).Write( " " ).Write( "W" ).Write( "r" ).Write( "i" ).Write( "t" ).Write( "e" ).Write( "(" ).Write( "." ).Write( "." ).Write( "." ).Write( ")" ).Write( " " ).Write( "c" ).Write( "h" ).Write( "a" ).Write( "i" ).Write( "n" ).Write( "." ).WriteLine();

    Expect( "Fluid API chaining executed successfully without reference crashes", true, true );
}

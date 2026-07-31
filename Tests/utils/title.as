void title( const string&in title )
{
    Console::SetColor( Console::Color::ForeGround, 0, 255, 0 );
    Console::Write("--- " );
    Console::SetColor( Console::Color::ForeGround, 250, 0, 150 );
    Console::Write(title );
    Console::SetColor( Console::Color::ForeGround, 0, 255, 0 );
    Console::Write(" ---" );
    Console::ResetColor();
    Console::WriteLine();
}

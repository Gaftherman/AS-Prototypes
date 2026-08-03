void title( const string&in title )
{
    Console
        .Fore.rgb( 0, 255, 0 ).Write("--- " )
        .Fore.rgb( 250, 0, 150 ).Write( title )
        .Fore.rgb( 0, 255, 0 ).Write(" ---" )
        .ResetColor()
    .WriteLine();
}

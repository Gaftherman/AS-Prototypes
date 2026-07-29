using namespace Tests;

void main()
{
    println("--- AngelScript json Test ---");

    JSON@ validObject = json::loads( "{\"this is a valid key\":\"in a valid object\"}" );
    Expect( "json::loads valid", true, validObject !is null );

    string deserialized = json::dumps( validObject );
    Expect( "json::dumps valid", true, deserialized != "" );
    println( "Serialized: " + deserialized );

    bool result = true;

    try {
        json::dumps( null );
        result = false;
    } catch {
        result = true;
    }
    Expect( "json::dumps with null JSON@", true, result );

    Expect( "json AS handle instantiation", true, @JSON() !is null );

    JSON jsn;
    Expect( "json AS static instantiation", true, @jsn !is null );

    try {
        json::loads( "{\"invalid\":value}" );
        result = false;
    } catch {
        result = true;
    }
    Expect( "json::loads with errors", true, result );

#if FALSE
    // -TODO Set some invalid characters
    JSON@ objInvalidCharacter = json::loads( "{\"a\":\"ñ\"}" );

    try {
        json::dumps( objInvalidCharacter ); // strict by default. ññññññ
        result = false;
    } catch {
        result = true;
    }
    Expect( "json::dumps with strict mode", true, result );
    Expect( "json::dumps with replace mode", true, json::dumps( objInvalidCharacter, json::error_handler::replace ) != "" );
    Expect( "json::dumps with ignore mode", true, json::dumps( objInvalidCharacter, json::error_handler::ignore ) != "" );
#endif

    JSON@ objConst = JSON();
    Expect( "JSON::ToString() const", true, objConst.ToString() == "null" );
    Expect( "JSON::ToString() (null internal json)", true, JSON().ToString() == "null" );
    @objConst = json::loads( "[1]" );
    Expect( "JSON::ToString() valid", true, objConst.ToString() == "[1]" );

}

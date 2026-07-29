using namespace Tests;

void main()
{
    println("--- AngelScript json Test ---");

    JSON@ validObject = json::loads( "{\"this is a valid key\":\"in a valid object\"}" );
    Expect( "json::loads valid", true, validObject !is null );

    string deserialized = json::dumps( validObject );
    Expect( "json::dumps valid", true, deserialized != "" );
//    println( "Serialized: " + deserialized );

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

    JSON@ obj = JSON();
    Expect( "JSON::to_string() const", true, obj.to_string() == "null" );
    Expect( "JSON::to_string() (null internal json)", true, JSON().to_string() == "null" );
    @obj = json::loads( "[1]" );
    Expect( "JSON::to_string() valid", true, obj.to_string() == "[1]" );

    Expect( "JSON::strict default true", true, obj.strict );
}

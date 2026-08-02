#include "utils/title.as"

using namespace Tests;

void main()
{
    title( "JSON class & methods" );

    JSON@ validObject = json::loads( "{\"this is a valid key\":\"in a valid object\"}" );
    Expect( "json::loads valid", true, validObject !is null );

    @validObject = json::load( "Tests/json/load.json" );
    Expect( "json::load valid", true, validObject !is null && validObject.to_string() == "[\"load\"]" );

    string deserialized = json::dumps( validObject );
    Expect( "json::dumps valid", true, deserialized == "[\"load\"]" );
//    Console::WriteLine( "Serialized: " + deserialized );

    Expect( "json::dump valid", true, json::dump(validObject, "Tests/json/dump.json" ) && ( @validObject = json::load( "Tests/json/dump.json" ) ) !is null && validObject.to_string() == "[\"load\"]" );

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

    Expect( "JSON::m_ErrorHandlerMode default to strict", true, obj.m_ErrorHandlerMode == json::error_handler::strict );

    Expect( "JSON::JSON( float )", true, @JSON(1.0f) !is null );
    Expect( "JSON::JSON( int )", true, @JSON(1) !is null );
    Expect( "JSON::JSON( bool )", true, @JSON(true) !is null );
    Expect( "JSON::JSON( string )", true, @JSON("empty") !is null );
    Expect( "JSON::JSON( string, serialized )", true, @JSON("{}", true) !is null );

    array<int> arrint = { -1 };
    @obj = JSON(arrint);
    Expect( "JSON::JSON( array<int> )", true, obj !is null && obj.to_string() == "[-1]" );

    array<uint> arruint = { 1 };
    @obj = JSON(arruint);
    Expect( "JSON::JSON( array<uint> )", true, obj !is null && obj.to_string() == "[1]" );

    array<bool> arrbool = { true };
    @obj = JSON(arrbool);
    Expect( "JSON::JSON( array<bool> )", true, obj !is null && obj.to_string() == "[true]" );

    array<float> arrfloat = { 0.5f };
    @obj = JSON(arrfloat);
    Expect( "JSON::JSON( array<float> )", true, obj !is null && obj.to_string() == "[0.5]" );

    array<double> arrdouble = { 0.123456789012345 };
    @obj = JSON(arrdouble);
    Expect( "JSON::JSON( array<double> )", true, obj !is null && obj.to_string() == "[0.123456789012345]" );

    array<string> arrstring = { "string" };
    @obj = JSON(arrstring);
    Expect( "JSON::JSON( array<string> )", true, obj !is null && obj.to_string() == "[\"string\"]" );
}

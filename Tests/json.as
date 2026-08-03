#include "utils/title.as"

using namespace Tests;

void main()
{
    title( "JSON class & methods" );

    JSON@ validObject = json::loads( "{\"this is a valid key\":\"in a valid object\"}" );
    Expect( "json::loads parses valid json object format", true, validObject !is null );

    if ( validObject !is null )
    {
        Expect( "Parsed json detects as structured object", true, validObject.is_object() );
        Expect( "Parsed json confirms as structured container", true, validObject.is_structured() );
        Expect( "Parsed json !is primitive", false, validObject.is_primitive() );
    }

    @validObject = json::load( "json/load.json" );
    bool isLoadValid = ( validObject !is null && validObject.to_string() == "[\"load\"]" );
    Expect( "json::load reads and parses explicit disk file path", true, isLoadValid );

    string deserialized = json::dumps( validObject );
    Expect( "json::dumps serializes object back to text representation", true, deserialized == "[\"load\"]" );

    bool dumpSuccess = json::dump( validObject, "json/dump.json" );
    Expect( "json::dump writes structured content to disk", true, dumpSuccess );

    JSON@ verifiedDumpObj = json::load( "json/dump.json" );
    bool isDumpVerified = ( verifiedDumpObj !is null && verifiedDumpObj.to_string() == "[\"load\"]" );
    Expect( "json::load successfully reads back the dumped serialization", true, isDumpVerified );

    bool catchTriggered = false;
    try 
    {
        json::dumps( null );
    } 
    catch
    {
        catchTriggered = true;
    }
    Expect( "json::dumps throws a script exception when receiving a null handle", true, catchTriggered );

    catchTriggered = false;
    try 
    {
        json::loads( "{\"invalid\":value}" );
    } 
    catch
    {
        catchTriggered = true;
    }
    Expect( "json::loads throws an exception on invalid syntax structural tokens", true, catchTriggered );

    Expect( "JSON instantiation via explicit handle instantiation factory", true, @JSON() !is null );

    JSON jsn;
    Expect( "JSON instantiation via static default stack construct", true, @jsn !is null );

    JSON@ obj = JSON();
    Expect( "Default constructor serializes to null json string equivalent", true, obj.to_string() == "null" );
    Expect( "Default instantiated object is recognized as json null type", true, obj.is_null() );
    Expect( "Default instantiated object is recognized as primitive", true, obj.is_primitive() );

    @obj = json::loads( "[1]" );
    Expect( "JSON::to_string() matches explicit array notation text after load", true, obj.to_string() == "[1]" );
    Expect( "JSON::is_array() evaluates true for array constructs", true, obj.is_array() );

    Expect( "JSON::m_ErrorHandlerMode initializes to strict strategy by default", true, obj.m_ErrorHandlerMode == json::error_handler::strict );

    JSON@ primitiveObj = JSON( 1.0f );
    Expect( "JSON numeric float initialization handle is valid", true, primitiveObj !is null );
    Expect( "JSON tracks float instance as floating point precision number", true, primitiveObj.is_number_float() );

    @primitiveObj = JSON( 1 );
    Expect( "JSON numeric integer initialization handle is valid", true, primitiveObj !is null );
    Expect( "JSON tracks int instance as standard integer number", true, primitiveObj.is_number_integer() );

    @primitiveObj = JSON( true );
    Expect( "JSON boolean initialization handle is valid", true, primitiveObj !is null );
    Expect( "JSON tracks bool instance as conditional boolean value", true, primitiveObj.is_boolean() );

    @primitiveObj = JSON( "empty" );
    Expect( "JSON explicit string constructor allocation handle is valid", true, primitiveObj !is null );
    Expect( "JSON tracks standard string data as text value type", true, primitiveObj.is_string() );

    @primitiveObj = JSON( "{}", true );
    Expect( "JSON implicit inline initialization constructor with override flag", true, primitiveObj !is null );

    array<int> arrint = { -1 };
    @obj = JSON( arrint );
    Expect( "JSON serialization array<int> converts to valid string representation", true, obj !is null && obj.to_string() == "[-1]" );
    Expect( "JSON collection correctly flagged as array type", true, obj.is_array() );

    array<uint> arruint = { 1 };
    @obj = JSON( arruint );
    Expect( "JSON serialization array<uint> handles unsigned formatting correctly", true, obj !is null && obj.to_string() == "[1]" );

    array<bool> arrbool = { true };
    @obj = JSON( arrbool );
    Expect( "JSON serialization array<bool> preserves literal structural terms", true, obj !is null && obj.to_string() == "[true]" );

    array<float> arrfloat = { 0.5f };
    @obj = JSON( arrfloat );
    Expect( "JSON serialization array<float> maps decimal floating tokens clean", true, obj !is null && obj.to_string() == "[0.5]" );

    array<double> arrdouble = { 0.123456789012345 };
    @obj = JSON( arrdouble );
    Expect( "JSON serialization array<double> preserves floating fractional data limits", true, obj !is null && obj.to_string() == "[0.123456789012345]" );

    array<string> arrstring = { "string" };
    @obj = JSON( arrstring );
    Expect( "JSON serialization array<string> encloses text entries inside internal syntax quotes", true, obj !is null && obj.to_string() == "[\"string\"]" );
}


#include <fstream>
#include <sstream>
#include <iostream>

#include "addons/ASJSON.hpp"
#include "CASDocRegistry.hpp"

class CASDocJSON : public CASDocRegistry
{
    bool Register() override
    {
        ASJSON::FILESYSTEM_LOAD_CALLBACK = []( std::filesystem::path& path, std::string& content, std::string& err ) -> bool
        {
            if( std::ifstream file(path); file.is_open() )
            {
                std::stringstream buffer;
                buffer << file.rdbuf();
                content = buffer.str(); 
                return true;
            }
            err = "Unexistent file";
            return false;
        };
        ASJSON::FILESYSTEM_DUMP_CALLBACK = []( std::filesystem::path& path, std::string& content, std::string& err ) -> bool
        {
            if( std::ofstream file(path); file.is_open() )
            {
                file << content;
                return true;
            }
            err = "File is read-only";
            return false;
        };

        return
        RegisterObjectType(
            "json value container supporting null, boolean, number, string, array, and object handles."sv,
            "json",
            0,
            asOBJ_REF
        ) &&
        RegisterObjectBehaviour(
            "json",
            asBEHAVE_ADDREF,
            "void f()",
            asMETHOD( ASJSON, AddRef ),
            asCALL_THISCALL
        ) &&
        RegisterObjectBehaviour(
            "json",
            asBEHAVE_RELEASE,
            "void f()",
            asMETHOD( ASJSON, Release ),
            asCALL_THISCALL
        ) &&
        RegisterObjectBehaviour(
            "Constructs a null json value."sv,
            "json",
            asBEHAVE_FACTORY,
            "json@ f()",
            asFUNCTION( ASJSON::JSONFactory ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs a json value from a string or serialized json representation."sv,
            "json",
            asBEHAVE_FACTORY,
            "json@ f( const string &in value, bool isSerialized = false )",
            asFUNCTION( ASJSON::JSONFactoryString ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "json",
            asBEHAVE_FACTORY,
            "json@ f( int value )",
            asFUNCTION( ASJSON::JSONFactoryInt ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "json",
            asBEHAVE_FACTORY,
            "json@ f( float value )",
            asFUNCTION( ASJSON::JSONFactoryFloat ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "json",
            asBEHAVE_FACTORY,
            "json@ f( bool value )",
            asFUNCTION( ASJSON::JSONFactoryBool ),
            asCALL_CDECL
        ) &&
        // array<T> register factory (Not dynamic)
        RegisterObjectBehaviour(
            "Constructs a json array value."sv,
            "json",
            asBEHAVE_FACTORY,
            "json@ f(const array<int>&in)",
            asFUNCTION( ASJSON::JSONFactoryArray ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs a json array value."sv,
            "json",
            asBEHAVE_FACTORY,
            "json@ f(const array<uint>&in)",
            asFUNCTION( ASJSON::JSONFactoryArray ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs a json array value."sv,
            "json",
            asBEHAVE_FACTORY,
            "json@ f(const array<bool>&in)",
            asFUNCTION( ASJSON::JSONFactoryArray ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs a json array value."sv,
            "json",
            asBEHAVE_FACTORY,
            "json@ f(const array<float>&in)",
            asFUNCTION( ASJSON::JSONFactoryArray ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs a json array value."sv,
            "json",
            asBEHAVE_FACTORY,
            "json@ f(const array<string>&in)",
            asFUNCTION( ASJSON::JSONFactoryArray ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs a json array value."sv,
            "json",
            asBEHAVE_FACTORY,
            "json@ f(const array<double>&in)",
            asFUNCTION( ASJSON::JSONFactoryArray ),
            asCALL_CDECL
        ) &&
        SetDefaultNamespace(
            "json" ) &&
        RegisterGlobalFunction(
            "Parses a serialized json string."sv,
            "json@ loads( const string&in serialized, bool ignore_comments = true )",
            asFUNCTION( ASJSON::loads ),
            asCALL_CDECL
        ) &&
        [&]() -> bool {
            if( ASJSON::FILESYSTEM_LOAD_CALLBACK.has_value() )
            {
                return RegisterGlobalFunction(
                    "Loads and parses a json file from the filesystem."sv,
                    "json@ load( const string&in filePath, bool ignore_comments = true )",
                    asFUNCTION( ASJSON::load ),
                    asCALL_CDECL
                );
            }
            return true;
        }() &&
        RegisterEnum(
            "Specifies error handling strategy during json serialization and deserialization."sv,
            "error_handler"
        ) &&
        RegisterEnumValue(
            "Strict, raises exceptions when anything wrong happens."sv,
            "error_handler",
            "strict",
            static_cast<int>( ASJSON::error_handler::strict )
        ) &&
        RegisterEnumValue(
            "Adaptative, avoids raising exceptions at all costs. returns default values if empty, converts numerical values to the getter, converts utf8 text, and makes various other operations more safer but lot of things could silently fail. consider using Permisive instead."sv,
            "error_handler",
            "coerce",
            static_cast<int>( ASJSON::error_handler::coerce )
        ) &&
        RegisterEnumValue(
            "Permisive, avoids raising exceptions but remains strict in general. returning null pointers, removes invalid utf8. writes error messages to the output callback if provided."sv,
            "error_handler",
            "permissive",
            static_cast<int>( ASJSON::error_handler::permissive )
        ) &&
        RegisterGlobalFunction(
            "Serializes a json object into a formatted string."sv,
            "string dumps( const json@ obj, int indents = -1, error_handler errors = error_handler::strict )",
            asFUNCTION( ASJSON::dumps ),
            asCALL_CDECL
        ) &&
        [&]() -> bool {
            if( ASJSON::FILESYSTEM_DUMP_CALLBACK.has_value() )
            {
                return RegisterGlobalFunction(
                    "Serializes a json object and writes it to a file."sv,
                    "bool dump( const json@ obj, const string&in filePath, int indents = -1, error_handler errors = error_handler::strict )",
                    asFUNCTION( ASJSON::dump ),
                    asCALL_CDECL
                );
            }
            return true;
        }() &&
        SetDefaultNamespace( "" ) &&
        RegisterObjectMethod(
            "Returns a string representation of the json object."sv,
            "json",
            "string to_string() const",
            asMETHOD( ASJSON, to_string ),
            asCALL_THISCALL
        ) &&
        RegisterObjectProperty(
            "Defines how should json handle errors."sv,
            "json",
            "json::error_handler m_ErrorHandlerMode",
            asOFFSET( ASJSON, m_ErrorHandlerMode)
        ) &&
        RegisterObjectMethod(
            "Returns true if the json value is null."sv,
            "json",
            "bool is_null() const",
            asMETHOD( ASJSON, is_null ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns true if the json value is a boolean."sv,
            "json",
            "bool is_boolean() const",
            asMETHOD( ASJSON, is_boolean ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns true if the json value is a number."sv,
            "json",
            "bool is_number() const",
            asMETHOD( ASJSON, is_number ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns true if the json value is an integer number."sv,
            "json",
            "bool is_number_integer() const",
            asMETHOD( ASJSON, is_number_integer ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns true if the json value is an unsigned integer number."sv,
            "json",
            "bool is_number_unsigned() const",
            asMETHOD( ASJSON, is_number_unsigned ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns true if the json value is a floating point number."sv,
            "json",
            "bool is_number_float() const",
            asMETHOD( ASJSON, is_number_float ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns true if the json value is an object."sv,
            "json",
            "bool is_object() const",
            asMETHOD( ASJSON, is_object ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns true if the json value is an array."sv,
            "json",
            "bool is_array() const",
            asMETHOD( ASJSON, is_array ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns true if the json value is a string."sv,
            "json",
            "bool is_string() const",
            asMETHOD( ASJSON, is_string ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns true if the json value is a primitive type (null, boolean, number, or string)."sv,
            "json",
            "bool is_primitive() const",
            asMETHOD( ASJSON, is_primitive ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns true if the json value is structured (object or array)."sv,
            "json",
            "bool is_structured() const",
            asMETHOD( ASJSON, is_structured ),
            asCALL_THISCALL
        );
    }
};

CASDocRegisterInterface(JSON);

#pragma once
#include "addon_registry.h"
// We use header-only https://github.com/nlohmann/json
#include "includes/json.hpp"

#include <iostream>
#include <sstream>
#include <optional>
#include <filesystem>
#include <functional>

class ASJSON
{
    protected:
        int refCount;

    public:

        using FILESYSTEM_CALLBACK = std::function<bool( std::filesystem::path& path, std::string& content, std::string& err )>;

        /***
         * @brief Set a callback for your own file system when json::load is called.
         * The provided "path" is the absolute path to the file json wants to read.
         * The provided "content" is the string buffer you need to fill with your file system.
         * The provided "err" is the error message for the exception.
         * The return value is whatever we should parse "content" or not.
         */
        static inline std::optional<FILESYSTEM_CALLBACK> FILESYSTEM_LOAD_CALLBACK;

        /***
         * @brief Set a callback for your own file system when json::dump is called.
         * The provided "path" is the absolute path to the file json wants to read.
         * The provided "content" is the string buffer you need write with your file system.
         * The provided "err" is the error message for the exception.
         * The return value is whatever we should parse "content" or not.
         */
        static inline std::optional<FILESYSTEM_CALLBACK> FILESYSTEM_DUMP_CALLBACK;

        enum class error_handler
        {
            strict = 0,
            coerce,
            permissive
        };

        nlohmann::json m_json;
        error_handler m_ErrorHandlerMode = error_handler::strict;

        ASJSON() : refCount(1) { }

        ~ASJSON()
        {
            m_json.clear();
        }

        void AddRef()
        {
            refCount++;
        }

        void Release()
        {
            if( --refCount == 0 )
            {
                delete this;
            }
        }

        // ==================================================================
        // START OF FACTORY
        // ==================================================================

        // Default constructor
        static ASJSON* JSONFactory()
        {
            return new ASJSON();
        }

        static ASJSON* JSONFactoryInt( int value )
        {
            ASJSON* obj = new ASJSON();
            obj->m_json = value;
            return obj;
        }

        static ASJSON* JSONFactoryFloat( float value )
        {
            ASJSON* obj = new ASJSON();
            obj->m_json = value;
            return obj;
        }

        static ASJSON* JSONFactoryBool( bool value )
        {
            ASJSON* obj = new ASJSON();
            obj->m_json = value;
            return obj;
        }

        static ASJSON* JSONFactoryString( const CString& value, bool isSerialized = false )
        {
            if( isSerialized )
            {
                return ASJSON::loads( value );
            }

            ASJSON* obj = new ASJSON();

            obj->m_json = std::string( value.c_str() );

            return obj;
        }

        // array<T> factory
        static ASJSON* JSONFactoryArray( CScriptArray* arr )
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( ctx == nullptr )
                return nullptr;

            asIScriptEngine* engine = ctx->GetEngine();

            if( engine == nullptr )
                return nullptr;

            if( arr == nullptr )
            {
                ctx->SetException( "JSON: null pointer array" );
                return nullptr;
            }

            ASJSON* obj = new ASJSON();

            obj->m_json = nlohmann::json::array();

            int typeId = arr->GetElementTypeId();
            int size = arr->GetSize();

            for( int i = 0; i < size; ++i )
            {
                void* ptr = arr->At(i);

                switch( typeId )
                {
                    case asTYPEID_INT32:
                    {
                        obj->m_json.push_back( *(int*)ptr );
                        continue;
                    }
                    case asTYPEID_UINT32:
                    {
                        int val = *(int*)ptr; // Lazy asign here
                        obj->m_json.push_back( val > 0 ? val : 0 );
                        continue;
                    }
                    case asTYPEID_FLOAT:
                    {
                        obj->m_json.push_back( *(float*)ptr );
                        continue;
                    }
                    case asTYPEID_DOUBLE:
                    {
                        obj->m_json.push_back( *(double*)ptr );
                        continue;
                    }
                    case asTYPEID_BOOL:
                    {
                        obj->m_json.push_back( *(bool*)ptr );
                        continue;
                    }
                    default:
                    {
                        if( typeId == engine->GetTypeIdByDecl( "string" ) )
                        {
                            obj->m_json.push_back( std::string( *(CString*)ptr ) );
                        }
                        else
                        {
                            if( ctx ) ctx->SetException( "JSON: unsupported array type" );
                            delete obj;
                            return nullptr;
                        }
                    }
                }
            }

            return obj;
        }
        // ==================================================================
        // END OF FACTORY
        // ==================================================================

        // Return whatever the file was propertly serialized or raise AS exception if needed.
        static bool dumpInternal( const ASJSON* obj, std::string& serialized, int indents )
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( obj == nullptr )
            {
                if( ctx != nullptr )
                    ctx->SetException( "JSON: Null pointer JSON@ object" );
                return false;
            }

            try
            {
                auto nlohmannErrHandler = nlohmann::json::error_handler_t::strict;

                switch( obj->m_ErrorHandlerMode )
                {
                    case error_handler::coerce:
                        nlohmannErrHandler = nlohmann::json::error_handler_t::replace;
                        break;
                    case error_handler::permissive:
                        nlohmannErrHandler = nlohmann::json::error_handler_t::ignore;
                        break;
                    case error_handler::strict:
                    default:
                        break;
                }

                // I wish nlohmann json had a "separator" char like python.
                serialized = obj->m_json.dump( indents, ' ', false, nlohmannErrHandler );
                return ( serialized.size() > 0 );
            }
            catch( nlohmann::json::exception& exception )
            {
                ctx->SetException( exception.what(), true );
            }

            return false;
        }

        static CString dumps( const ASJSON* obj, int indents = -1 )
        {
            CString str;

            std::string serialized;

            if( ASJSON::dumpInternal( obj, serialized, indents ) )
            {
                str = serialized.c_str();
            }

            return str;
        }

        static bool dump( const ASJSON* obj, const CString& filePath, int indents = -1 )
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( !FILESYSTEM_DUMP_CALLBACK.has_value() )
            {
                if( ctx != nullptr )
                    ctx->SetException( "The application does not implements a file system dump callback.", true );
                return false;
            }

            std::filesystem::path path( std::filesystem::absolute( filePath ) );
            std::string err;
            std::string buffer;

            if( ASJSON::dumpInternal( obj, buffer, indents ) )
            {
                bool result = FILESYSTEM_DUMP_CALLBACK.value()( path, buffer, err );

                if( result )
                    return true;

                std::stringstream errs;
                errs << "JSON ERROR: Could not write file at \"" << path.string() << "\"\n";

                if( !err.empty() )
                    errs << "Reason: " << err << "\n";

                err = errs.str();

                if( ctx != nullptr )
                    ctx->SetException( err.c_str(), true );
            }

            return false;
        }

        static ASJSON* LoadsInternal( char* buffer, bool ignore_comments )
        {
            asIScriptContext* ctx = asGetActiveContext();

            try
            {
                if( auto js = nlohmann::json::parse( buffer, nullptr, true, ignore_comments ); js.is_structured() )
                {
                    ASJSON* obj = new ASJSON();
                    obj->m_json = std::move( js );
                    return obj;
                }
                else
                {
                    if( ctx != nullptr )
                        ctx->SetException( "JSON: Only array or objects can be deserialized.", true );
                }
            }
            catch( nlohmann::json::parse_error& exception )
            {
                if( ctx != nullptr )
                    ctx->SetException( exception.what(), true );
            }

            return nullptr;
        }

        static ASJSON* loads( const CString& serialized, bool ignore_comments = true )
        {
            return ASJSON::LoadsInternal( (char*)serialized.c_str(), ignore_comments );
        }

        static ASJSON* load( const CString& filePath, bool ignore_comments = true )
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( !FILESYSTEM_LOAD_CALLBACK.has_value() )
            {
                if( ctx != nullptr )
                    ctx->SetException( "The application does not implements a file system load callback.", true );
                return nullptr;
            }

            std::filesystem::path path( std::filesystem::absolute( filePath ) );
            std::string err;
            std::string buffer;

            bool result = FILESYSTEM_LOAD_CALLBACK.value()( path, buffer, err );

            if( result )
                return ASJSON::LoadsInternal( (char*)buffer.c_str(), ignore_comments );

            std::stringstream errs;
            errs << "JSON ERROR: Could not read file at \"" << path.string() << "\"\n";

            if( !err.empty() )
                errs << "Reason: " << err << "\n";

            err = errs.str();

            if( ctx != nullptr )
                ctx->SetException( err.c_str(), true );

            return nullptr;
        }

        // ==================================================================
        // START OF CONVERSIONS
        // ==================================================================

        // Alias to dumps
        CString to_string()
        {
            return ASJSON::dumps(this, -1);
        }
        // ==================================================================
        // END OF CONVERSIONS
        // ==================================================================

        // ==================================================================
        // START OF BUILDER PATTERNS
        // ==================================================================
        // ==================================================================
        // END OF BUILDER PATTERNS
        // ==================================================================

        bool is_null() const {
            return this->m_json.is_null();
        }
        bool is_boolean() const {
            return this->m_json.is_boolean();
        }
        bool is_number() const {
            return this->m_json.is_number();
        }
        bool is_number_integer() const {
            return this->m_json.is_number_integer();
        }
        bool is_number_unsigned() const {
            return this->m_json.is_number_unsigned();
        }
        bool is_number_float() const {
            return this->m_json.is_number_float();
        }
        bool is_object() const {
            return this->m_json.is_object();
        }
        bool is_array() const {
            return this->m_json.is_array();
        }
        bool is_string() const {
            return this->m_json.is_string();
        }
        bool is_primitive() const {
            return this->m_json.is_primitive();
        }
        bool is_structured() const {
            return this->m_json.is_structured();
        }

        static inline void Register( asIScriptEngine* engine )
        {
            REGISTER_OBJECT_TYPE( "JSON", 0, asOBJ_REF, "JSON value container supporting null, boolean, number, string, array, and object types." );
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_ADDREF, "void f()", asMETHOD(ASJSON, AddRef), asCALL_THISCALL, "Increments the reference count of the JSON object." );
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_RELEASE, "void f()", asMETHOD(ASJSON, Release), asCALL_THISCALL, "Decrements the reference count and destroys the JSON object when 0." );

            // Factory Start
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_FACTORY, "JSON@ f()", asFUNCTION(ASJSON::JSONFactory), asCALL_CDECL, "Constructs a null JSON value." );
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_FACTORY, "JSON@ f( const string &in value, bool isSerialized = false )", asFUNCTION(ASJSON::JSONFactoryString), asCALL_CDECL, "Constructs a JSON value from a string or serialized JSON representation." );
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_FACTORY, "JSON@ f( int value )", asFUNCTION(ASJSON::JSONFactoryInt), asCALL_CDECL, "Constructs a JSON integer value." );
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_FACTORY, "JSON@ f( float value )", asFUNCTION(ASJSON::JSONFactoryFloat), asCALL_CDECL, "Constructs a JSON float value." );
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_FACTORY, "JSON@ f( bool value )", asFUNCTION(ASJSON::JSONFactoryBool), asCALL_CDECL, "Constructs a JSON boolean value." );

            // array<T> register factory (Not dynamic)
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_FACTORY, "JSON@ f(const array<int>&in)", asFUNCTION(ASJSON::JSONFactoryArray), asCALL_CDECL, "Constructs a JSON array value." );
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_FACTORY, "JSON@ f(const array<uint>&in)", asFUNCTION(ASJSON::JSONFactoryArray), asCALL_CDECL, "Constructs a JSON array value." );
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_FACTORY, "JSON@ f(const array<bool>&in)", asFUNCTION(ASJSON::JSONFactoryArray), asCALL_CDECL, "Constructs a JSON array value." );
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_FACTORY, "JSON@ f(const array<float>&in)", asFUNCTION(ASJSON::JSONFactoryArray), asCALL_CDECL, "Constructs a JSON array value." );
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_FACTORY, "JSON@ f(const array<string>&in)", asFUNCTION(ASJSON::JSONFactoryArray), asCALL_CDECL, "Constructs a JSON array value." );
            REGISTER_OBJECT_BEHAVIOUR( "JSON", asBEHAVE_FACTORY, "JSON@ f(const array<double>&in)", asFUNCTION(ASJSON::JSONFactoryArray), asCALL_CDECL, "Constructs a JSON array value." );

            // Methods in json namespace.
            engine->SetDefaultNamespace( "json" );
            {
                // Load and parse json using the FileSystem
                REGISTER_GLOBAL_FUNCTION( "JSON@ load( const string&in filePath, bool ignore_comments = true )", asFUNCTION(ASJSON::load), asCALL_CDECL, "Loads and parses a JSON file from the filesystem." );
                // Load and parse json using string
                REGISTER_GLOBAL_FUNCTION( "JSON@ loads( const string&in serialized, bool ignore_comments = true )", asFUNCTION(ASJSON::loads), asCALL_CDECL, "Parses a serialized JSON string." );

                // Error handlers for dump/s
                REGISTER_ENUM( "error_handler", "Specifies error handling strategy during JSON serialization and deserialization." );
                REGISTER_ENUM_VALUE( "error_handler", "strict", static_cast<int>(ASJSON::error_handler::strict), "Strict, raises exceptions when anything wrong happens." );
                REGISTER_ENUM_VALUE( "error_handler", "coerce", static_cast<int>(ASJSON::error_handler::coerce), "Adaptative, avoids raising exceptions at all costs. returns default values if empty, converts numerical values to the getter, converts utf8 text, and makes various other operations more safer but lot of things could silently fail. consider using Permisive instead." );
                REGISTER_ENUM_VALUE( "error_handler", "permissive", static_cast<int>(ASJSON::error_handler::permissive), "Permisive, avoids raising exceptions but remains strict in general. returning null pointers, removes invalid utf8. writes error messages to the output callback if provided." );

                // Return a string representing the serialized given object
                REGISTER_GLOBAL_FUNCTION( "string dumps( const JSON@ obj, int indents = -1, error_handler errors = error_handler::strict )", asFUNCTION(ASJSON::dumps), asCALL_CDECL, "Serializes a JSON object into a formatted string." );
                // Write a serialized representation of the given object in the given file.
                REGISTER_GLOBAL_FUNCTION( "bool dump( const JSON@ obj, const string&in filePath, int indents = -1, error_handler errors = error_handler::strict )", asFUNCTION(ASJSON::dump), asCALL_CDECL, "Serializes a JSON object and writes it to a file." );
            }
            engine->SetDefaultNamespace( "" );

            // Alias to json::dumps using indents -1 and error_handler_t::ignore. this is exception-safe to print or debug in AS
            REGISTER_OBJECT_METHOD( "JSON", "string to_string() const", asMETHOD(ASJSON, to_string), asCALL_THISCALL, "Returns a string representation of the JSON object." );

            REGISTER_OBJECT_PROPERTY( "JSON", "json::error_handler m_ErrorHandlerMode", asOFFSET(ASJSON, m_ErrorHandlerMode), "Defines how should json handle errors." );

            REGISTER_OBJECT_METHOD( "JSON", "bool is_null() const", asMETHOD(ASJSON, is_null), asCALL_THISCALL, "Returns true if the JSON value is null." );
            REGISTER_OBJECT_METHOD( "JSON", "bool is_boolean() const", asMETHOD(ASJSON, is_boolean), asCALL_THISCALL, "Returns true if the JSON value is a boolean." );
            REGISTER_OBJECT_METHOD( "JSON", "bool is_number() const", asMETHOD(ASJSON, is_number), asCALL_THISCALL, "Returns true if the JSON value is a number." );
            REGISTER_OBJECT_METHOD( "JSON", "bool is_number_integer() const", asMETHOD(ASJSON, is_number_integer), asCALL_THISCALL, "Returns true if the JSON value is an integer number." );
            REGISTER_OBJECT_METHOD( "JSON", "bool is_number_unsigned() const", asMETHOD(ASJSON, is_number_unsigned), asCALL_THISCALL, "Returns true if the JSON value is an unsigned integer number." );
            REGISTER_OBJECT_METHOD( "JSON", "bool is_number_float() const", asMETHOD(ASJSON, is_number_float), asCALL_THISCALL, "Returns true if the JSON value is a floating point number." );
            REGISTER_OBJECT_METHOD( "JSON", "bool is_object() const", asMETHOD(ASJSON, is_object), asCALL_THISCALL, "Returns true if the JSON value is an object." );
            REGISTER_OBJECT_METHOD( "JSON", "bool is_array() const", asMETHOD(ASJSON, is_array), asCALL_THISCALL, "Returns true if the JSON value is an array." );
            REGISTER_OBJECT_METHOD( "JSON", "bool is_string() const", asMETHOD(ASJSON, is_string), asCALL_THISCALL, "Returns true if the JSON value is a string." );
            REGISTER_OBJECT_METHOD( "JSON", "bool is_primitive() const", asMETHOD(ASJSON, is_primitive), asCALL_THISCALL, "Returns true if the JSON value is a primitive type (null, boolean, number, or string)." );
            // Either json object or array
            REGISTER_OBJECT_METHOD( "JSON", "bool is_structured() const", asMETHOD(ASJSON, is_structured), asCALL_THISCALL, "Returns true if the JSON value is structured (object or array)." );
        }
};

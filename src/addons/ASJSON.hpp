#pragma once
#include "addon_registry.h"
// We use header-only https://github.com/nlohmann/json
#include "includes/json.hpp"

#include <iostream>
#include <sstream>

// define to use json::dump/json::load (Requires implementation of Game's FileSystem.)
#define ASJSON_GAME_FILE_SYSTEM 0

class ASJSON
{
    protected:
        int refCount;

    public:
        nlohmann::json m_json;
        bool strict = true;

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

        // Store the default enum value in here in case it changes and avoid constantly casting.
        static const int __error_handler_default__ = static_cast<int>( nlohmann::json::error_handler_t::strict );

        // Return whatever the file was propertly serialized or raise AS exception if needed.
        static bool dumpInternal( const ASJSON* obj, std::string& serialized, int indents, int error_handler )
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( ctx == nullptr )
                return false;

            if( obj == nullptr )
            {
                ctx->SetException( "JSON: Null pointer JSON@ object" );
                return false;
            }

            try
            {
                // I wish nlohmann json had a "separator" char like python.
                serialized = obj->m_json.dump( indents, ' ', false, static_cast<nlohmann::json::error_handler_t>(error_handler) );
                return ( serialized.size() > 0 );
            }
            catch( nlohmann::json::exception& exception )
            {
                ctx->SetException( exception.what(), true );
            }

            return false;
        }

#if ASJSON_GAME_FILE_SYSTEM
        static bool dump( const ASJSON* obj, const CString& filePath, int indents = -1, int error_handler = __error_handler_default__ )
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( ctx == nullptr )
                return false;

            std::string serialized;

            if( ASJSON::dumpInternal( obj, serialized, indents, error_handler ) )
            {
                // -TODO write serialized into the file at filePath
                return true;
            }
            return false;
        }
#endif

        static CString dumps( const ASJSON* obj, int indents = -1, int error_handler = __error_handler_default__ )
        {
            CString str;

            std::string serialized;

            if( ASJSON::dumpInternal( obj, serialized, indents, error_handler ) )
            {
                // -TODO Assign std::string to CString
                str = serialized.c_str();
            }

            return str;
        }

        static ASJSON* loads( const CString& serialized, bool ignore_comments = true )
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( ctx == nullptr )
                return nullptr;

            try
            {
                if( auto js = nlohmann::json::parse( (char*)serialized.c_str(), nullptr, true, ignore_comments ); js.is_structured() )
                {
                    ASJSON* obj = new ASJSON();
                    obj->m_json = std::move( js );
                    return obj;
                }
                else
                {
                    ctx->SetException( "JSON: Only array or objects can be deserialized.", true );
                }
            }
            catch( nlohmann::json::parse_error& exception )
            {
                ctx->SetException( exception.what(), true );
            }

            return nullptr;
        }

#if ASJSON_GAME_FILE_SYSTEM
        static ASJSON* load( const CString& filePath )
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( ctx == nullptr )
                return nullptr;

            // -TODO Need to use the game's FileSystem here reading filePath to respect plugins/ or maps/ paths
            CString serialized; // Set this variable to the file's content and pass on the buffer to the loads method.

            if( false )
            {
                std::stringstream errs;
                errs << "JSON: Can not open file " << serialized.c_str() << "\n";
                std::string err = errs.str();
                ctx->SetException( err.c_str(), true );
            }

            return ASJSON::loads( serialized );
        }
#endif

        // ==================================================================
        // START OF CONVERSIONS
        // ==================================================================

        // Alias to dumps without any Exception being thrown.
        CString to_string()
        {
            return ASJSON::dumps(this, -1, static_cast<int>(nlohmann::json::error_handler_t::replace) );
        }
        // ==================================================================
        // END OF CONVERSIONS
        // ==================================================================

        // ==================================================================
        // START OF BUILDER PATTERNS
        // ==================================================================
        ASJSON* SetStrict( bool is_strict = true ) {
            this->strict = is_strict;
            return this;
        }
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
#if ASJSON_GAME_FILE_SYSTEM
                // Load and parse json using the FileSystem
                REGISTER_GLOBAL_FUNCTION( "JSON@ load( const string&in filePath, bool ignore_comments = true )", asFUNCTION(ASJSON::load), asCALL_CDECL, "Loads and parses a JSON file from the filesystem." );
#endif
                // Load and parse json using string
                REGISTER_GLOBAL_FUNCTION( "JSON@ loads( const string&in serialized, bool ignore_comments = true )", asFUNCTION(ASJSON::loads), asCALL_CDECL, "Parses a serialized JSON string." );

                // Error handlers for dump/s
                REGISTER_ENUM( "error_handler", "Specifies error handling strategy during JSON serialization and deserialization." );
                REGISTER_ENUM_VALUE( "error_handler", "strict", static_cast<int>(nlohmann::json::error_handler_t::strict), "Throw a type_error exception on invalid UTF-8 sequences." );
                REGISTER_ENUM_VALUE( "error_handler", "replace", static_cast<int>(nlohmann::json::error_handler_t::replace), "Replace invalid UTF-8 sequences with U+FFFD." );
                REGISTER_ENUM_VALUE( "error_handler", "ignore", static_cast<int>(nlohmann::json::error_handler_t::ignore), "Ignore invalid UTF-8 sequences." );

                // Return a string representing the serialized given object
                REGISTER_GLOBAL_FUNCTION( "string dumps( const JSON@ obj, int indents = -1, error_handler errors = error_handler::strict )", asFUNCTION(ASJSON::dumps), asCALL_CDECL, "Serializes a JSON object into a formatted string." );
#if ASJSON_GAME_FILE_SYSTEM
                // Write a serialized representation of the given object in the given file.
                REGISTER_GLOBAL_FUNCTION( "bool dump( const JSON@ obj, const string&in filePath, int indents = -1, error_handler errors = error_handler::strict )", asFUNCTION(ASJSON::dump), asCALL_CDECL, "Serializes a JSON object and writes it to a file." );
#endif
            }
            engine->SetDefaultNamespace( "" );

            // Alias to json::dumps using indents -1 and error_handler_t::ignore. this is exception-safe to print or debug in AS
            REGISTER_OBJECT_METHOD( "JSON", "string to_string() const", asMETHOD(ASJSON, to_string), asCALL_THISCALL, "Returns a string representation of the JSON object." );

            // when false; JSON will silent fail. when true; JSON will raise exceptions.
            REGISTER_OBJECT_PROPERTY( "JSON", "bool strict", asOFFSET(ASJSON, strict), "When true, operations raise script exceptions on error; when false, operations fail silently." );
            REGISTER_OBJECT_METHOD( "JSON", "JSON@ SetStrict( bool is_strict = true )", asMETHOD(ASJSON, SetStrict), asCALL_THISCALL, "When true, operations raise script exceptions on error; when false, operations fail silently." );

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

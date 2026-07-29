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

        static ASJSON* JSONFactory()
        {
            return new ASJSON();
        }

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

        CString ToString()
        {
            return ASJSON::dumps(this, -1, static_cast<int>(nlohmann::json::error_handler_t::replace) );
        }

        static inline void Register( asIScriptEngine* engine )
        {
            engine->RegisterObjectType( "JSON", 0, asOBJ_REF );
            engine->RegisterObjectBehaviour( "JSON", asBEHAVE_ADDREF, "void f()", asMETHOD(ASJSON, AddRef), asCALL_THISCALL );
            engine->RegisterObjectBehaviour( "JSON", asBEHAVE_RELEASE, "void f()", asMETHOD(ASJSON, Release), asCALL_THISCALL );

            engine->RegisterObjectBehaviour( "JSON", asBEHAVE_FACTORY, "JSON@ f()", asFUNCTION(ASJSON::JSONFactory), asCALL_CDECL );

            // Methods in json namespace.
            engine->SetDefaultNamespace( "json" );
            {
#if ASJSON_GAME_FILE_SYSTEM
                // Load and parse json using the FileSystem
                engine->RegisterGlobalFunction( "JSON@ load( const string&in filePath, bool ignore_comments = true )", asFUNCTION(ASJSON::load), asCALL_CDECL );
#endif
                // Load and parse json using string
                engine->RegisterGlobalFunction( "JSON@ loads( const string&in serialized, bool ignore_comments = true )", asFUNCTION(ASJSON::loads), asCALL_CDECL );

                // Error handlers for dump/s
                engine->RegisterEnum( "error_handler" );
                // throw a type_error exception in case of invalid UTF-8
                engine->RegisterEnumValue( "error_handler", "strict", static_cast<int>(nlohmann::json::error_handler_t::strict) );
                // replace invalid UTF-8 sequences with U+FFFD
                engine->RegisterEnumValue( "error_handler", "replace", static_cast<int>(nlohmann::json::error_handler_t::replace) );
                // ignore invalid UTF-8 sequences
                engine->RegisterEnumValue( "error_handler", "ignore", static_cast<int>(nlohmann::json::error_handler_t::ignore) );

                // Return a string representing the serialized given object
                engine->RegisterGlobalFunction( "string dumps( const JSON@ obj, int indents = -1, error_handler errors = error_handler::strict )", asFUNCTION(ASJSON::dumps), asCALL_CDECL );
#if ASJSON_GAME_FILE_SYSTEM
                // Write a serialized representation of the given object in the given file.
                engine->RegisterGlobalFunction( "bool dump( const JSON@ obj, const string&in filePath, int indents = -1, error_handler errors = error_handler::strict )", asFUNCTION(ASJSON::dump), asCALL_CDECL );
#endif

                // Alias to json::dumps using indents -1 and error_handler_t::ignore. this is exception-safe to print or debug in AS
                engine->RegisterObjectMethod( "JSON", "string ToString() const", asMETHOD(ASJSON, ToString), asCALL_THISCALL );

                // when false; JSON will silent fail. when true; JSON will raise exceptions.
                engine->RegisterObjectProperty( "JSON", "bool strict", asOFFSET(ASJSON, strict) );
            }
            engine->SetDefaultNamespace( "" );
        }
};

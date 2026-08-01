#include "addon_registry.h"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <optional>

namespace ASException
{
    // Struct container per module
    struct ExceptionModuleData
    {
        std::filesystem::path ScriptSection;
        std::string Message;
        int Id = 0;
        CScriptDictionary* dictionaryData = nullptr;
    };

    static inline void Clear( ExceptionModuleData* context )
    {
        if( context != nullptr )
        {
            context->ScriptSection.clear();
            context->Message.clear();

            // Clean up dictionary object
            if( CScriptDictionary* dict = context->dictionaryData; dict != nullptr )
            {
                context->dictionaryData = nullptr;
                dict->Release();
            }
        }
    }

    void CleanUpModuleData( asIScriptModule* ctx )
    {
        if( ExceptionModuleData* data = static_cast<ExceptionModuleData*>( ctx->GetUserData(1) ); data != nullptr )
        {
            Clear( data );
            delete data;
        }
    }

    // Initialize one ExceptionModuleData per script and return a pointer to it.
    std::optional<std::pair<ExceptionModuleData*, asIScriptContext*>> GetModuleData()
    {
        if( asIScriptContext* ctx = asGetActiveContext(); ctx != nullptr )
        {
            if( asIScriptFunction* func = ctx->GetFunction(); func != nullptr )
            {
                if( asIScriptModule* module = func->GetModule(); module != nullptr )
                {
                    ExceptionModuleData* data = static_cast<ExceptionModuleData*>( module->GetUserData(1) );

                    if( data == nullptr )
                    {
                        if( auto engine = ctx->GetEngine(); engine != nullptr )
                        {
                            data = new ExceptionModuleData();
                            module->SetUserData( data, 1 ); 
                            engine->SetModuleUserDataCleanupCallback( CleanUpModuleData, 1 );
                        }
                    }
                    return { { data, ctx } };
                }
            }
        }
        return std::nullopt;
    }

    static void ClearScripted()
    {
        if( auto moduleDataOpt = GetModuleData(); moduleDataOpt.has_value() )
        {
            Clear( moduleDataOpt.value().first );
        }
    }

    static int Id()
    {
        if( auto moduleDataOpt = GetModuleData(); moduleDataOpt.has_value() )
            return moduleDataOpt.value().first->Id;
        return -1;
    }

    static CString Message()
    {
        CString str;

        if( auto moduleDataOpt = GetModuleData(); moduleDataOpt.has_value() )
        {
            auto moduleData = moduleDataOpt.value();

            std::string& msg = moduleData.first->Message;

            if( msg.size() == 0 )
            {
                moduleData.first->Message = moduleData.second->GetExceptionString();
            }

            str = msg;
        }
        return str;
    }

    static void ScriptSection(
        CString* absolute,
        CString* relative,
        CString* fileName,
        CString* methodName,
        CString* nameSpace,
        CString* objectName
    )
    {
        CString str;

        if( auto moduleDataOpt = GetModuleData(); moduleDataOpt.has_value() )
        {
            auto moduleData = moduleDataOpt.value();

            // Build paths
            if( absolute != nullptr || relative != nullptr || fileName != nullptr )
            {
                std::filesystem::path& section = moduleData.first->ScriptSection;

                if( section.empty() )
                {
                    const char* scriptSection = nullptr;
                    moduleData.second->GetExceptionLineNumber( 0, &scriptSection );

                    if( scriptSection != nullptr )
                    {
                        // Format path
                        moduleData.first->ScriptSection = std::filesystem::path( scriptSection );
                    }
                }

                if( !section.empty() )
                {
                    if( absolute != nullptr )
                    {
                        *absolute = section.string();
                    }

                    if( relative != nullptr )
                    {
                        std::filesystem::path basePath = std::filesystem::current_path();
                        std::filesystem::path rel_path = std::filesystem::relative( section, basePath );
                        *relative = rel_path.string();
                    }

                    if( fileName != nullptr )
                    {
                        *fileName = section.filename().replace_extension("").string();
                    }
                }
            }

            // Build declarations
            if( methodName != nullptr || nameSpace != nullptr || objectName != nullptr )
            {
                const asIScriptFunction* function = moduleData.second->GetExceptionFunction();

                if( function != nullptr )
                {
                    if( methodName != nullptr )
                    {
                        if( const char* cstr = function->GetDeclaration(); cstr != nullptr && cstr[0] != '\0' )
                        {
                            *methodName = cstr;
                        }
                    }
                    if( nameSpace != nullptr )
                    {
                        if( const char* cstr = function->GetNamespace(); cstr != nullptr && cstr[0] != '\0' )
                        {
                            *nameSpace = cstr;
                        }
                    }
                    if( objectName != nullptr )
                    {
                        if( const char* cstr = function->GetObjectName(); cstr != nullptr && cstr[0] != '\0' )
                        {
                            *objectName = cstr;
                        }
                    }
                }
            }
        }
    }

    inline void ThrowScriptException( std::pair<ExceptionModuleData*, asIScriptContext*> context, const CString& message, bool canCatch )
    {
        // Clear previous exception data
        Clear( context.first );

        context.first->Id++;
        context.second->SetException( message.c_str(), canCatch );
    }

    static void Throw( const CString& message, bool canCatch = true )
    {
        if( auto moduleDataOpt = GetModuleData(); moduleDataOpt.has_value() )
        {
            ThrowScriptException( moduleDataOpt.value(), message, canCatch );
        }
    }

    static void ThrowDictionary( const CString& message, CScriptDictionary* additionalData, bool canCatch = true )
    {
        if( auto moduleDataOpt = GetModuleData(); moduleDataOpt.has_value() )
        {
            auto moduleData = moduleDataOpt.value();

            ThrowScriptException( moduleData, message, canCatch );

            if( additionalData != nullptr )
            {
                if( moduleData.second->WillExceptionBeCaught() )
                {
                    moduleData.first->dictionaryData = additionalData;
                }
                else
                {
                    // Don't add reference if the script won't catch exception.
                    additionalData->Release();
                }
            }
        }
    }

    static inline void Register( asIScriptEngine* engine )
    {
        engine->SetDefaultNamespace( "Exception" );
        REGISTER_GLOBAL_FUNCTION( "void Throw( const string&in exception, bool canCatch = true )", asFUNCTION(ASException::Throw), asCALL_CDECL, "Raises a script exception. if canCatch is false the script's catch block won't be called." );
        REGISTER_GLOBAL_FUNCTION( "void Throw( const string&in exception, dictionary@ additionalData, bool canCatch = true )", asFUNCTION(&::ASException::ThrowDictionary), asCALL_CDECL, "Raises a script exception with aditional metadata dictionary. if canCatch is false the script's catch block won't be called." );
        REGISTER_GLOBAL_FUNCTION( "void Clear()", asFUNCTION(ASException::ClearScripted), asCALL_CDECL, "Releases reference to the last exception. by default exceptions are cleared when new ones are created. Call this method after a catch block to clear all members." );
        REGISTER_GLOBAL_FUNCTION( "const int Id()", asFUNCTION(ASException::Id), asCALL_CDECL, "Get the current exception count. this value only increases for explicit script-throw exceptions." );
        REGISTER_GLOBAL_FUNCTION( "string Message()", asFUNCTION(ASException::Message), asCALL_CDECL, "Get the current exception message" );
        REGISTER_GLOBAL_FUNCTION( "void ScriptSection( string&out absolute = void, string&out relative = void, string&out fileName = void, string&out methodName = void, string&out nameSpace = void, string&out objectName = void )", asFUNCTION(ASException::ScriptSection), asCALL_CDECL, "Get the path to the script that raised the last exception." );
        engine->SetDefaultNamespace( "" );
    }
}

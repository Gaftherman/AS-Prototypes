#include "addon_registry.h"
#include <iostream>
#include <sstream>

class ASException
{
    protected:
        CString message;
        CString func;
        CString sect;
        CString stack;
        int line;
        CScriptDictionary* data;
        int refCount;

    public:

        ASException() : refCount(1) { }

        ~ASException() { }

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

        static void RaiseException( const CString& message, CScriptDictionary* data = nullptr )
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( ctx != nullptr )
            {
                ctx->SetException( message.c_str(), true );

                if( data != nullptr )
                {
                    if( ctx->WillExceptionBeCaught() )
                    {
                        // -TODO Como mantener data valido hasta GetException
                    }
                }

            }
        }

        static ASException* GetException()
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( ctx == nullptr )
                return nullptr;

            if( ctx->GetState() != asEXECUTION_EXCEPTION )
            {
                ctx->SetException( "null pointer error: Can not call GetException outside of a 'catch' block!", false );
                return nullptr;
            }

            const char* scriptSection = nullptr;
            const asIScriptFunction* function = ctx->GetExceptionFunction();

            ASException* exception = new ASException();

            exception->message = ctx->GetExceptionString();
            exception->line = ctx->GetExceptionLineNumber(0, &scriptSection);
            exception->sect = scriptSection ? scriptSection : "";
            exception->func = function ? function->GetDeclaration() : "";

            std::stringstream stack;

            for( asUINT n = 1; n < ctx->GetCallstackSize(); n++ )
            {
                function = ctx->GetFunction(n);

                if( function )
                {
                    if( function->GetFuncType() == asFUNC_SCRIPT )
                    {
                        int line = ctx->GetLineNumber(n, 0, &scriptSection);
                        stack << (scriptSection ? scriptSection : "") << " (" << line << "): " << function->GetDeclaration() << "\n";
                    }
                    else
                    {
                        // The context is being reused by the application for a nested call
                        stack << "{...application...}: " << function->GetDeclaration() << "\n";
                    }
                }
                else
                {
                    // The context is being reused by the script engine for a nested call
                    stack << "{...script engine...}\n";
                }
            }

            exception->stack = stack.str();

            return exception;
        }

        static inline void Register( asIScriptEngine* engine )
        {
            engine->RegisterObjectType( "Exception", 0, asOBJ_REF );
            engine->RegisterObjectBehaviour( "Exception", asBEHAVE_ADDREF, "void f()", asMETHOD(ASException, AddRef), asCALL_THISCALL );
            engine->RegisterObjectBehaviour( "Exception", asBEHAVE_RELEASE, "void f()", asMETHOD(ASException, Release), asCALL_THISCALL );

            engine->RegisterObjectProperty("Exception", "string message", asOFFSET(ASException, message));
            engine->RegisterObjectProperty("Exception", "string func", asOFFSET(ASException, func) );
            engine->RegisterObjectProperty("Exception", "string sect", asOFFSET(ASException, sect) );
            engine->RegisterObjectProperty("Exception", "string stack", asOFFSET(ASException, stack) );
            engine->RegisterObjectProperty("Exception", "int line", asOFFSET(ASException, line) );
            engine->RegisterObjectProperty("Exception", "dictionary data", asOFFSET(ASException, data) );

            // Raise a exception with additional information stored in data if needed.
            engine->RegisterGlobalFunction( "void SetException( const string&in exception, dictionary@ data = null )", asFUNCTION(ASException::RaiseException), asCALL_CDECL );

            // Get exception
            engine->RegisterGlobalFunction( "Exception@ GetException()", asFUNCTION(ASException::GetException), asCALL_CDECL );
        }
};

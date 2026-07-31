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

#if 0
            if( ctx->GetState() != asEXECUTION_EXCEPTION )
            {
                ctx->SetException( "null pointer error: Can not call GetException outside of a 'catch' block!", false );
                return nullptr;
            }
#endif

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
            REGISTER_OBJECT_TYPE( "Exception", 0, asOBJ_REF, "Exception object storing callstack, line number, message, and metadata." );
            REGISTER_OBJECT_BEHAVIOUR( "Exception", asBEHAVE_ADDREF, "void f()", asMETHOD(ASException, AddRef), asCALL_THISCALL, "Increments reference count of Exception." );
            REGISTER_OBJECT_BEHAVIOUR( "Exception", asBEHAVE_RELEASE, "void f()", asMETHOD(ASException, Release), asCALL_THISCALL, "Decrements reference count of Exception." );

            REGISTER_OBJECT_PROPERTY( "Exception", "string message", asOFFSET(ASException, message), "Exception error message string." );
            REGISTER_OBJECT_PROPERTY( "Exception", "string func", asOFFSET(ASException, func), "Function declaration where exception occurred." );
            REGISTER_OBJECT_PROPERTY( "Exception", "string sect", asOFFSET(ASException, sect), "Script section or file name where exception occurred." );
            REGISTER_OBJECT_PROPERTY( "Exception", "string stack", asOFFSET(ASException, stack), "Call stack trace at the moment exception occurred." );
            REGISTER_OBJECT_PROPERTY( "Exception", "int line", asOFFSET(ASException, line), "Script line number where exception occurred." );
            REGISTER_OBJECT_PROPERTY( "Exception", "dictionary data", asOFFSET(ASException, data), "Additional contextual data dictionary attached to exception." );

            // Raise an exception with additional information stored in data if needed.
            REGISTER_GLOBAL_FUNCTION( "void SetException( const string&in exception, dictionary@ data = null )", asFUNCTION(ASException::RaiseException), asCALL_CDECL, "Raises a script exception with an optional metadata dictionary." );

            // Get exception
            REGISTER_GLOBAL_FUNCTION( "Exception@ GetException()", asFUNCTION(ASException::GetException), asCALL_CDECL, "Returns the current active exception object." );
        }
};

#include "addons/ASException.hpp"
#include "CASDocRegistry.hpp"

class CASDocException : public CASDocRegistry
{
    bool Register() override
    {
        return SetDefaultNamespace( "Exception" ) &&
        RegisterGlobalFunction(
            "Raises a script exception. if canCatch is false the script's catch block won't be called."sv,
            "void Throw( const string&in exception, bool canCatch = true )",
            asFUNCTION( ASException::Throw ),
            asCALL_CDECL
        ) &&
        RegisterGlobalFunction(
            "Raises a script exception with aditional metadata dictionary. if canCatch is false the script's catch block won't be called."sv,
            "void Throw( const string&in exception, dictionary@ additionalData, bool canCatch = true )",
            asFUNCTION( ASException::ThrowDictionary ),
            asCALL_CDECL
        ) &&
        RegisterGlobalFunction(
            "Releases reference to the last exception. by default exceptions are cleared when new ones are created. Call this method after a catch block to clear all members."sv,
            "void Clear()",
            asFUNCTION( ASException::ClearScripted ),
            asCALL_CDECL
        ) &&
        RegisterGlobalFunction(
            "Get the current exception count. this value only increases for explicit script-throw exceptions."sv,
            "int Id()",
            asFUNCTION( ASException::Id ),
            asCALL_CDECL
        ) &&
        RegisterGlobalFunction(
            "Line of where the exception was raised."sv,
            "int Line()",
            asFUNCTION( ASException::Line ),
            asCALL_CDECL
        ) &&
        RegisterGlobalFunction(
            "Get the current exception message."sv,
            "string Message()",
            asFUNCTION( ASException::Message ),
            asCALL_CDECL
        ) &&
        RegisterGlobalFunction(
            "Get the call stack in string form."sv,
            "string CallStack()",
            asFUNCTION( ASException::CallStack ),
            asCALL_CDECL
        ) &&
        RegisterGlobalFunction(
            "Get a handle to the dictionary data if the exception provided one when raised."sv,
            "dictionary@ Dictionary()",
            asFUNCTION( ASException::Dictionary ),
            asCALL_CDECL
        ) &&
        RegisterGlobalFunction(
            "Get the path to the script that raised the last exception."sv,
            "void ScriptSection( string&out absolute = void, string&out relative = void, string&out fileName = void, string&out methodName = void, string&out nameSpace = void, string&out objectName = void )",
            asFUNCTION( ASException::ScriptSection ),
            asCALL_CDECL
        ) &&
        SetDefaultNamespace( "" );
    }
};

CASDocRegisterInterface(Exception);

#pragma once
#include "addon_registry.h"

namespace ASDispose
{
    static bool Dispose( void* mem, int typeId )
    {
        if( mem == nullptr )
            return false;

        asIScriptContext* ctx = asGetActiveContext();

        if( !ctx )
            return false;

        asIScriptEngine* engine = ctx->GetEngine();

        if( !engine )
            return false;

        // No primitive deletions!
        if( !( typeId & asTYPEID_MASK_OBJECT ) )
        {
            ctx->SetException( "Cannot dispose of a non-object type!", true );
            return false;
        }

        // -TODO Exception if the object provided is been marked as "const"
        // ctx->SetException( "Cannot dispose of a const object!", true );

        asIScriptObject** objPtr = reinterpret_cast<asIScriptObject**>(mem);

        if( !objPtr )
            return false;

        if( asIScriptObject* obj = *objPtr; obj != nullptr )
        {
            // -TODO If the input object is a handle remove all references to it
            // -TODO If the input is a static class delete it and construct a new fresh class with all properties to default
//            return true;
        }

        return false;
    }

    static inline void Register( asIScriptEngine* engine )
    {
        engine->RegisterGlobalFunction( "bool Dispose( ?&in )", asFUNCTION(ASDispose::Dispose), asCALL_CDECL );
    }
}

#pragma once
#include "addon_registry.h"

namespace ASDispose
{
    static bool Dispose( asIScriptObject* obj, int typeId )
    {
        if( obj != nullptr )
        {
            if( asIScriptContext* ctx = asGetActiveContext(); ctx != nullptr )
            {
                if( asIScriptEngine* engine = ctx->GetEngine(); engine != nullptr )
                {
                    if( typeId & asTYPEID_OBJHANDLE )
                    {
                        // -TODO Como eliminar la wea totalmente de toda referencia
                        return true;
                    }
                }

                ctx->SetException( "Can not Dispose of a non-handle object!", true );
            }
        }
        return false;
    }

    static inline void Register( asIScriptEngine* engine )
    {
        engine->RegisterGlobalFunction( "bool Dispose( ?&out )", asFUNCTION(&ASDispose::Dispose), asCALL_CDECL );
    }
}

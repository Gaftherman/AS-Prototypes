#pragma once
#include "addon_registry.h"

namespace ASColors
{
    class ASHEX
    {
        private:
            int refCount;

        public:

            ASHEX() : refCount(1) {}
            ~ASHEX() {}

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

            static inline void Register( asIScriptEngine* engine )
            {
                REGISTER_OBJECT_TYPE( "HEX", 0, asOBJ_REF, "Represents a color class container of HEX value." );
                REGISTER_OBJECT_BEHAVIOUR( "HEX", asBEHAVE_ADDREF, "void f()", asMETHOD(ASHEX, AddRef), asCALL_THISCALL, "Represents a color class container of HEX value." );
                REGISTER_OBJECT_BEHAVIOUR( "HEX", asBEHAVE_RELEASE, "void f()", asMETHOD(ASHEX, Release), asCALL_THISCALL, "Represents a color class container of HEX value." );
            }
    };
}

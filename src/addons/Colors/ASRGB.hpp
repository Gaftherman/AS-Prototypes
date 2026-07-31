#pragma once
#include "addon_registry.h"

#include <optional>
#include <algorithm>

namespace ASColors
{
    class ASRGB
    {
        private:
            int refCount;

        public:

            ASRGB() : refCount(1) {}
            ~ASRGB() {}

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

#define DefineColorAS(colorName) \
    REGISTER_OBJECT_METHOD( "RGB", "int "#colorName"( optional<int> = nullopt )", asMETHOD(ASRGB, colorName), asCALL_THISCALL, "Set the "#colorName" color." ); \
    REGISTER_OBJECT_METHOD( "RGB", "int "#colorName"() const", asMETHOD(ASRGB, colorName), asCALL_THISCALL, "Set the "#colorName" color." )
#define DefineColor(colorName) \
        int m_##colorName = 0; \
        int colorName( std::optional<int> color = std::nullopt ) \
        { \
            if( color.has_value() ) \
            { \
                this->m_##colorName = std::clamp( *color, 0, 255 ); \
            } \
            return this->m_##colorName; \
        }

        DefineColor(Red)
        DefineColor(Green)
        DefineColor(Blue)

        void Clear()
        {
            this->m_Red = this->m_Green = this->m_Blue = 0;
        }

        static ASRGB* Factory()
        {
            ASRGB* rgb = new ASRGB();
            return rgb;
        }

        static ASRGB* FactoryInteger( int r, int g, int b )
        {
            ASRGB* rgb = new ASRGB();
            rgb->Red(r);
            rgb->Green(g);
            rgb->Blue(b);
            return rgb;
        }

// Factory desde HEX, metodo ToHEX()
#if ASCOLOR_HEX
#endif

        static inline void Register( asIScriptEngine* engine )
        {
            REGISTER_OBJECT_TYPE( "RGB", 0, asOBJ_REF, "Represents a color class container of Red, Green and Blue." );
            REGISTER_OBJECT_BEHAVIOUR( "RGB", asBEHAVE_ADDREF, "void f()", asMETHOD(ASRGB, AddRef), asCALL_THISCALL, "Represents a color class container of Red, Green and Blue." );
            REGISTER_OBJECT_BEHAVIOUR( "RGB", asBEHAVE_RELEASE, "void f()", asMETHOD(ASRGB, Release), asCALL_THISCALL, "Represents a color class container of Red, Green and Blue." );

            // constructors
            REGISTER_OBJECT_BEHAVIOUR( "RGB", asBEHAVE_FACTORY, "RGB@ f()",
                asFUNCTION(ASRGB::Factory), asCALL_CDECL, "Constructs a RGB class" );

            REGISTER_OBJECT_BEHAVIOUR( "RGB", asBEHAVE_FACTORY, "RGB@ f(int&in r, int&in g, int&in b)",
                asFUNCTION(ASRGB::FactoryFactoryInteger), asCALL_CDECL, "Constructs a RGB class" );

            REGISTER_OBJECT_BEHAVIOUR( "RGB", asBEHAVE_FACTORY, "void Clear()",
                asFUNCTION(ASRGB::Clear), asCALL_CDECL, "Clear all colors to zero." );

            DefineColorAS(Red);
            DefineColorAS(Green);
            DefineColorAS(Blue);
        }
    };
}

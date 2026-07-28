#pragma once
#include "addon_registry.h"

class ASOptional
{
    private:
        asITypeInfo* typeInfo;
        int subTypeId;
        int subTypeSize;
        bool hasValue = false;
        void* valueBuffer;
        int refCount;

    public:
        ASOptional( asITypeInfo* tinfo ) : typeInfo(tinfo), valueBuffer(nullptr), refCount(1)
        {
            typeInfo->AddRef();
            subTypeId = typeInfo->GetSubTypeId();
            subTypeSize = typeInfo->GetEngine()->GetSizeOfPrimitiveType(subTypeId);
            
            if( subTypeSize == 0 )
            {
                asITypeInfo* subType = typeInfo->GetSubType();
                if( subType) subTypeSize = subType->GetSize();
            }
        }

        ~ASOptional()
        {
            Clear();
            typeInfo->Release();
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

        bool HasValue() const
        {
            return hasValue;
        }

        void Clear()
        {
            if( !hasValue )
                return;

            if( valueBuffer && ( subTypeId & asTYPEID_MASK_OBJECT ) )
            {
                asIScriptObject* obj = *(asIScriptObject**)valueBuffer;

                if( obj )
                {
                    obj->Release();
                }
            }

            operator delete(valueBuffer);
            valueBuffer = nullptr;
            hasValue = false;
        }

        void Set( void* ref )
        {
            Clear();

            if( !ref )
                return;

            valueBuffer = operator new(subTypeSize);
            hasValue = true;

            if( subTypeId & asTYPEID_MASK_OBJECT )
            {
                asIScriptObject* obj = *(asIScriptObject**)ref;

                *(asIScriptObject**)valueBuffer = obj;

                if( obj != nullptr )
                {
                    obj->AddRef();
                }
            }
            else
            {
                std::memcpy( valueBuffer, ref, subTypeSize );
            }
        }

        void Get( void* outRef )
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( !hasValue )
            {
                if( ctx != nullptr)
                {
                    ctx->SetException( "null pointer value in optional!", true );
                }
                return;
            }

            if( subTypeId & asTYPEID_MASK_OBJECT )
            {
                asIScriptObject* obj = *(asIScriptObject**)valueBuffer;

                *(asIScriptObject**)outRef = obj;

                if( obj != nullptr )
                {
                    obj->AddRef();
                }
            }
            else
            {
                std::memcpy( outRef, valueBuffer, subTypeSize );
            }
        }

        static ASOptional* Factory(asITypeInfo* typeInfo )
        {
            return new ASOptional( typeInfo );
        }

        static ASOptional* FactoryWithValue( asITypeInfo* typeInfo, void* valueRef )
        {
            ASOptional* opt = new ASOptional( typeInfo );

            if( opt != nullptr && valueRef != nullptr )
            {
                opt->Set( valueRef );
            }

            return opt;
        }


        static void AssignValueWrapper(ASOptional* opt, void* valueRef )
        {
            if( opt != nullptr )
            {
                opt->Set( valueRef );
            }
        }

        static void SetValueWrapper(ASOptional* opt, void* valueRef )
        {
            if( opt != nullptr )
            {
                opt->Set( valueRef );
            }
        }

        void* GetValuePointer()
        {
            asIScriptContext* ctx = asGetActiveContext();

            if( !hasValue )
            {
                if( ctx != nullptr)
                {
                    ctx->SetException( "null pointer value in optional!", true );
                }
                return nullptr;
            }

            // If it's a handle (T@), AngelScript expects a pointer to the handle
            if( subTypeId & asTYPEID_MASK_OBJECT )
            {
                return valueBuffer; // Returns void** (which matches T@&)
            }

            // Primitives (int, float, etc.) return the direct data address
            return valueBuffer; // Returns void* (which matches T&)
        }

        static void* GetValueWrapper( ASOptional* opt )
        {
            return opt != nullptr ? opt->GetValuePointer() : nullptr;
        }

        static inline void Register( asIScriptEngine* engine )
        {
            engine->RegisterObjectType( "optional<class T>", 0, asOBJ_REF | asOBJ_TEMPLATE);
            engine->RegisterObjectBehaviour( "optional<T>", asBEHAVE_ADDREF, "void f()", asMETHOD(ASOptional, AddRef), asCALL_THISCALL );
            engine->RegisterObjectBehaviour( "optional<T>", asBEHAVE_RELEASE, "void f()", asMETHOD(ASOptional, Release), asCALL_THISCALL );

            // Default constructor
            engine->RegisterObjectBehaviour( "optional<T>", asBEHAVE_FACTORY, "optional<T>@ f(int &in)", 
                asFUNCTION((ASOptional*(*)(asITypeInfo*))ASOptional::Factory), asCALL_CDECL );

            // -TODO This crash. not sure how to allow constructors like optional<string> optStr( "something" )
            // engine->RegisterObjectBehaviour( "optional<T>", asBEHAVE_FACTORY, "optional<T>@ f(int &in, const T &in value)", 
            //    asFUNCTION((ASOptional*(*)(asITypeInfo*, void*))ASOptional::FactoryWithValue), asCALL_CDECL );

            engine->RegisterObjectMethod( "optional<T>", "bool has_value() const", asMETHOD(ASOptional, HasValue), asCALL_THISCALL );
            engine->RegisterObjectMethod( "optional<T>", "void clear()", asMETHOD(ASOptional, Clear), asCALL_THISCALL );
            engine->RegisterObjectMethod( "optional<T>", "const T& value() const", asFUNCTION(ASOptional::GetValueWrapper), asCALL_CDECL_OBJFIRST );
            engine->RegisterObjectMethod( "optional<T>", "void set(const T &in value)", asFUNCTION(ASOptional::SetValueWrapper), asCALL_CDECL_OBJFIRST );

            engine->RegisterObjectMethod( "optional<T>", "optional<T>& opAssign(const T &in value)", 
                asFUNCTION(ASOptional::AssignValueWrapper), asCALL_CDECL_OBJFIRST);
        }

};

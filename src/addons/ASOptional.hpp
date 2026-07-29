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

            if( valueBuffer )
            {
                if( subTypeId & asTYPEID_OBJHANDLE )
                {
                    void* obj = *(void**)valueBuffer;
                    if( obj )
                    {
                        typeInfo->GetEngine()->ReleaseScriptObject(obj, typeInfo->GetSubType());
                    }
                    operator delete(valueBuffer);
                }
                else if( subTypeId & asTYPEID_MASK_OBJECT )
                {
                    typeInfo->GetEngine()->ReleaseScriptObject(valueBuffer, typeInfo->GetSubType());
                }
                else
                {
                    operator delete(valueBuffer);
                }
            }

            valueBuffer = nullptr;
            hasValue = false;
        }

        void Set( void* ref )
        {
            Clear();

            if( !ref )
                return;

            hasValue = true;

            if( subTypeId & asTYPEID_OBJHANDLE )
            {
                valueBuffer = operator new(sizeof(void*));
                void* obj = *(void**)ref;
                *(void**)valueBuffer = obj;
                if( obj != nullptr )
                {
                    typeInfo->GetEngine()->AddRefScriptObject(obj, typeInfo->GetSubType());
                }
            }
            else if( subTypeId & asTYPEID_MASK_OBJECT )
            {
                valueBuffer = typeInfo->GetEngine()->CreateScriptObjectCopy(ref, typeInfo->GetSubType());
            }
            else
            {
                valueBuffer = operator new(subTypeSize);
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

            if( subTypeId & asTYPEID_OBJHANDLE )
            {
                void* obj = *(void**)valueBuffer;
                *(void**)outRef = obj;
                if( obj != nullptr )
                {
                    typeInfo->GetEngine()->AddRefScriptObject(obj, typeInfo->GetSubType());
                }
            }
            else if( subTypeId & asTYPEID_MASK_OBJECT )
            {
                typeInfo->GetEngine()->AssignScriptObject(outRef, valueBuffer, typeInfo->GetSubType());
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

            if( subTypeId & asTYPEID_OBJHANDLE )
            {
                return valueBuffer;
            }

            return valueBuffer;
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

            engine->RegisterObjectBehaviour( "optional<T>", asBEHAVE_FACTORY, "optional<T>@ f(int &in, const T &in value)", 
                asFUNCTION((ASOptional*(*)(asITypeInfo*, void*))ASOptional::FactoryWithValue), asCALL_CDECL );

            engine->RegisterObjectMethod( "optional<T>", "bool has_value() const", asMETHOD(ASOptional, HasValue), asCALL_THISCALL );
            engine->RegisterObjectMethod( "optional<T>", "void clear()", asMETHOD(ASOptional, Clear), asCALL_THISCALL );
            engine->RegisterObjectMethod( "optional<T>", "const T& value() const", asFUNCTION(ASOptional::GetValueWrapper), asCALL_CDECL_OBJFIRST );
            engine->RegisterObjectMethod( "optional<T>", "void set(const T &in value)", asFUNCTION(ASOptional::SetValueWrapper), asCALL_CDECL_OBJFIRST );

            engine->RegisterObjectMethod( "optional<T>", "optional<T>& opAssign(const T &in value)", 
                asFUNCTION(ASOptional::AssignValueWrapper), asCALL_CDECL_OBJFIRST);
        }

};

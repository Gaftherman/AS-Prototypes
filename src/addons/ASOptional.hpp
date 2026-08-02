#pragma once

#include <angelscript.h>

#include <memory>

// Whatever to register a std::nullopt-like object.
#define ASOPTIONAL_ADD_NULLOPT 0

// See ASOptional.cpp for registration
class ASOptional
{
private:
    asITypeInfo* typeInfo;
    int subTypeId;
    int subTypeSize;
    bool hasValue = false;
    void* valueBuffer;
    int refCount;

    asITypeInfo* GetSubTypeInfo() const
    {
        asITypeInfo* subType = typeInfo->GetSubType();
        if( !subType )
        {
            subType = typeInfo->GetEngine()->GetTypeInfoById(subTypeId);
        }
        return subType;
    }

#if ASOPTIONAL_ADD_NULLOPT
    class ASNullOptional
    {
    public:
        int refCount = 1;

        void AddRef() { refCount++; }

        void Release()
        {
            if (--refCount == 0)
                delete this;
        }
    };
#endif

public:
    ASOptional( asITypeInfo* tinfo ) : typeInfo(tinfo), valueBuffer(nullptr), refCount(1)
    {
        typeInfo->AddRef();
        subTypeId = typeInfo->GetSubTypeId();
        subTypeSize = typeInfo->GetEngine()->GetSizeOfPrimitiveType(subTypeId);
        
        if( subTypeSize == 0 )
        {
            asITypeInfo* subType = GetSubTypeInfo();
            if( subType ) subTypeSize = subType->GetSize();
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

        asITypeInfo* subType = GetSubTypeInfo();

        if( valueBuffer )
        {
            if( subTypeId & asTYPEID_OBJHANDLE )
            {
                void* obj = *(void**)valueBuffer;
                if( obj && subType )
                    typeInfo->GetEngine()->ReleaseScriptObject(obj, subType);
                operator delete(valueBuffer);
            }
            else if( subType && (subType->GetFlags() & asOBJ_REF) )
            {
                typeInfo->GetEngine()->ReleaseScriptObject(valueBuffer, subType);
            }
            else if( subTypeId & asTYPEID_MASK_OBJECT )
            {
                if( subType )
                    typeInfo->GetEngine()->ReleaseScriptObject(valueBuffer, subType);
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
        asITypeInfo* subType = GetSubTypeInfo();

        if( subTypeId & asTYPEID_OBJHANDLE )
        {
            valueBuffer = operator new(sizeof(void*));
            void* obj = *(void**)ref;
            *(void**)valueBuffer = obj;
            if( obj && subType )
                typeInfo->GetEngine()->AddRefScriptObject(obj, subType);
        }
        else if( subType && (subType->GetFlags() & asOBJ_REF) )
        {
            valueBuffer = ref;
            typeInfo->GetEngine()->AddRefScriptObject(valueBuffer, subType);
        }
        else if( subTypeId & asTYPEID_MASK_OBJECT )
        {
            if( subType )
                valueBuffer = typeInfo->GetEngine()->CreateScriptObjectCopy(ref, subType);
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
            if( ctx != nullptr )
                ctx->SetException( "null pointer value in optional!", true );
            return;
        }

        asITypeInfo* subType = GetSubTypeInfo();

        if( subTypeId & asTYPEID_OBJHANDLE )
        {
            void* obj = *(void**)valueBuffer;
            *(void**)outRef = obj;
            if( obj && subType )
                typeInfo->GetEngine()->AddRefScriptObject(obj, subType);
        }
        else if( subType && (subType->GetFlags() & asOBJ_REF) )
        {
            *(void**)outRef = valueBuffer;
            typeInfo->GetEngine()->AddRefScriptObject(valueBuffer, subType);
        }
        else if( subTypeId & asTYPEID_MASK_OBJECT )
        {
            if( subType )
                typeInfo->GetEngine()->AssignScriptObject(outRef, valueBuffer, subType);
        }
        else
        {
            std::memcpy( outRef, valueBuffer, subTypeSize );
        }
    }

#if ASOPTIONAL_ADD_NULLOPT
    static ASOptional* FactoryNullOptional( asITypeInfo* typeInfo, ASNullOptional* )
    {
        return new ASOptional(typeInfo);
    }
#endif

    static ASOptional* Factory( asITypeInfo* typeInfo )
    {
        return new ASOptional( typeInfo );
    }

    static ASOptional* FactoryWithValue( asITypeInfo* typeInfo, void* valueRef )
    {
        ASOptional* opt = new ASOptional( typeInfo );

        if( opt && valueRef )
            opt->Set( valueRef );

        return opt;
    }

    static ASOptional* FactoryGeneric( asITypeInfo* typeInfo, void* listBuffer )
    {
        asIScriptContext* ctx = asGetActiveContext();

        asUINT* buffer = reinterpret_cast<asUINT*>(listBuffer);
        asUINT elementCount = *buffer++; 

        if( elementCount > 1 )
        {
            if( ctx != nullptr ) {
                ctx->SetException( "Too many arguments! Expected one.", true );
            }

            return nullptr;
        }

        if( elementCount == 0 )
        {
            return new ASOptional( typeInfo );
        }

        return ASOptional::FactoryWithValue( typeInfo, buffer );
    }

    static void AssignValueWrapper( ASOptional* opt, void* valueRef )
    {
        if( opt )
            opt->Set( valueRef );
    }

    static void SetValueWrapper( ASOptional* opt, void* valueRef )
    {
        if( opt )
            opt->Set( valueRef );
    }

    void* GetValuePointer()
    {
        asIScriptContext* ctx = asGetActiveContext();

        if( !hasValue )
        {
            if( ctx != nullptr )
                ctx->SetException( "null pointer value in optional!", true );
            return nullptr;
        }

        if( subTypeId & asTYPEID_OBJHANDLE )
        {
            return valueBuffer;
        }

        asITypeInfo* subType = GetSubTypeInfo();

        if( subType && (subType->GetFlags() & asOBJ_REF) )
        {
            return valueBuffer;
        }

        return valueBuffer;
    }

    static void* GetValueWrapper( ASOptional* opt )
    {
        return opt ? opt->GetValuePointer() : nullptr;
    }

    static inline void Register( asIScriptEngine* )
    {
#if ASOPTIONAL_ADD_NULLOPT
        REGISTER_OBJECT_TYPE( "nullopt_t", 0, asOBJ_REF, "" );

        REGISTER_OBJECT_BEHAVIOUR("nullopt_t", asBEHAVE_ADDREF, "void f()",
            asMETHOD(ASNullOptional, AddRef), asCALL_THISCALL, "" );

        REGISTER_OBJECT_BEHAVIOUR("nullopt_t", asBEHAVE_RELEASE, "void f()",
            asMETHOD(ASNullOptional, Release), asCALL_THISCALL, "" );

        REGISTER_OBJECT_BEHAVIOUR( "optional<T>", asBEHAVE_FACTORY, "optional<T>@ f(int &in, const nullopt_t@ value)",
            asFUNCTION((ASOptional*(*)(asITypeInfo*, ASNullOptional*))ASOptional::FactoryNullOptional), asCALL_CDECL, "Constructs a empty optional." );

        static ASNullOptional* g_nullopt = new ASNullOptional();

        REGISTER_GLOBAL_PROPERTY( "const nullopt_t@ nullopt", &g_nullopt, "" );
#endif
    }
};

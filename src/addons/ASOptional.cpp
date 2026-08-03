#include "addons/ASOptional.hpp"
#include "CASDocRegistry.hpp"

class CASDocOptional : public CASDocRegistry
{
    bool Register() override
    {
        #define NAME "optional<T>"

        return
        RegisterObjectType(
            "Generic container that may or may not contain a value of type T.\nThis is the equivalent to C++ std::optional."sv,
            "optional<class T>",
            0,
            asOBJ_REF | asOBJ_TEMPLATE
        ) &&
        RegisterObjectBehaviour(
            NAME,
            asBEHAVE_ADDREF,
            "void f()",
            asMETHOD( ASOptional, AddRef ),
            asCALL_THISCALL
        ) &&
        RegisterObjectBehaviour(
            NAME,
            asBEHAVE_RELEASE,
            "void f()",
            asMETHOD( ASOptional, Release ),
            asCALL_THISCALL
        ) &&
        RegisterObjectBehaviour(
            "Constructs an empty optional."sv,
            NAME,
            asBEHAVE_FACTORY,
            "optional<T>@ f( int &in )",
            asFUNCTIONPR( ASOptional::Factory, ( asITypeInfo* ), ASOptional* ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs an optional via an initialization list syntax."sv,
            NAME,
            asBEHAVE_LIST_FACTORY,
            "optional<T>@ f(int &in, const T &in value) { repeat T }",
            asFUNCTIONPR( ASOptional::FactoryGeneric, ( asITypeInfo*, void* ), ASOptional* ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs an optional initialized with a value."sv,
            NAME,
            asBEHAVE_FACTORY,
            "optional<T>@ f( int &in, const T &in value )",
            asFUNCTIONPR( ASOptional::FactoryWithValue, ( asITypeInfo*, void* ), ASOptional* ),
            asCALL_CDECL
        ) &&
        RegisterObjectMethod(
            "Returns true if the optional contains a value, false otherwise."sv,
            NAME,
            "bool has_value() const",
            asMETHOD( ASOptional, HasValue ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Clears the contained value and resets the optional to an empty state."sv,
            NAME,
            "void clear()",
            asMETHOD( ASOptional, Clear ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns a reference to the contained value. Throws a script exception if empty."sv,
            NAME,
            "const T& value() const", // -TODO Can modify ref value?
            asFUNCTIONPR( ASOptional::GetValueWrapper, ( ASOptional* ), void* ),
            asCALL_CDECL_OBJFIRST
        ) &&
        RegisterObjectMethod(
            "Sets the value contained in the optional."sv,
            NAME,
            "void set(const T &in value)",
            asFUNCTIONPR( ASOptional::SetValueWrapper, ( ASOptional*, void* ), void ),
            asCALL_CDECL_OBJFIRST
        ) &&
        RegisterObjectMethod(
            "Assigns a new value to the optional container."sv,
            NAME,
            "optional<T>& opAssign( const T &in value )",
            asFUNCTIONPR( ASOptional::AssignValueWrapper, ( ASOptional*, void* ), void ),
            asCALL_CDECL_OBJFIRST
        );
    }
};

CASDocRegisterInterface(CASDocOptional);

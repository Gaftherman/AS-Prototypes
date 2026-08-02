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
            asMETHOD(ASOptional, AddRef),
            asCALL_THISCALL
        ) &&
        RegisterObjectBehaviour(
            NAME,
            asBEHAVE_RELEASE,
            "void f()",
            asMETHOD(ASOptional, Release),
            asCALL_THISCALL
        ) &&
        RegisterObjectBehaviour(
            "Constructs a empty optional container."sv,
            NAME,
            asBEHAVE_FACTORY,
            "optional<T>@ f(int &in)",
            asFUNCTION((ASOptional*(*)(asITypeInfo*))ASOptional::Factory),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs a optional container with the given value in initialization list or empty list."sv,
            NAME,
            asBEHAVE_LIST_FACTORY,
            "optional<T>@ f(int &in, const T &in value) { repeat T }",
            asFUNCTION((ASOptional*(*)(asITypeInfo*, void*))ASOptional::FactoryGeneric),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs a optional container with the given value."sv,
            NAME,
            asBEHAVE_FACTORY,
            "optional<T>@ f(int &in, const T &in value)",
            asFUNCTION((ASOptional*(*)(asITypeInfo*, void*))ASOptional::FactoryWithValue),
            asCALL_CDECL
        ) &&
        RegisterObjectMethod(
            "Returns true if the optional contains a value, false otherwise."sv,
            NAME,
            "bool has_value() const",
            asMETHOD(ASOptional, HasValue),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Clears the contained value and resets the optional to an empty state."sv,
            NAME,
            "void clear()",
            asMETHOD(ASOptional, Clear),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns a reference to the contained value. Throws a script exception if empty."sv,
            NAME,
            "const T& value() const", // -TODO Can modify ref value?
            asFUNCTION(ASOptional::GetValueWrapper),
            asCALL_CDECL_OBJFIRST
        ) &&
        RegisterObjectMethod(
            "Sets the value contained in the optional."sv,
            NAME,
            "void set(const T &in value)",
            asFUNCTION(ASOptional::SetValueWrapper),
            asCALL_CDECL_OBJFIRST
        ) &&
        RegisterObjectMethod(
            "Assigns a new value to the optional container."sv,
            NAME,
            "optional<T>& opAssign(const T &in value)",
            asFUNCTION(ASOptional::AssignValueWrapper),
            asCALL_CDECL_OBJFIRST
        );
    }
};

CASDocRegisterInterface(CASDocOptional);

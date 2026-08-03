#include "addons/ASOptional.hpp"
#include "CASDocRegistry.hpp"

class CASDocOptional : public CASDocRegistry
{
    bool Register() override
    {
        #define NAME "optional"
        #define NAME_NULL "nullopt_t"

        static ASOptional::ASNullOptional* g_nullopt = new ASOptional::ASNullOptional();

        return
        RegisterObjectType(
            "Generic container that may or may not contain a value of type T.\nThis is the equivalent to C++ std::optional."sv,
            NAME "<class T>",
            0,
            asOBJ_REF | asOBJ_TEMPLATE
        ) &&
        RegisterObjectBehaviour(
            NAME "<T>",
            asBEHAVE_ADDREF,
            "void f()",
            asMETHOD( ASOptional, AddRef ),
            asCALL_THISCALL
        ) &&
        RegisterObjectBehaviour(
            NAME "<T>",
            asBEHAVE_RELEASE,
            "void f()",
            asMETHOD( ASOptional, Release ),
            asCALL_THISCALL
        ) &&
        RegisterObjectBehaviour(
            "Constructs an empty " NAME "."sv,
            NAME "<T>",
            asBEHAVE_FACTORY,
            NAME "<T>@ f( int &in )",
            asFUNCTIONPR( ASOptional::Factory, ( asITypeInfo* ), ASOptional* ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs an " NAME " via an initialization list syntax."sv,
            NAME "<T>",
            asBEHAVE_LIST_FACTORY,
            NAME "<T>@ f(int &in, const T &in value) { repeat T }",
            asFUNCTIONPR( ASOptional::FactoryGeneric, ( asITypeInfo*, void* ), ASOptional* ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs an " NAME " initialized with a value."sv,
            NAME "<T>",
            asBEHAVE_FACTORY,
            NAME "<T>@ f( int &in, const T &in value )",
            asFUNCTIONPR( ASOptional::FactoryWithValue, ( asITypeInfo*, void* ), ASOptional* ),
            asCALL_CDECL
        ) &&
        RegisterObjectMethod(
            "Returns true if the " NAME " contains a value, false otherwise."sv,
            NAME "<T>",
            "bool has_value() const",
            asMETHOD( ASOptional, HasValue ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Clears the contained value and resets the " NAME " to an empty state."sv,
            NAME "<T>",
            "void clear()",
            asMETHOD( ASOptional, Clear ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns a reference to the contained value. Throws a script exception if empty."sv,
            NAME "<T>",
            "const T& value() const", // -TODO Can modify ref value?
            asFUNCTIONPR( ASOptional::GetValueWrapper, ( ASOptional* ), void* ),
            asCALL_CDECL_OBJFIRST
        ) &&
        RegisterObjectMethod(
            "Sets the value contained in the " NAME "."sv,
            NAME "<T>",
            "void set(const T &in value)",
            asFUNCTIONPR( ASOptional::SetValueWrapper, ( ASOptional*, void* ), void ),
            asCALL_CDECL_OBJFIRST
        ) &&
        RegisterObjectMethod(
            "Assigns a new value to the " NAME " container."sv,
            NAME "<T>",
            NAME "<T>& opAssign( const T &in value )",
            asFUNCTIONPR( ASOptional::AssignValueWrapper, ( ASOptional*, void* ), void ),
            asCALL_CDECL_OBJFIRST
        ) &&
        // Start of nullopt registry.
        RegisterObjectType(
            "Generic empty property for using with " NAME "<T>"sv,
            NAME_NULL,
            0,
            asOBJ_REF | asOBJ_NOCOUNT
        ) &&
        RegisterObjectBehaviour(
            "Constructs an empty " NAME "."sv,
            NAME "<T>",
            asBEHAVE_FACTORY,
            NAME "<T>@ f(int &in, const " NAME_NULL "@ value)",
            asFUNCTIONPR( ASOptional::FactoryNullOptional, ( asITypeInfo*, ASOptional::ASNullOptional* ), ASOptional* ),
            asCALL_CDECL
        ) &&
        RegisterGlobalProperty(
            "Generic null/empty " NAME " for " NAME "<T> operations."sv,
            "const " NAME_NULL "@ nullopt",
            &g_nullopt
        );
    }
};

CASDocRegisterInterface(CASDocOptional);

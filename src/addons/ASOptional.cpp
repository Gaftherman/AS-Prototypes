#include "addons/ASOptional.hpp"
#include "CASDocRegistry.hpp"

/**
 * Options for optional registration.
 */

// Name of the angel script class template (class optional<T>)
#define OPTIONAL_NAME "optional"

// Whatever to register the nullopt class
#define OPTIONAL_REGISTER_NULLOPT 1

// Name of the nullopt class (class nullopt_t)
#define OPTIONAL_NULLOPT_NAME "nullopt_t"

// Name of the global property nullopt (nullopt_t@ nullopt)
#define OPTIONAL_NULLOPT_PROPERTY_NAME "nullopt"

class CASDocOptional : public CASDocRegistry
{
    bool Register() override
    {
#if OPTIONAL_REGISTER_NULLOPT
        static ASOptional::ASNullOptional* g_nullopt = new ASOptional::ASNullOptional();
#endif

        return
        RegisterObjectType(
            "Generic container that may or may not contain a value of type T.\nThis is the equivalent to C++ std::optional."sv,
            OPTIONAL_NAME "<class T>",
            0,
            asOBJ_REF | asOBJ_TEMPLATE
        ) &&
        RegisterObjectBehaviour(
            OPTIONAL_NAME "<T>",
            asBEHAVE_ADDREF,
            "void f()",
            asMETHOD( ASOptional, AddRef ),
            asCALL_THISCALL
        ) &&
        RegisterObjectBehaviour(
            OPTIONAL_NAME "<T>",
            asBEHAVE_RELEASE,
            "void f()",
            asMETHOD( ASOptional, Release ),
            asCALL_THISCALL
        ) &&
        RegisterObjectBehaviour(
            "Constructs an empty " OPTIONAL_NAME "."sv,
            OPTIONAL_NAME "<T>",
            asBEHAVE_FACTORY,
            OPTIONAL_NAME "<T>@ f( int &in )",
            asFUNCTIONPR( ASOptional::Factory, ( asITypeInfo* ), ASOptional* ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs an " OPTIONAL_NAME " via an initialization list syntax."sv,
            OPTIONAL_NAME "<T>",
            asBEHAVE_LIST_FACTORY,
            OPTIONAL_NAME "<T>@ f(int &in, const T &in value) { repeat T }",
            asFUNCTIONPR( ASOptional::FactoryGeneric, ( asITypeInfo*, void* ), ASOptional* ),
            asCALL_CDECL
        ) &&
        RegisterObjectBehaviour(
            "Constructs an " OPTIONAL_NAME " initialized with a value."sv,
            OPTIONAL_NAME "<T>",
            asBEHAVE_FACTORY,
            OPTIONAL_NAME "<T>@ f( int &in, const T &in value )",
            asFUNCTIONPR( ASOptional::FactoryWithValue, ( asITypeInfo*, void* ), ASOptional* ),
            asCALL_CDECL
        ) &&
        RegisterObjectMethod(
            "Returns true if the " OPTIONAL_NAME " contains a value, false otherwise."sv,
            OPTIONAL_NAME "<T>",
            "bool has_value() const",
            asMETHOD( ASOptional, HasValue ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Clears the contained value and resets the " OPTIONAL_NAME " to an empty state."sv,
            OPTIONAL_NAME "<T>",
            "void clear()",
            asMETHOD( ASOptional, Clear ),
            asCALL_THISCALL
        ) &&
        RegisterObjectMethod(
            "Returns a reference to the contained value. Throws a script exception if empty."sv,
            OPTIONAL_NAME "<T>",
            "const T& value() const", // -TODO Can modify ref value?
            asFUNCTIONPR( ASOptional::GetValueWrapper, ( ASOptional* ), void* ),
            asCALL_CDECL_OBJFIRST
        ) &&
        RegisterObjectMethod(
            "Sets the value contained in the " OPTIONAL_NAME "."sv,
            OPTIONAL_NAME "<T>",
            "void set(const T &in value)",
            asFUNCTIONPR( ASOptional::SetValueWrapper, ( ASOptional*, void* ), void ),
            asCALL_CDECL_OBJFIRST
        ) &&
        RegisterObjectMethod(
            "Assigns a new value to the " OPTIONAL_NAME " container."sv,
            OPTIONAL_NAME "<T>",
            OPTIONAL_NAME "<T>& opAssign( const T &in value )",
            asFUNCTIONPR( ASOptional::AssignValueWrapper, ( ASOptional*, void* ), void ),
            asCALL_CDECL_OBJFIRST
#if OPTIONAL_REGISTER_NULLOPT
        ) &&
        RegisterObjectType(
            "Generic empty property for using with " OPTIONAL_NAME "<T>"sv,
            OPTIONAL_NULLOPT_NAME,
            0,
            asOBJ_REF | asOBJ_NOCOUNT
        ) &&
        RegisterObjectBehaviour(
            "Constructs an empty " OPTIONAL_NAME "."sv,
            OPTIONAL_NAME "<T>",
            asBEHAVE_FACTORY,
            OPTIONAL_NAME "<T>@ f(int &in, const " OPTIONAL_NULLOPT_NAME "@ value)",
            asFUNCTIONPR( ASOptional::FactoryNullOptional, ( asITypeInfo*, ASOptional::ASNullOptional* ), ASOptional* ),
            asCALL_CDECL
        ) &&
        RegisterGlobalProperty(
            "Generic null/empty " OPTIONAL_NAME " for " OPTIONAL_NAME "<T> operations."sv,
            "const " OPTIONAL_NULLOPT_NAME "@ " OPTIONAL_NULLOPT_PROPERTY_NAME,
            &g_nullopt
#endif
        );
    }
};

CASDocRegisterInterface(CASDocOptional);

#undef OPTIONAL_NAME
#undef OPTIONAL_REGISTER_NULLOPT
#undef OPTIONAL_NULLOPT_NAME
#undef OPTIONAL_NULLOPT_PROPERTY_NAME

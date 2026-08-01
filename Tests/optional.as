#include "utils/title.as"

using namespace Tests;

class pev
{
    string targetname;

    pev( const string&in name )
    {
        this.targetname = name;
    }
}

#if ASOPTIONAL_ADD_NULLOPT
optional<double> MethodReturnNullOpt() { return nullopt; }
#endif
optional<int> MethodReturnOptionalEmpty() { return {}; }
optional<int> MethodReturnOptionalValid() { return {1}; }
optional<int> MethodReturnOptionalInvalid() { return {1,2}; }
void MethodOptionalImplicitConstruct( optional<int> iOpt ) {
    Expect("method with optional argument", true, iOpt.has_value() && iOpt.value() == 1 );
}

void main()
{
    title( "optional<T> class" );

    optional<int> optId;
    Expect( "Empty optional", false, optId.has_value() );

    optId.set(42);
    Expect("optional.set", true, optId.has_value() );

    Expect("optional.value", true, optId.value() == 42 );

    optId.clear();
    Expect("optional.clear", false, optId.has_value() );

    optional<string> optString("String");
    Expect("optional.value", true, optString.has_value() && optString.value() == "String" );

    optional<pev> optClass;
    pev entvar( "SomeDude" );
    optClass.set(entvar);
    Expect( "optional with class handles", true, entvar.targetname == optClass.value().targetname );

    optional<pev@> optClassHandle;
    pev@ entvarHandle = pev( "SomeDude2" );
    optClassHandle.set(entvarHandle);
    Expect( "optional with class handles", true, entvarHandle is optClassHandle.value() );

    Expect( "method return empty optional {}", true, !MethodReturnOptionalEmpty().has_value() );

    auto optmet = MethodReturnOptionalValid();
    Expect( "method return valid optional {1}", true, optmet.has_value() && optmet.value() == 1 );

    try {
        auto optmet2 = MethodReturnOptionalInvalid();
        Expect( "method return invalid optional {1,2}", true, false );
    }
    catch {
        Expect( "method return invalid optional {1,2}", true, true );
    }

    MethodOptionalImplicitConstruct(1);

#if ASOPTIONAL_ADD_NULLOPT
    optional<string> optFromNull(nullopt);
    Expect( "optional construct with nullopt", true, !optFromNull.has_value() );
    Expect( "optional return with nullopt", true, !MethodReturnNullOpt().has_value() );
#endif
}

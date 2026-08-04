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

optional<double> MethodReturnNullOpt()
{
    return nullopt;
}

void MethodArgumentNullOptionalEmpty( optional<int> iOpt = nullopt )
{
    Expect( "method with optional argument set to nullopt by default", true, !iOpt.has_value());
}

optional<int> MethodReturnOptionalEmpty()
{
    return {};
}

optional<int> MethodReturnOptionalValid()
{
    return { 1 };
}

optional<int> MethodReturnOptionalInvalid()
{
    return { 1, 2 };
}

#if FALSE
// This actually crashes the program.
optional<array<int>> MethodReturnOptionalDoubleInitializerList()
{
    return { { 1 } };
}
#endif

void MethodOptionalImplicitConstruct( optional<int> iOpt )
{
    Expect( "method with implicitly constructed optional from primitive", true, iOpt.has_value()&& iOpt.value()== 1 );
}

void MethodArgumentOptionalEmpty( optional<int> iOpt = optional<int>() )
{
    Expect( "method with optional argument set to \"optional<int>()\"", true, !iOpt.has_value());
}

void main()
{
    title( "optional<T> class" );

    optional<int> optId;
    Expect( "Default constructor creates an empty optional", false, optId.has_value());

    optId.set( 42 );
    Expect( "optional.has_value returns true after set()", true, optId.has_value());
    Expect( "optional.value returns the correct value after set()", true, optId.value()== 42 );

    optId.clear();
    Expect( "optional.has_value returns false after clear()", false, optId.has_value());

    optional<string> optString( "String" );
    Expect( "optional initialization with value constructor", true, optString.has_value()&& optString.value()== "String" );

    optional<pev> optClass;
    pev entvar( "SomeDude" );
    optClass.set( entvar );
    Expect( "optional copies and stores a static class value", true, entvar.targetname == optClass.value().targetname );

    optional<pev@> optClassHandle;
    pev@ entvarHandle = pev( "SomeDude2" );
    optClassHandle.set( entvarHandle );
    Expect( "optional correctly stores and matches class handles", true, entvarHandle is optClassHandle.value());

    @entvarHandle = null;
    Expect( "optional<@> adds reference count to handles", true, optClassHandle.has_value() && optClassHandle.value() !is null );

    weakref<pev> someDataWeak( optClassHandle.value() );
    optClassHandle.clear();
    Expect( "optional<@> clears reference count to handles", true, someDataWeak.get() is null );

    Expect( "method successfully returns empty optional using {}", true, !MethodReturnOptionalEmpty().has_value());

    auto optmet = MethodReturnOptionalValid();
    Expect( "method successfully returns valid optional using {1}", true, optmet.has_value()&& optmet.value()== 1 );

    try 
    {
        auto optmet2 = MethodReturnOptionalInvalid();
        Expect( "method fails to return invalid list {1,2} inside try block", true, false );
    }
    catch 
    {
        Expect( "method throws script exception on multi-value initialization list {1,2}", true, true );
    }

#if FALSE
    auto optArr = MethodReturnOptionalDoubleInitializerList();
    Expect( "method returns array initializer list nested in optional initializer list", true, optArr.has_value()&& optArr.value()[0] == 1 );
#endif

    MethodOptionalImplicitConstruct( 1 );

    optional<int> optHandleDecl(3);
    optional<int>@ optHandle = optHandleDecl;
    optHandleDecl.set(2);
    Expect( "optional<T>@ valid handle", true, optHandle !is null && optHandle.has_value() && optHandle.value() == 2 );

#if FALSE
    optional<int> optSetInt = 10;
    Expect( "optional==opAssign int", true, optSetInt.has_value() && optSetInt.value() == 10 );

    optional<string> optSetString = "string";
    Expect( "optional==opAssign string", true, optSetString.has_value() && optSetString.value() == "string" );

    optional<pev@> optSetHandle = entvarHandle;
    Expect( "optional==opAssign @handle", true, !optSetHandle.has_value() );
#endif

    optional<string> optFromNull( nullopt );
    Expect( "optional explicit constructor with nullopt", true, !optFromNull.has_value());
    Expect( "method returns empty optional via nullopt", true, !MethodReturnNullOpt().has_value());
    MethodArgumentNullOptionalEmpty();
    optional<int> optSetToNull(4);
    optSetToNull.set(nullopt);
    Expect( "optional explicit set(nullopt)", true, !optSetToNull.has_value());
    optional<int> optSetToNull2(4);
    optSetToNull2 = nullopt;
    Expect( "optional explicit ==opAssign nullopt", true, !optSetToNull2.has_value());

    MethodArgumentOptionalEmpty();

    optional<int> OptFromOpt_wValue = optional<int>(1);
    Expect( "optional explicit constructor with nullopt", true, OptFromOpt_wValue.has_value() && OptFromOpt_wValue.value() == 1 );

    optional<int>@ OptFromOpt_wValueHandle = OptFromOpt_wValue;
    optional<int> OptFromOpt_wValue2 = OptFromOpt_wValueHandle;
    Expect( "optional explicit constructor with nullopt@", true, OptFromOpt_wValue2.has_value() && OptFromOpt_wValue2.value() == 1 );

    optional<int> OptFromNull = null;
    Expect( "optional explicit constructor with null", true, !OptFromNull.has_value() );

    optional<int>@ OptHandleFromNull = null;
    Expect( "optional@ explicit constructor with null", true, OptHandleFromNull is null );

    optional<int>@ OptHandleWithValue = 1;
    Expect( "optional@ assign handle to null", true, OptHandleWithValue.has_value() && ( @OptHandleWithValue = null ) is null );

    @OptHandleWithValue = 1;
    Expect( "optional@ assign null handle to int", true, OptHandleWithValue.has_value() && OptHandleWithValue.value() == 1 );

    OptHandleWithValue = null;
    Expect( "optional@ assign (value) to null", true, OptHandleWithValue !is null && !OptHandleWithValue.has_value() );
}

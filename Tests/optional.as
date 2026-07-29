using namespace Tests;

class pev
{
    string targetname;

    pev( const string&in name )
    {
        this.targetname = name;
    }
}

void main()
{
    println("--- AngelScript optional Test ---");

    optional<int> optId;
    Expect( "Empty optional", false, optId.has_value() );

    optId.set(42);
    Expect("optional.set", true, optId.has_value() );

    Expect("optional.value", true, optId.value() == 42 );

    optId.clear();
    Expect("optional.clear", false, optId.has_value() );

    optional<string> optString("String");
    Expect("optional.value", true, optString.has_value() && optString.value() == "String" );

    optional<pev@> optClassHandle;
    pev@ entvarHandle = pev( "SomeDude2" );
    optClassHandle.set(entvarHandle);
    Expect( "optional with class handles", true, entvarHandle is optClassHandle.value() );
}

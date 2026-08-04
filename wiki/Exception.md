# Exception

This addon allows scripts to raise exceptions and make try-catch more useful.

This addon will provide the next API:
```as
namespace Exception
{
    /// Raises a script exception. if canCatch is false the script's catch block won't be called.
    void Throw(const string&in exception, bool canCatch = true);
    /// Raises a script exception with aditional metadata dictionary. if canCatch is false the script's catch block won't be called.
    void Throw(const string&in exception, dictionary@ additionalData, bool canCatch = true);
    /// Releases reference to the last exception. by default exceptions are cleared when new ones are created. Call this method after a catch block to clear all members.
    void Clear();
    /// Get the current exception count. this value only increases for explicit script-throw exceptions.
    int Id();
    /// Line of where the exception was raised.
    int Line();
    /// Get the current exception message.
    string Message();
    /// Get the call stack in string form.
    string CallStack();
    /// Get a handle to the dictionary data if the exception provided one when raised.
    dictionary@ Dictionary();
    /// Get the path to the script that raised the last exception.
    void ScriptSection(string&out absolute = void, string&out relative = void, string&out fileName = void, string&out methodName = void, string&out nameSpace = void, string&out objectName = void);
}
```

> Some examples and tests can be found at [Exception.as](../Tests/Exception.as)

# Registering the addon

- Get the ``.hpp`` file from [ASException.hpp](../src/addons/ASException.hpp)

```cpp
#include "ASException.hpp"
```

- Call the static method ``Register`` passing on the ``asIScriptEngine`` pointer as argument.
```cpp
ASException::Register(engine)
```

# NOTES:

- CScriptDictionary is expected to be registered.
    - If it is not then Dictionary() and the Throw overload won't be registered.

- ``std::string`` is used as angelscript's string.
    - To update that open the ASException file and follow the aliased ``ScriptString``:
        ```cpp
            // Set your custom string if any.
            using ScriptString = std::string;
        ```

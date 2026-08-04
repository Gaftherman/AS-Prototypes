# optional

This addon allows for a new template class ``optional`` which may or may not hold a value.

This is the equivalent to C++ ``std::optional``

This addon will provide the next API:
```as
class optional<T>
{
    /// Returns true if the optional contains a value, false otherwise.
    bool has_value() const;
    /// Clears the contained value and resets the optional to an empty state.
    void clear();
    /// Returns a reference to the contained value. Throws a script exception if empty.
    const T& value() const;
    /// Sets the value contained in the optional.
    void set(const T&in value);
    /// Assigns a new value to the optional container.
    optional<T>& opAssign(const T&in value);
    /// Assign one optional from another.
    optional<T>& opAssign(const optional<T>&in other);
    /// Sets the value contained in the optional.
    void set(const nullopt_t@ value);
    /// Assigns a new value to the optional container.
    optional<T>& opAssign(const nullopt_t@ value);
}
/// Generic empty property for using with optional<T>
class nullopt_t{}
/// Generic null/empty optional for optional<T> operations.
const nullopt_t@ nullopt;
```

> Some examples and tests can be found at [optional.as](../Tests/optional.as)

# Registering the addon

- Get the ``.hpp`` file from [ASOptional.hpp](../src/addons/ASOptional.hpp)

```cpp
#include "ASOptional.hpp"
```

- Call the static method ``Register`` passing on the ``asIScriptEngine`` pointer as argument.
```cpp
ASOptional::Register(engine)
```

# NOTES:

- ``nullopt`` can be disabled.
    - Pass false as second argument when registering ``ASOptional::Register(engine, false)``


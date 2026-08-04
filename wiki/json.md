# json

This addon allows for a new container class ``json`` which is a dictionary on esteroids with options to convert data from/to string.

This addon will provide the next API:
```as
...
```

> Some examples and tests can be found at [json.as](../Tests/json.as)

# Registering the addon

- Get the ``.hpp`` file from [ASJSON.hpp](../src/addons/ASJSON.hpp)

```cpp
#include "ASJSON.hpp"
```

- Call the static method ``Register`` passing on the ``asIScriptEngine`` pointer as argument.
```cpp
ASJSON::Register(engine)
```

# NOTES:

- ``load`` and ``dump`` methods are disabled by default.
    - To enable it you must provide your own file system. refeer to [File System Callbacks](#file-system-callbacks).
    
    
# File System Callbacks

To read and write files using ``dump`` and ``load`` methods in the ``json`` namespace you need to provide your own file system for opening, writing and reading files.

Our json callbacks will provide you the necesary.

> ```cpp
> using FILESYSTEM_CALLBACK = std::function<bool( std::filesystem::path& path, std::string& content, std::string& err )>;
> ```

- The provided ``std::filesystem::path& path`` contains the absolute path to the json file that you can use to delimit and white list paths.
    > You may modify the path reference for json to print the right message.
- The provided ``std::string& content`` contains the content provided by json to either write or read depending the callback that was called for dump or load.
- The provided ``std::string& err`` is empty and you may fill it as you prefeer, that string will be used if a exception was raised for the method callback origin or if your callback returned false
- return true to let json know the content is been writen/readed. return false otherwise.

### Basic example using C++ ``std::filesystem``
```cpp
ASJSON::FILESYSTEM_LOAD_CALLBACK = []( std::filesystem::path& path, std::string& content, std::string& err ) -> bool
{
    if( std::ifstream file(path); file.is_open() )
    {
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str(); 
        return true;
    }
    err = "Unexistent file";
    return false;
};
ASJSON::FILESYSTEM_DUMP_CALLBACK = []( std::filesystem::path& path, std::string& content, std::string& err ) -> bool
{
    if( std::ofstream file(path); file.is_open() )
    {
        file << content;
        return true;
    }
    err = "File is read-only";
    return false;
};
```

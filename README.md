# AS-Prototypes

AS-Prototypes is a C++ executable (`asrun`) designed to load, compile, and execute AngelScript (`.as`) files from the command line with standard add-ons and custom extension plugins pre-registered.

[Español](README_ES.md)

## Requirements

- CMake 3.15 or higher
- C++17 compatible compiler (MSVC 2019/2022, GCC, Clang)
- Git (required for CMake FetchContent)

## Building

Use CMake to configure and build the executable.

```powershell
# Configure build directory
cmake -B build

# Build Release binary
cmake --build build --config Release
```

The compiled binary will be located at `build/Release/asrun.exe`.

## Usage

Pass any AngelScript `.as` file directly to `asrun`:

```powershell
.\build\Release\asrun.exe path/to/script.as
```

### Running Tests

Run the integrated test suite with:

```powershell
.\build\Release\asrun.exe --test --no-pause
```

## Features & Plugins

### Standard Add-ons
- `CScriptBuilder`: Support for `#include` directives
- `scriptstdstring`: `string` data type and text utilities
- `scriptarray`: Generic dynamic array `array<T>`
- `scriptdictionary`: Key-value map `dictionary`
- `scriptmath`: Math functions (`sin`, `cos`, `sqrt`, `pow`)
- `scriptfile` & `scriptfilesystem`: File and directory I/O operations
- `scripthandle`: Reference and handle support
- `datetime`: Date and time utilities

### Custom Plugins
- `ASOptional` (`optional<T>`): Generic optional container supporting primitives, value types, and class handles.
- `ASJSON` (`JSON`): Native JSON parser and serializer.
- `ASDispose` (`Dispose`): Explicit resource disposal mechanism.
- `ASException` (`Exception`): Exception handling and catching from script execution.

## Project Structure

```text
AS-Prototypes/
├── CMakeLists.txt         # CMake build configuration
├── README.md              # English documentation
├── README_ES.md           # Spanish documentation
├── src/
│   ├── main.cpp            # CLI entry point
│   ├── addon_registry.h   # Addon registry interface
│   ├── addon_registry.cpp # Addon registration implementation
│   └── addons/            # Custom plugins
│       ├── ASOptional.hpp # optional<T> implementation
│       ├── ASJSON.hpp     # JSON parser implementation
│       ├── ASDispose.hpp  # Dispose pattern implementation
│       └── ASException.hpp# Exception handling implementation
└── Tests/                 # Automated test suite
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[zlib](https://opensource.org/licenses/Zlib)

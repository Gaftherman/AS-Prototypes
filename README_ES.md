# AS-Prototypes

AS-Prototypes es un ejecutable en C++ (`asrun`) diseñado para cargar, compilar y ejecutar archivos de AngelScript (`.as`) desde la línea de comandos con add-ons estándar y plugins personalizados pre-registrados.

[English](README.md)

## Requisitos

- CMake 3.15 o superior
- Compilador C++ compatible con C++17 (MSVC 2019/2022, GCC, Clang)
- Git (requerido por CMake FetchContent)

## Compilación

Utiliza CMake para configurar y compilar el ejecutable.

```powershell
# Configurar el directorio de construcción
cmake -B build

# Compilar el binario en Release
cmake --build build --config Release
```

El ejecutable compilado se ubicará en `build/Release/asrun.exe`.

## Uso

Pasa cualquier archivo de AngelScript `.as` directamente a `asrun`:

```powershell
.\build\Release\asrun.exe ruta/al/script.as
```

### Ejecución de Pruebas

Ejecuta la suite de pruebas integrada con:

```powershell
.\build\Release\asrun.exe --test --no-pause
```

## Características y Plugins

### Add-ons Estándar
- `CScriptBuilder`: Soporte para la directiva `#include`
- `scriptstdstring`: Tipo de dato `string` y utilidades de texto
- `scriptarray`: Arreglo dinámico genérico `array<T>`
- `scriptdictionary`: Diccionario clave-valor `dictionary`
- `scriptmath`: Funciones matemáticas (`sin`, `cos`, `sqrt`, `pow`)
- `scriptfile` & `scriptfilesystem`: Operaciones I/O de archivos y directorios
- `scripthandle`: Soporte para referencias y handles
- `datetime`: Operaciones con fechas y tiempo

### Plugins Personalizados
- `ASOptional` (`optional<T>`): Contenedor opcional genérico con soporte para tipos primitivos, objetos por valor y handles de clases.
- `ASJSON` (`JSON`): Parser y serializador JSON nativo.
- `ASDispose` (`Dispose`): Mecanismo de liberación explícita de recursos.
- `ASException` (`Exception`): Sistema de captura y manejo de excepciones desde script.

## Estructura del Proyecto

```text
AS-Prototypes/
├── CMakeLists.txt         # Configuración del sistema de construcción CMake
├── README.md              # Documentación en inglés
├── README_ES.md           # Documentación en español
├── src/
│   ├── main.cpp            # Punto de entrada CLI
│   ├── addon_registry.h   # Interfaz del registro de add-ons
│   ├── addon_registry.cpp # Implementación del registro de add-ons
│   └── addons/            # Plugins personalizados
│       ├── ASOptional.hpp # Implementación de optional<T>
│       ├── ASJSON.hpp     # Implementación del parser JSON
│       ├── ASDispose.hpp  # Implementación de Dispose
│       └── ASException.hpp# Implementación de manejo de excepciones
└── Tests/                 # Suite de pruebas automatizadas
```

## Contribuciones

Las contribuciones son bienvenidas. Para cambios importantes, abre un issue primero para discutir lo que te gustaría cambiar.

Por favor asegúrate de actualizar las pruebas según corresponda.

## Licencia

[zlib](https://opensource.org/licenses/Zlib)

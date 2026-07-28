# AngelScript CLI Runner (`asrun`)

Un ejecutable en C++ diseñado para cargar, compilar y ejecutar archivos `.as` ([AngelScript](https://github.com/anjo76/angelscript)) desde la línea de comandos con todos sus add-ons estándar pre-registrados.

---

## 🚀 Características

- **Add-ons de AngelScript Integrados**:
  - `CScriptBuilder` (Soporte para directivas `#include`)
  - `scriptstdstring` (`std::string` y utilidades)
  - `scriptarray` (Arreglos dinámicos `array<T>`)
  - `scriptdictionary` (Diccionarios clave-valor `dictionary`)
  - `scriptmath` (Funciones matemáticas: `sin`, `cos`, `sqrt`, `pow`, etc.)
  - `scriptfile` & `scriptfilesystem` (Lectura, escritura y operaciones de archivos/carpetas)
  - `scripthandle` (Manejo de referencias)
  - `datetime` (Operaciones con fechas y tiempo)
- **Consola integradas**: Funciones `print(const string &in)` y `println(const string &in)`.
- **Soporte Drag & Drop**: Puedes arrastrar y soltar cualquier archivo `.as` directamente sobre el ejecutable `asrun.exe` en Windows. La consola se mantendrá abierta mostrando el resultado hasta que presiones `ENTER`.
- **Diagnósticos de errores**: Imprime advertencias y errores de compilación indicando archivo, línea y columna exactos.
- **Gestión con CMake FetchContent**: Descarga e integra la librería oficial `anjo76/angelscript` de forma automática durante la compilación.
- **Automatización CI/CD (GitHub Actions)**: Genera automáticamente el ejecutable `asrun.exe` como artefacto descargable en cada `push` / `pull request` y adjunta el archivo `.exe` en los *Releases* cuando publicas un tag (ej. `v1.0.0`).

---

## 🛠️ Requisitos de Compilación

- **CMake** 3.15 o superior
- Compilador de C++ con soporte para **C++17** (MSVC 2019/2022, GCC, Clang)
- Git (para que CMake FetchContent descargue AngelScript)

---

## 📦 Compilación

### En Windows (PowerShell / Visual Studio / CMD)

```powershell
# 1. Configurar proyecto con CMake
cmake -B build

# 2. Compilar ejecutable (Configuración Release)
cmake --build build --config Release
```

El ejecutable resultante estará ubicado en `build/Release/asrun.exe` (o `build/asrun` según el generador).

---

## 💻 Uso

Ejecuta cualquier script `.as` pasándolo como parámetro al ejecutable `asrun`:

```powershell
.\build\Release\asrun.exe path/to/script.as
```

### Ejemplos Incluidos

Puedes probar los ejemplos ubicados en la carpeta `examples/`:

1. **Hola Mundo Basico**:
   ```powershell
   .\build\Release\asrun.exe examples/hello.as
   ```

2. **Funciones Matemáticas**:
   ```powershell
   .\build\Release\asrun.exe examples/math_test.as
   ```

3. **Arreglos y Diccionarios**:
   ```powershell
   .\build\Release\asrun.exe examples/array_dict_test.as
   ```

4. **Entrada / Salida de Archivos**:
   ```powershell
   .\build\Release\asrun.exe examples/file_io_test.as
   ```

---

## 📝 Estructura del Proyecto

```
AS-Prototypes/
├── CMakeLists.txt         # Configuración del sistema de construcción CMake
├── .gitignore             # Archivos omitidos en Git
├── README.md              # Documentación principal
├── src/
│   ├── main.cpp            # Punto de entrada CLI y ejecutor AngelScript
│   ├── addon_registry.h   # Declaración de registro de add-ons
│   └── addon_registry.cpp # Implementación del registro de add-ons
└── examples/
    ├── hello.as           # Ejemplo básico de script
    ├── math_test.as       # Ejemplo de add-on de matemáticas
    ├── array_dict_test.as # Ejemplo de arreglos y diccionarios
    └── file_io_test.as    # Ejemplo de lectura/escritura de archivos
```

---

## 📜 Licencia
Este proyecto utiliza la biblioteca [AngelScript](https://github.com/anjo76/angelscript) bajo la licencia zlib.

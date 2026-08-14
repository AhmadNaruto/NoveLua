# NoveLua

NoveLua is a clean, minimal, high-performance, and safe Kotlin/Java ↔ JNI ↔ Luau VM bridge library designed for Android ART.

It is a JNI-based port of the core functionalities of `hollow-cube/luau-java`, replacing Java's Foreign Function & Memory (FFM) API with standard JNI methods to run natively in Android mobile app environments.

---

## Features

* **High Performance**: Native C++ bridge linking Kotlin directly with Luau VM C APIs.
* **Safety & Automatic Garbage Collection**: Kotlin callbacks are registered as custom user data with GC-dtors. The native layer manages global JNI references (`NewGlobalRef` / `DeleteGlobalRef`) preventing leaks.
* **Coroutine Support**: Complete integration for `LuaYieldableFunction` allowing yield and resume operations from Kotlin.
* **64-bit Integer Support**: Direct support for 64-bit integers (`Long` mapping) utilizing Luau's `lua_pushinteger64` and `lua_tointeger64` without precision loss.
* **Error and Traceback Propagation**: Luau script exceptions are caught and mapped back to Kotlin as `LuaException`. The Luau call stack is parsed and merged into the JVM exception `StackTraceElement` with the class name `"lua"`.
* **Thread Sandboxing**: Isolate environment libraries on specific coroutine execution threads.

---

## Project Structure

```
├── CMakeLists.txt              # Root CMake configuration (optimizations, Luau modules absorption)
├── build.gradle.kts            # Root Gradle configuration
├── settings.gradle.kts         # Root settings gradle
├── api_documentation.md        # Detailed API Documentation (in Indonesian)
└── src
    ├── main
    │   ├── cpp
    │   │   ├── luau/           # Upstream Luau VM submodule
    │   │   ├── luaujava.cpp    # Ported utility source
    │   │   ├── luaujavac.cpp   # Ported compiler source
    │   │   └── novelua_jni.cpp # JNI Native trampoline and wrappers
    │   └── kotlin/             # Kotlin library bindings
    │       └── io/github/novela/luau/
    │           ├── LuaEngine.kt
    │           ├── LuaState.kt
    │           ├── LuaValue.kt
    │           └── ...
    └── test
        └── kotlin/             # JUnit test suites
            └── io/github/novela/luau/NoveLuaTest.kt
```

---

## Build Instructions

NoveLua compiles both the native C++ library (`libnovelua.so`) and the Kotlin JVM class files.

### Prerequisites
* JDK 21+
* CMake 3.15+
* C++17 Compiler (GCC/Clang or Android NDK)

### Build & Run Tests
To run the automated build and JUnit test suite:
```bash
./gradlew test
```
The custom task `compileNatives` will automatically build the JNI shared library and copy it to the build folder before running the Kotlin tests.

---

## Quick Examples

### 1. Compile & Execute a Script
```kotlin
import io.github.novela.luau.LuaEngine
import io.github.novela.luau.LuaState

LuaEngine().use { engine ->
    val L = engine.mainState
    
    // Execute script
    engine.execute("result = 10 * 5 + 2")
    
    // Get variable
    L.getField(LuaState.GLOBALS_INDEX, "result")
    val valNum = L.toValue(-1)
    println("Result: ${valNum}") // Output: Number(value=52.0)
    L.pop(1)
}
```

### 2. Register Callback
```kotlin
val logFunc = LuaFunction { state ->
    val msg = state.toString(1) ?: ""
    println("[Kotlin] Log: $msg")
    0 // Return 0 values to Lua
}

L.pushFunction(logFunc, "log")
L.setField(LuaState.GLOBALS_INDEX, "log")

engine.execute("log('Hello from Luau!')")
```

### 3. Handle Exceptions with Lua Frames
```kotlin
try {
    engine.execute("error('Intentional error!')")
} catch (e: LuaException) {
    // Luau traceback frames are merged directly into Java/Kotlin stacktrace
    e.printStackTrace()
}
```

---

## Documentation

For full API specifications, memory boundary details, and implementation guidelines, refer to the [API Documentation (Indonesian)](file:///data/data/com.termux/files/home/novelua/api_documentation.md).

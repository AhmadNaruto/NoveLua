# NoveLua Native SDK - Detailed Technical Summary

**NoveLua** is a high-performance, ultra-compact native SDK for Android (`arm64-v8a`, API level 24+). It provides embedding capabilities for scripting engines (**Luau** & **QuickJS**), a high-speed HTML5 DOM parser with CSS selector support (**LexSoup / Lexbor**), a feature-complete regular expression engine (**PCRE2**), and a native **DOM Interoperability Layer** bridging HTML nodes to Lua and JavaScript execution environments.

---

## 🚀 Key Specifications & Architecture Overview

- **Core Languages**: C++20, C17, Kotlin 2.0
- **Android Target**: Min SDK 24+, Target SDK 34
- **Supported ABI**: Exclusively **`arm64-v8a`**
- **Build System**: Gradle 8.x (Kotlin DSL), CMake 3.22+, Android NDK
- **C++ Runtime Policy**: Self-contained static linking (`c++_static` & `c++abi`), ensuring **100% independence from `libc++_shared.so`** to prevent dependency collisions.
- **License**: MIT License
- **Total Binary Footprint**: **~1.33 MB** total combined AAR package (~3.89 MB uncompressed native `.so` total)

---

## 🧩 Architectural Components & Modules

The SDK is organized into modular Gradle components (`:modules:*`), each encapsulating a dedicated native library and its corresponding Kotlin JNI wrapper:

### 1. Luau Scripting Engine (`:modules:luau`)
- **Engine**: Luau VM (Roblox fast, type-aware Lua dialect derived from Lua 5.1).
- **Purpose**: Provides sandboxed, memory-efficient script execution for dynamic application logic.
- **AAR Size**: **~365 KB** (Native `.so` ~930 KB)

### 2. QuickJS Engine (`:modules:quickjs`)
- **Engine**: QuickJS (Lightweight C-based ES2020 JavaScript engine created by Fabrice Bellard).
- **Purpose**: Executes modern JavaScript (ES2020 standard compliance) with low memory footprint and fast startup time.
- **AAR Size**: **~428 KB** (Native `.so` ~956 KB)

### 3. LexSoup HTML Parser (`:modules:lexsoup`)
- **Engine**: Lexbor (High-performance C HTML5 parser & CSS selector engine).
- **Purpose**: Parses raw HTML documents into a structured DOM, supporting CSS3 selector queries (`#id`, `.class`, element hierarchy) and DOM node manipulations.
- **AAR Size**: **~300 KB** (Native `.so` ~1.37 MB)

### 4. PCRE2 Regex Engine (`:modules:regex`)
- **Engine**: PCRE2 (Perl-Compatible Regular Expressions v10+ in C).
- **Purpose**: Full-featured regex matching, capturing groups, and substitution logic bypassing Android's built-in regex limits.
- **AAR Size**: **~237 KB** (Native `.so` ~636 KB)

### 5. DOM Interoperability Layer (`:modules:interop`)
- **Purpose**: Provides automated binding between LexSoup HTML DOM nodes and Luau/QuickJS scripting runtimes.
- **Key Feature**: Enables Lua/JS code running inside NoveLua VMs to directly query, inspect, and update the host HTML DOM.
- **AAR Size**: **~10 KB** (Pure Kotlin / JNI Interop glue layer)

### 6. Benchmark & Demo (`:modules:benchmark`, `:app`)
- **Purpose**: `:modules:benchmark` contains performance micro-benchmarks; `:app` acts as an integration testbed and demonstration app.

---

## 📊 Binary Footprint & Size Analysis

| Module Path | Output AAR File | Native Library (`.so`) | AAR Size |
| :--- | :--- | :---: | :---: |
| **`modules/regex`** | `regex-release.aar` | ~636 KB | **~237 KB** |
| **`modules/lexsoup`** | `lexsoup-release.aar` | ~1.37 MB | **~300 KB** |
| **`modules/luau`** | `luau-release.aar` | ~930 KB | **~365 KB** |
| **`modules/quickjs`** | `quickjs-release.aar` | ~956 KB | **~428 KB** |
| **`modules/interop`** | `interop-release.aar` | - | **~10 KB** |
| **Total SDK Combined** | **5 NoveLua AAR Libraries** | **~3.89 MB** | **~1.33 MB** |

---

## ⚡ Native Compilation & Optimization Strategy

NoveLua enforces strict C++ optimization flags in `cmake/common.cmake` to achieve minimum binary footprint without sacrificing execution speed:

- **Optimization Profile**: `MinSizeRel` with Clang `-Oz` (optimize for size beyond `-Os`).
- **Link-Time Optimization (LTO)**: ThinLTO (`-flto=thin`) across C/C++ compilation units.
- **Dead Code & Symbol Stripping**:
  - `-ffunction-sections`, `-fdata-sections`
  - `-Wl,--gc-sections` (garbage collect unused sections)
  - `-Wl,--strip-all` (strip all debug symbols)
  - `-Wl,--icf=all` (Identical Code Folding)
- **Symbol Visibility**: `-fvisibility=hidden` & `-fvisibility-inlines-hidden`.
- **Runtime Features**: Disabled RTTI (`-fno-rtti`) to reduce vtable overhead.
- **Static STL Linkage**: Forced `-nostdlib++` and explicit linkage with `c++_static` and `c++abi`.

---

## 💻 Kotlin API & Code Usage Examples

### 1. Executing Luau Script
```kotlin
import io.github.novelua.luau.VM

fun runLuauScript() {
    VM().use { vm ->
        val result = vm.eval("local a = 15; local b = 25; return a + b")
        println("Luau Result: $result") // Output: 40
    }
}
```

### 2. Executing QuickJS JavaScript
```kotlin
import io.github.novelua.js.Runtime
import io.github.novelua.js.Context

fun runJavaScript() {
    Runtime().use { runtime ->
        Context(runtime).use { context ->
            val result = context.eval("(function(x) { return x * x; })(9);")
            println("QuickJS Result: ${result.asInt()}") // Output: 81
            result.close()
        }
    }
}
```

### 3. Parsing HTML DOM & CSS Selectors with LexSoup
```kotlin
import io.github.novelua.lexsoup.Parser

fun parseHtml() {
    val html = """
        <div id="content">
            <h1 class="header">NoveLua HTML Parser</h1>
            <p>Fast Lexbor engine embedded in Kotlin.</p>
        </div>
    """.trimIndent()

    val document = Parser.parse(html)
    val title = document.selectFirst(".header")?.text()
    println("Extracted Title: $title")
}
```

### 4. High-Performance Regex with PCRE2
```kotlin
import io.github.novelua.regex.Regex
import io.github.novelua.regex.Options

fun matchRegex() {
    Regex.compile("""([a-zA-Z0-9._%+-]+)@([a-zA-Z0-9.-]+\.[a-zA-Z]{2,})""", Options(ignoreCase = true)).use { pattern ->
        val matcher = pattern.matcher("Contact us at support@novelua.dev")
        if (matcher.find()) {
            println("User: ${matcher.group(1)}")   // support
            println("Domain: ${matcher.group(2)}") // novelua.dev
        }
    }
}
```

### 5. DOM Interoperability (Binding DOM to Luau/QuickJS)
```kotlin
import io.github.novelua.interop.registerLexSoup
import io.github.novelua.interop.bindDocument
import io.github.novelua.lexsoup.Parser
import io.github.novelua.luau.VM

fun runDomInterop() {
    val doc = Parser.parse("<h1 id='title'>Hello from NoveLua</h1>")

    VM().use { vm ->
        vm.registerLexSoup()
        vm.bindDocument("document", doc)

        vm.eval("""
            local text = document:selectFirst("#title"):text()
            print("Lua DOM Access:", text)
        """)
    }
}
```

---

## 🛠️ Android Integration Instructions

### Step 1: Add AAR Binaries
Place the output `.aar` files into your Android app project directory (e.g. `app/libs/`).

### Step 2: Configure `app/build.gradle.kts`
```kotlin
android {
    compileSdk = 34

    defaultConfig {
        minSdk = 24
        targetSdk = 34

        // Mandatory: NoveLua native builds target arm64-v8a
        ndk {
            abiFilters.add("arm64-v8a")
        }
    }
}

dependencies {
    implementation(files("libs/novelua-regex.aar"))
    implementation(files("libs/novelua-lexsoup.aar"))
    implementation(files("libs/novelua-luau.aar"))
    implementation(files("libs/novelua-quickjs.aar"))
    implementation(files("libs/novelua-interop.aar"))
}
```

---

## 🛠️ Build Automation & Helper Scripts

The repository includes Python-based automation scripts for JNI/C++/Kotlin code generation and documentation checking:

- `gen_kt_all.py` / `gen_kt.py`: Generates Kotlin API wrapper facades.
- `gen_cpp.py` / `gen_headers.py` / `gen_jni.py`: Generates C++ header bindings and JNI glue functions.
- `add_kdoc.py` / `check_kdoc.py`: Enforces KDoc documentation coverage across Kotlin files.
- `generate_phase6.py` / `generate_phase6_part2.py`: Extended phase code generation utilities.

---

## 📦 How to Build from Source

To compile all SDK modules and output release `.aar` packages locally:

```bash
# 1. Clone repository with submodules (if any)
git clone --recursive https://github.com/AhmadNaruto/NoveLua.git
cd NoveLua

# 2. Run unit tests
./gradlew test

# 3. Assemble all release AAR modules and APK
./gradlew assembleRelease
```

Generated AAR outputs will be located at:
- `modules/regex/build/outputs/aar/regex-release.aar`
- `modules/lexsoup/build/outputs/aar/lexsoup-release.aar`
- `modules/luau/build/outputs/aar/luau-release.aar`
- `modules/quickjs/build/outputs/aar/quickjs-release.aar`
- `modules/interop/build/outputs/aar/interop-release.aar`

# NoveLua Native SDK

[![Android Engine](https://img.shields.io/badge/Android-SDK_24%2B-brightgreen.svg)](https://developer.android.com)
[![Architecture](https://img.shields.io/badge/ABI-arm64--v8a-blue.svg)](https://developer.android.com/ndk)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-orange.svg)](https://isocpp.org)
[![Kotlin](https://img.shields.io/badge/Kotlin-2.0-purple.svg)](https://kotlinlang.org)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

**NoveLua** adalah SDK Native berkinerja tinggi untuk Android yang menyediakan *scripting engine* (**Luau** & **QuickJS**), *HTML DOM parser* berkecepatan tinggi (**LexSoup / Lexbor**), dan *regex engine* berfitur lengkap (**PCRE2**) dalam ukuran binary yang sangat kecil (~1.33 MB total AAR).

---

## 🌟 Fitur Utama

- **Zero External Runtime Dependency**: Menggunakan static STL (`c++_static` & `c++abi`), sehingga **100% bebas dari `libc++_shared.so`** dan mencegah konflik dependency dengan library lain.
- **Ultra Compact Binary**: Dibangun dengan profil optimasi `MinSizeRel`, Clang `-Oz`, ThinLTO, hidden symbol visibility, dan section garbage collection.
- **Multi-Engine Support**:
  - 🚀 **Luau Engine**: Luau VM (Roblox Lua dialect) yang cepat dan hemat memori.
  - ⚡ **QuickJS Engine**: Modern JavaScript (ES2020) engine yang ringan.
  - 🥣 **LexSoup HTML Parser**: Lexbor C HTML5 parser & CSS selector engine.
  - 🔍 **PCRE2 Regex Engine**: Perl-compatible regular expressions engine.
  - 🔗 **DOM Interop Layer**: Binding otomatis antara HTML DOM node dengan engine Luau & QuickJS.

---

## 📦 Ukuran Binary & Footprint AAR

| Modul | File AAR Output | Ukuran Native Library (`.so`) | Ukuran File AAR |
| :--- | :--- | :---: | :---: |
| **`modules:regex`** | `modules/regex/build/outputs/aar/regex-release.aar` | ~636 KB | **~237 KB** |
| **`modules:lexsoup`** | `modules/lexsoup/build/outputs/aar/lexsoup-release.aar` | ~1.37 MB | **~300 KB** |
| **`modules:luau`** | `modules/luau/build/outputs/aar/luau-release.aar` | ~930 KB | **~365 KB** |
| **`modules:quickjs`** | `modules/quickjs/build/outputs/aar/quickjs-release.aar` | ~956 KB | **~428 KB** |
| **`modules:interop`** | `modules/interop/build/outputs/aar/interop-release.aar` | - | **~10 KB** |
| **Total Combined** | **5 Modul SDK NoveLua** | **~3.89 MB** | **~1.33 MB** |

---

## 🛠️ Panduan Penggunaan AAR di Aplikasi Android

### Langkah 1: Salin File AAR

1. Buat direktori `libs/` di dalam modul aplikasi Android Anda (contoh: `app/libs/`).
2. Salin file `.aar` dari hasil build NoveLua ke folder `app/libs/`:
   - `novelua-regex.aar`
   - `novelua-lexsoup.aar`
   - `novelua-luau.aar`
   - `novelua-quickjs.aar`
   - `novelua-interop.aar`

---

### Langkah 2: Tambahkan Dependency di `build.gradle.kts`

Buka file `app/build.gradle.kts` pada proyek Android Anda dan tambahkan konfigurasi dependency:

```kotlin
android {
    namespace = "com.example.myapp"
    compileSdk = 34

    defaultConfig {
        minSdk = 24
        targetSdk = 34

        // NoveLua dikompilasi khusus untuk arsitektur ARM64 (arm64-v8a)
        ndk {
            abiFilters.add("arm64-v8a")
        }
    }
}

dependencies {
    // Menambahkan AAR secara lokal
    implementation(files("libs/novelua-regex.aar"))
    implementation(files("libs/novelua-lexsoup.aar"))
    implementation(files("libs/novelua-luau.aar"))
    implementation(files("libs/novelua-quickjs.aar"))
    implementation(files("libs/novelua-interop.aar"))
}
```

Jika menggunakan Gradle `build.gradle` (Groovy DSL):

```groovy
android {
    defaultConfig {
        ndk {
            abiFilters "arm64-v8a"
        }
    }
}

dependencies {
    implementation files('libs/novelua-regex.aar')
    implementation files('libs/novelua-lexsoup.aar')
    implementation files('libs/novelua-luau.aar')
    implementation files('libs/novelua-quickjs.aar')
    implementation files('libs/novelua-interop.aar')
}
```

---

## 💡 Contoh Penggunaan Kode (Kotlin)

### 1. Luau Script Execution

```kotlin
import io.github.novelua.luau.VM

fun runLuauExample() {
    VM().use { vm ->
        val result = vm.eval("return 10 + 20")
        println("Hasil Luau: $result") // Output: 30
    }
}
```

### 2. QuickJS JavaScript Engine

```kotlin
import io.github.novelua.js.Runtime
import io.github.novelua.js.Context

fun runQuickJSExample() {
    Runtime().use { runtime ->
        Context(runtime).use { context ->
            val result = context.eval("(function(a, b) { return a * b; })(6, 7);")
            println("Hasil QuickJS: ${result.asInt()}") // Output: 42
            result.close()
        }
    }
}
```

### 3. LexSoup HTML DOM Parser & CSS Selectors

```kotlin
import io.github.novelua.lexsoup.Parser

fun parseHtmlExample() {
    val html = """
        <html>
            <body>
                <div class="content">
                    <h1 id="title">Selamat Datang di NoveLua</h1>
                    <p class="desc">Engine HTML Parser Tercepat</p>
                </div>
            </body>
        </html>
    """.trimIndent()

    val doc = Parser.parse(html)
    val titleElement = doc.selectFirst("#title")
    println("Judul: ${titleElement?.text()}") // Output: Selamat Datang di NoveLua

    val descElement = doc.selectFirst(".desc")
    println("Deskripsi: ${descElement?.text()}") // Output: Engine HTML Parser Tercepat
}
```

### 4. PCRE2 Regex Engine

```kotlin
import io.github.novelua.regex.Regex
import io.github.novelua.regex.Options

fun regexExample() {
    Regex.compile("""(\d{4})-(\d{2})-(\d{2})""", Options(ignoreCase = true)).use { pattern ->
        val matcher = pattern.matcher("Tanggal rilis: 2026-08-01")
        if (matcher.find()) {
            println("Tahun: ${matcher.group(1)}") // 2026
            println("Bulan: ${matcher.group(2)}") // 08
            println("Hari:  ${matcher.group(3)}") // 01
        }
    }
}
```

### 5. DOM Interoperability (Bind DOM to Luau/QuickJS)

```kotlin
import io.github.novelua.interop.registerLexSoup
import io.github.novelua.interop.bindDocument
import io.github.novelua.lexsoup.Parser
import io.github.novelua.luau.VM

fun domInteropExample() {
    val doc = Parser.parse("<div id='app'>Hello World</div>")
    
    VM().use { vm ->
        // Register binding LexSoup ke Luau VM
        vm.registerLexSoup()
        vm.bindDocument("document", doc)
        
        // Akses DOM langsung dari dalam script Luau
        vm.eval("""
            local title = document:selectFirst("#app"):text()
            print("DOM dari Luau:", title)
        """)
    }
}
```

---

## 🏗️ Cara Build AAR dari Source Code

Jika Anda ingin mengompilasi SDK dari source code secara lokal:

```bash
# Clone repository
git clone --recursive https://github.com/AhmadNaruto/NoveLua.git
cd NoveLua

# Jalankan pengujian unit
./gradlew test

# Compile seluruh modul AAR dan APK rilis
./gradlew assembleRelease
```

File `.aar` yang dihasilkan akan tersimpan di:
- `modules/regex/build/outputs/aar/regex-release.aar`
- `modules/lexsoup/build/outputs/aar/lexsoup-release.aar`
- `modules/luau/build/outputs/aar/luau-release.aar`
- `modules/quickjs/build/outputs/aar/quickjs-release.aar`
- `modules/interop/build/outputs/aar/interop-release.aar`

---

## 📄 Lisensi

Proyek ini dirilis di bawah lisensi [MIT License](LICENSE).

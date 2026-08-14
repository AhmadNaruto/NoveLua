# Panduan Integrasi: Menambahkan Lexbor HTML Parser ke Engine NoveLua

Jika Anda ingin agar pustaka **Lexbor** (HTML parser berkinerja tinggi dalam bahasa C) dapat diakses secara global di dalam skrip Luau pada proyek NoveLua, Anda harus mengompilasikannya ke dalam shared library (`libnovelua.so`) dan mendaftarkannya sebagai pustaka standar global pada VM Luau.

Berikut adalah langkah-langkah arsitektural dan teknis yang perlu dilakukan:

---

## Langkah 1: Menambahkan Source Code Lexbor dan Binding C++

Lexbor adalah library C murni. Anda memerlukan pustaka Lexbor itu sendiri serta file jembatan (binding) C++ yang menerjemahkan API Lexbor ke format API C Luau.

1. **Unduh Source Code Lexbor**:
   Masukkan source code core Lexbor ke dalam folder:
   `src/main/cpp/lexbor/`
2. **Buat File Binding (`novelua_lexbor.h` dan `novelua_lexbor.cpp`)**:
   File ini bertanggung jawab membungkus API Lexbor ke dalam fungsi-fungsi yang dapat dipanggil oleh Luau.
   
   Contoh header `novelua_lexbor.h`:
   ```cpp
   #ifndef NOVELUA_LEXBOR_H
   #define NOVELUA_LEXBOR_H

   #include "lua.h"

   // Fungsi entry point untuk mendaftarkan library ke state Luau
   int luaopen_lexbor(lua_State* L);

   #endif
   ```

---

## Langkah 2: Memperbarui `CMakeLists.txt`

Anda harus mendaftarkan direktori file header Lexbor dan mengompilasi semua file source Lexbor bersama dengan target `novelua`.

Ubah file [CMakeLists.txt](file:///data/data/com.termux/files/home/novelua/CMakeLists.txt):

```cmake
# 1. Tambahkan Include Directories untuk Lexbor
include_directories(
    src/main/cpp/luau/VM/include
    src/main/cpp/luau/Compiler/include
    src/main/cpp/lexbor/source            # Path ke header Lexbor
    src/main/cpp                          # Path ke novelua_lexbor.h
)

# 3. Cari dan kumpulkan semua file sumber (.c dan .cpp) Lexbor
file(GLOB_RECURSE LEXBOR_SOURCES
    src/main/cpp/lexbor/source/*.c
)

# 4. Masukkan ke dalam target library novelua
add_library(novelua SHARED
    src/main/cpp/novelua_jni.cpp
    src/main/cpp/luaujava.cpp
    src/main/cpp/luaujavac.cpp
    src/main/cpp/novelua_lexbor.cpp      # File binding C++
    ${LEXBOR_SOURCES}                     # File library Lexbor
)

# 5. Hubungkan dengan target Luau.VM
target_link_libraries(novelua PRIVATE Luau.VM)
```

---

## Langkah 3: Menulis Binding C++ Luau ↔ Lexbor

Di dalam `novelua_lexbor.cpp`, buatlah fungsi yang mem-parsing dokumen HTML dan mengembalikannya sebagai tabel objek atau userdata ke Luau.

Contoh sederhana binding parsing HTML:
```cpp
#include "novelua_lexbor.h"
#include <lexbor/html/parser.h>
#include <lexbor/dom/interfaces/element.h>

// Fungsi pembantu untuk mem-parsing string HTML
static int l_html_parse(lua_State* L) {
    size_t len;
    const char* html_str = luaL_checklstring(L, 1, &len);

    // Inisialisasi parser Lexbor
    lxb_html_document_t* document = lxb_html_document_create();
    if (document == nullptr) {
        luaL_error(L, "Gagal mengalokasikan dokumen Lexbor");
    }

    lxb_status_t status = lxb_html_document_parse(document, (const lxb_char_t*)html_str, len);
    if (status != LXB_STATUS_OK) {
        lxb_html_document_destroy(document);
        luaL_error(L, "Gagal mem-parsing dokumen HTML");
    }

    // Bungkus document sebagai userdata native agar siklus hidupnya bisa diatur GC Luau
    // (Atau buat representasi tabel/userdata khusus untuk Node DOM)
    lxb_html_document_t** udata = (lxb_html_document_t**)lua_newuserdata(L, sizeof(lxb_html_document_t*));
    *udata = document;

    // TODO: Pasang metatable dengan dtor dan method select/find
    return 1;
}

// Daftarkan fungsi ke dalam modul global "html"
static const luaL_Reg html_funcs[] = {
    {"parse", l_html_parse},
    {NULL, NULL}
};

int luaopen_lexbor(lua_State* L) {
    // Daftarkan modul dengan nama global "html"
    luaL_register(L, "html", html_funcs);
    return 1;
}
```

---

## Langkah 4: Registrasi Library secara Global di JNI Bridge

Buka file [novelua_jni.cpp](file:///data/data/com.termux/files/home/novelua/src/main/cpp/novelua_jni.cpp). Anda harus memanggil `luaopen_lexbor` saat VM standar library diinisialisasi.

1. **Sertakan Header**:
   ```cpp
   #include "novelua_lexbor.h"
   ```
2. **Panggil Fungsi pada `nativeOpenLibs`**:
   Modifikasi fungsi JNI `Java_io_github_novela_luau_LuaState_nativeOpenLibs`:
   ```cpp
   extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeOpenLibs(JNIEnv*, jclass, jlong statePtr) {
       lua_State* L = reinterpret_cast<lua_State*>(statePtr);
       luaL_openlibs(L);       // Buka library bawaan Luau (math, string, table, dll)
       luaopen_lexbor(L);      // Buka dan daftarkan modul global "html" (Lexbor)
   }
   ```

---

## Langkah 5: Pengujian di Skrip Luau

Setelah Anda mengompilasi ulang library (`./gradlew test`), modul global `html` akan tersedia secara langsung di skrip Luau tanpa perlu di-require manual.

Contoh penggunaan di Luau:
```lua
-- Memanggil parser Lexbor global
local doc = html.parse("<html><body><h1>Halo NoveLua!</h1></body></html>")

-- Hasil berupa dokumen ter-parse yang dapat diproses lebih lanjut
print(doc)
```

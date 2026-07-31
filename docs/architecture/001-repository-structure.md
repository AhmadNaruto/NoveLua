# Repository Structure

```
root/

docs/

modules/

native/

thirdparty/

scripts/

cmake/

gradle/
```

---

# Thirdparty

Semua dependency wajib berupa Git Submodule.

```
thirdparty/

luau/

quickjs/

pcre2/

lexbor/
```

Tidak diperbolehkan melakukan modifikasi source library kecuali benar-benar diperlukan.

---

# Modules

```
modules/

luau/

regex/

quickjs/

lexsoup/
```

Setiap module menghasilkan Android Library.

---

# Native

```
native/

common/

luau/

regex/

quickjs/

lexsoup/
```

Tidak boleh ada dependency silang antar native module.

---

# Common

Folder common hanya berisi utility JNI yang benar-benar reusable.

Contoh:

- JString
- ScopedLocalRef
- ThrowException
- UTF8 Conversion

Tidak boleh berisi business logic.

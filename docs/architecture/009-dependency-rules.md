# Dependency Rules

## Dependency Flow

```
Application

↓

Kotlin API

↓

JNI

↓

Native Wrapper

↓

Thirdparty
```

---

## Independence

Setiap module harus independen.

Module tidak boleh bergantung pada module native lain.

Contoh yang TIDAK diperbolehkan:

LexSoup

↓

QuickJS

atau

QuickJS

↓

Regex

Integrasi dilakukan pada level Kotlin atau melalui Luau sebagai host.

---

## Shared Code

Kode bersama hanya boleh ditempatkan pada:

```
native/common
```

Isi common hanya utility.

Tidak boleh berisi business logic.

---

## Thirdparty

Semua dependency berada di:

```
thirdparty/
```

Semua dependency wajib berupa Git Submodule.

Tidak diperbolehkan menyalin source library ke dalam repository.

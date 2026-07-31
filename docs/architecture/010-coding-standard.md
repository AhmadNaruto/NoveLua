# Coding Standard Specification

Version: 1.0

---

# Purpose

Dokumen ini mendefinisikan standar penulisan kode untuk seluruh proyek Novelua Native SDK.

Semua implementasi yang dibuat oleh AI Agent maupun kontributor wajib mengikuti dokumen ini.

Dokumen ini berlaku untuk:

- Kotlin
- C++
- C
- JNI
- CMake
- Gradle

Apabila terjadi konflik antara implementasi dan dokumen ini, maka implementasi harus diubah.

---

# General Principles

Kode harus:

- sederhana
- eksplisit
- mudah dibaca
- mudah diuji
- mudah dipelihara
- memiliki tanggung jawab yang jelas

Hindari abstraksi yang tidak diperlukan.

Lebih baik kode sedikit lebih panjang tetapi jelas.

---

# General Rules

- Jangan membuat framework baru.
- Jangan membuat helper yang terlalu generik.
- Jangan menggunakan macro jika fungsi biasa sudah cukup.
- Jangan menggunakan template C++ secara berlebihan.
- Hindari inheritance jika composition sudah cukup.
- Hindari singleton.
- Hindari global mutable state.
- Hindari hidden behavior.

---

# Naming Convention

Gunakan bahasa Inggris.

Nama harus deskriptif.

Contoh:

Parser

Document

Element

Regex

Runtime

Context

VM

---

Hindari:

Mgr

Obj

Tmp

Data2

Value1

test123

---

# Directory Structure

```

module/

src/main/kotlin/

src/main/cpp/

src/test/

```

Setiap module memiliki struktur yang sama.

---

# File Naming

Kotlin

Gunakan PascalCase.

Contoh:

```

Document.kt

Element.kt

Regex.kt

```

---

C++

Gunakan snake_case.

```

document.cpp

document.hpp

regex.cpp

regex.hpp

```

---

JNI

Gunakan suffix "_jni".

```

document_jni.cpp

regex_jni.cpp

vm_jni.cpp

```

---

# Kotlin Style

Gunakan Kotlin idiom.

Gunakan:

```
class Document

object Regex

sealed class

enum class

data class
```

---

Hindari Java style.

Contoh yang tidak diperbolehkan:

```
public class Document {

}
```

---

Property lebih diprioritaskan daripada getter.

Gunakan:

```
document.title
```

Bukan:

```
document.getTitle()
```

kecuali memang diperlukan.

---

Gunakan extension function jika sesuai.

---

# Kotlin Nullability

Gunakan nullable hanya jika benar-benar diperlukan.

Lebih baik:

```
fun parent(): Element?
```

daripada

```
fun parent(): Element
```

yang melempar exception.

---

# Resource Management

Semua object native yang memiliki resource wajib:

```
Closeable
```

atau

```
AutoCloseable
```

---

Gunakan:

```
use {

}
```

bila memungkinkan.

---

# Visibility

Gunakan visibility sekecil mungkin.

Prioritas:

private

↓

internal

↓

public

---

# Exception

Gunakan exception Kotlin.

Contoh:

IllegalArgumentException

IllegalStateException

RuntimeException

IOException

---

Jangan mengembalikan null untuk kondisi error.

---

# C++ Standard

Gunakan:

C++20

---

Gunakan:

```
std::unique_ptr

std::shared_ptr

std::optional

std::variant

std::string_view

std::span
```

jika sesuai.

---

Hindari:

malloc

free

new

delete

secara langsung.

---

Gunakan RAII.

---

# Namespace

Seluruh wrapper berada pada namespace:

```
novelua
```

Contoh:

```
novelua::luau

novelua::regex

novelua::quickjs

novelua::lexsoup
```

---

# Header Rules

Gunakan:

```
#pragma once
```

Jangan menggunakan include guard.

---

Minimal include.

Gunakan forward declaration bila memungkinkan.

---

# Include Order

1. Header sendiri

2. Project header

3. Thirdparty

4. STL

---

Contoh:

```
#include "document.hpp"

#include "node.hpp"

#include <lexbor/dom/interfaces/document.h>

#include <memory>
```

---

# Function

Function harus pendek.

Ideal:

20-40 baris.

Jika lebih dari 80 baris, pertimbangkan refactor.

---

# Class

Satu class memiliki satu tanggung jawab.

Jangan membuat God Object.

---

# Memory

Tidak boleh memory leak.

Tidak boleh dangling pointer.

Tidak boleh double free.

Semua ownership harus jelas.

---

# Error Handling

Gunakan exception internal.

JNI menerjemahkan exception menjadi exception Kotlin.

Jangan mengembalikan invalid pointer.

---

# Logging

Native:

Gunakan logging internal hanya untuk debug.

Release build tidak boleh menghasilkan log berlebihan.

---

# JNI Standard

Semua JNI menggunakan RegisterNatives.

Tidak menggunakan:

```
Java_xxx_xxx()
```

---

JNI hanya bertugas:

- convert argument

- convert return value

- call native wrapper

Tidak boleh memiliki business logic.

---

JNIEnv tidak boleh disimpan.

JavaVM boleh disimpan.

---

Semua jclass dan jmethodID wajib dicache.

---

# Handle

Semua native object menggunakan:

```
Long
```

di Kotlin.

Pointer native tidak boleh diekspos.

---

# C API

Thirdparty C API tidak boleh dipanggil langsung dari Kotlin.

Harus melalui Native Wrapper.

---

# CMake

Gunakan target modern.

Contoh:

```
target_link_libraries()

target_include_directories()

target_compile_features()
```

---

Hindari:

```
include_directories()

link_directories()
```

---

# Gradle

Gunakan Kotlin DSL.

Gunakan Version Catalog.

Hindari hardcode dependency.

---

# Testing

Setiap public API wajib memiliki unit test.

Semua bug yang diperbaiki harus memiliki regression test.

---

# Documentation

Seluruh public API wajib memiliki KDoc.

JNI internal tidak memerlukan dokumentasi panjang.

---

# AI Agent Rules

AI Agent wajib:

- mengikuti seluruh Architecture Specification

- mengikuti Coding Standard Specification

- tidak membuat abstraksi baru tanpa alasan kuat

- tidak mengubah struktur repository

- tidak mengubah package

- tidak membuat API publik tanpa spesifikasi

- tidak menambahkan dependency baru tanpa persetujuan

- tidak menggunakan library tambahan jika fitur dapat dibuat menggunakan STL atau library yang sudah ada

- menghasilkan kode yang konsisten dengan seluruh project

---

# Definition of Done

Sebuah implementasi dianggap selesai apabila:

- mengikuti Architecture Specification

- mengikuti Coding Standard

- lolos build

- lolos seluruh unit test

- tidak menghasilkan warning compiler

- tidak menghasilkan memory leak

- seluruh public API terdokumentasi

- lolos code review

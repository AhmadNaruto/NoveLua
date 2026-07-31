# Architecture Overview

## Project

**Novelua Native SDK**

---

# Vision

Novelua Native SDK menyediakan wrapper JNI yang ringan, aman, dan idiomatik untuk beberapa library native sehingga dapat digunakan melalui Kotlin pada Android.

Project ini berfokus pada performa tinggi, API yang bersih, dan kemudahan pemeliharaan.

---

# Goals

- Native-first architecture
- Kotlin-friendly API
- Minimal JNI overhead
- Stable public API
- High performance
- Memory safe
- Easy maintenance
- Modular design
- Independent modules

---

# Non Goals

Project ini TIDAK bertujuan untuk:

- Membuat universal runtime
- Membuat generic binding framework
- Membuat object registry global
- Menggunakan Reflection
- Menggunakan Annotation Processing
- Menggunakan runtime code generation
- Menggunakan automatic binding generator

Semua wrapper dibuat secara manual agar kualitas API tetap tinggi.

---

# Supported Platform

Android

---

# Supported ABI

- arm64-v8a

---

# Language

Kotlin

C17

C++20

JNI

---

# Native Libraries

- Luau
- PCRE2
- QuickJS
- Lexbor

---

# Design Principles

- Simplicity
- Explicit API
- Small JNI Layer
- Manual Wrapper
- Predictable Memory
- Zero Hidden Magic
- Easy Debugging

---

# High Level Architecture

Application

↓

Kotlin API

↓

JNI

↓

Native Wrapper

↓

Thirdparty Library

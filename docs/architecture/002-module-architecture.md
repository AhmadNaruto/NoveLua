# Module Architecture

Setiap module memiliki struktur yang identik.

```
Kotlin API

↓

JNI

↓

Native Wrapper

↓

Thirdparty Library
```

---

# Responsibilities

## Kotlin API

- Public API
- Kotlin Friendly
- Null Safety
- Resource Management

---

## JNI

JNI hanya bertugas:

- Convert parameter
- Convert return value
- Lookup native handle
- Call native wrapper

JNI tidak boleh memiliki business logic.

---

## Native Wrapper

Native Wrapper berisi seluruh implementasi.

Native Wrapper bertugas:

- Memory management
- Error translation
- Thirdparty adaptation
- Object conversion

---

## Thirdparty Library

Source library tidak boleh dipanggil langsung oleh Kotlin.

Semua akses harus melalui Native Wrapper.

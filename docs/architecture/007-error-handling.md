# Error Handling

Semua error native diterjemahkan menjadi exception Kotlin.

```
Native Error

↓

JNI

↓

RuntimeException

↓

Kotlin
```

---

## Rules

Tidak boleh:

- return nullptr
- return invalid pointer
- ignore native error

Harus:

- Throw RuntimeException
- Throw IllegalArgumentException
- Throw IllegalStateException

sesuai kondisi.

---

Setiap module wajib memiliki error adapter sendiri.

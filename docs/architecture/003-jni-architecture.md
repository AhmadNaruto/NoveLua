# JNI Architecture

## Rules

Semua native method menggunakan RegisterNatives.

Tidak menggunakan Java_com_xxx.

---

## Handle

Semua native object disimpan sebagai Long.

Contoh:

```
private val nativeHandle: Long
```

Pointer native tidak boleh diekspos.

---

## JNI Layer

JNI hanya boleh melakukan:

- Argument conversion
- Native call
- Return conversion

Tidak boleh ada business logic.

---

## Reference Rules

Gunakan LocalRef sesingkat mungkin.

GlobalRef hanya jika diperlukan.

WeakGlobalRef hanya untuk callback.

---

## Exception

Exception native harus diterjemahkan menjadi RuntimeException Kotlin.

JNI tidak boleh mengembalikan nullptr untuk kondisi error.

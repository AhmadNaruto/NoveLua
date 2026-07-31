# Memory Management

Semua native object menggunakan RAII.

Tidak diperbolehkan malloc/free tersebar.

---

## Rules

- Gunakan constructor
- Gunakan destructor
- Hindari raw ownership

---

## JNI

JNI tidak menyimpan pointer.

JNI hanya menyimpan opaque handle.

---

## String

Gunakan UTF-8.

Hindari copy string.

---

## Array

Hindari copy ByteArray.

Gunakan direct access jika memungkinkan.

---

## Resource

Semua resource wajib memiliki dispose()/close() pada Kotlin bila diperlukan.

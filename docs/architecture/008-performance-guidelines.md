# Performance Guidelines

## Rules

- Hindari copy String
- Hindari copy ByteArray
- Cache jclass
- Cache jmethodID
- Cache jfieldID
- Gunakan move semantics
- Gunakan string_view bila memungkinkan
- Hindari allocation pada hot path

---

## JNI

Kurangi crossing JNI.

Lebih baik satu native call besar daripada ribuan native call kecil.

---

## Native

Gunakan reserve() untuk vector bila ukuran sudah diketahui.

Hindari std::endl.

Gunakan '\n'.

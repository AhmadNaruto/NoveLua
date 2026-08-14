# Dokumentasi API NoveLua

NoveLua adalah pustaka pengikatan (binding) minimal, berkinerja tinggi, dan aman untuk Android ART yang menghubungkan Kotlin/Java dengan Luau VM menggunakan JNI (Java Native Interface).

Pustaka ini memporting fungsionalitas utama dari `hollow-cube/luau-java` dengan menggantikan arsitektur FFM (Foreign Function & Memory) ke JNI agar dapat berjalan secara native di runtime Android (ART).

---

## 1. Arsitektur & Aturan Memori JNI

Integrasi JNI dalam NoveLua dirancang dengan memperhatikan aspek keamanan memori dan pencegahan kebocoran (leak prevention):

* **Manajemen JNI Global References**: Callback dari Kotlin (`LuaFunction` dan `LuaYieldableFunction`) dibungkus dalam blok userdata native menggunakan `lua_newuserdatadtor`. Ketika Luau VM melakukan Garbage Collection (GC) pada objek userdata tersebut, destructor native (`udata_dtor_custom`) otomatis melepaskan referensi global JVM (`DeleteGlobalRef`).
* **Exception Boundary (JVM ↔ Luau)**: Luau VM menggunakan C `longjmp` untuk penanganan kesalahan runtime. Karena meloncat langsung melewati frame JNI/JVM dapat menyebabkan crash fatal, NoveLua menangkap setiap exception JVM di dalam trampoline JNI, mengosongkan status exception di JVM (`ExceptionClear`), menyimpan throwable tersebut secara thread-local (`g_pendingException`), dan melempar status error `-103` ke Luau agar stack di Luau dibersihkan terlebih dahulu. Setelah kontrol kembali ke Kotlin, exception JVM yang disimpan tadi akan dilempar kembali secara bersih.
* **Traceback Rekonstruksi**: NoveLua menyisipkan custom error handler C++ di dalam pemanggilan `lua_pcall`. Handler ini menggunakan `luaL_traceback` untuk merekam jejak panggilan Luau sebelum stack dilepas, kemudian parser Kotlin mengubahnya menjadi array `StackTraceElement` dengan class name `"lua"`.

---

## 2. Model Data (`LuaValue`)

Semua data yang dikomunikasikan dari dan ke Luau direpresentasikan melalui model data tersegel (sealed class) `LuaValue`.

```kotlin
sealed class LuaValue {
    object Nil : LuaValue()
    data class Boolean(val value: kotlin.Boolean) : LuaValue()
    data class Number(val value: Double) : LuaValue()
    data class Integer(val value: Long) : LuaValue()
    data class String(val value: kotlin.String) : LuaValue()
    class Table(val table: LuaTable) : LuaValue()
    class Function(val state: LuaState, val reference: LuaReference) : LuaValue()
    class Thread(val thread: LuaThread) : LuaValue()
    data class Userdata(val value: Any, val tag: Int) : LuaValue()
    class Buffer(val value: java.nio.ByteBuffer) : LuaValue()
    data class Vector(val x: Float, val y: Float, val z: Float) : LuaValue()
}
```

---

## 3. Komponen Utama

### A. `LuaEngine`
Mengelola siklus hidup VM utama, kompilasi kode sumber Luau ke bytecode, dan eksekusi skrip.

#### Konstruktor & Metode Utama:
* **`LuaEngine()`**: Membuat instansi Luau VM baru dan memuat pustaka standar Luau (`base`, `string`, `table`, `math`, `utf8`, `buffer`, `coroutine`).
* **`val mainState: LuaState`**: Mendapatkan instance `LuaState` utama dari engine.
* **`fun compile(source: String): ByteArray`**: Mengompilasi kode sumber Lua string ke bytecode biner Luau. Melempar `LuaException` jika terjadi kesalahan sintaksis.
* **`fun execute(source: String, chunkName: String = "script")`**: Mengompilasi dan langsung mengeksekusi skrip pada `mainState`.
* **`fun close()`**: Menutup engine dan menghancurkan semua resource native state. (`LuaEngine` mengimplementasikan `AutoCloseable`).

---

### B. `LuaState`
Menyediakan API tingkat rendah untuk manipulasi stack Luau, pemanggilan fungsi, manajemen coroutine, dan interaksi dengan tabel.

#### Konstanta:
* **`LuaState.REGISTRY_INDEX = -10000`**: Pseudo-index untuk tabel registri global internal VM.
* **`LuaState.GLOBALS_INDEX = -10002`**: Pseudo-index untuk tabel variabel lingkungan global.

#### Navigasi & Manipulasi Stack:
* **`fun getTop(): Int`**: Mengembalian index elemen teratas pada stack.
* **`fun setTop(idx: Int)`**: Mengatur posisi elemen teratas stack.
* **`fun pop(n: Int)`**: Mengeluarkan `n` buah elemen dari atas stack.
* **`fun pushNil()`** / **`fun pushBoolean(b: Boolean)`** / **`fun pushNumber(n: Double)`** / **`fun pushInteger(i: Long)`** / **`fun pushString(s: String)`** / **`fun pushVector(x: Float, y: Float, z: Float)`**
* **`fun toBoolean(idx: Int): Boolean`** / **`fun toNumber(idx: Int): Double`** / **`fun toInteger(idx: Int): Long`** / **`fun toString(idx: Int): String?`** / **`fun toValue(idx: Int): LuaValue`**
* **`fun type(idx: Int): LuaType`**: Mengembalikan tipe data Luau dari elemen pada stack berdasarkan enum `LuaType`.

#### Pemanggilan & Eksekusi:
* **`fun load(chunkName: String, bytecode: ByteArray)`**: Memuat bytecode ke state untuk siap dijalankan.
* **`fun call(nargs: Int, nresults: Int)`**: Memanggil fungsi teratas pada stack dengan parameter sebanyak `nargs` dan jumlah hasil kembali `nresults`. Jika terjadi kegagalan skrip, akan melempar `LuaException` lengkap dengan traceback.
* **`fun yield(nresults: Int): Int`**: Menangguhkan (yield) eksekusi thread aktif dengan mengembalikan hasil sebanyak `nresults`.

#### Pembuatan Objek Data:
* **`fun newTable()`**: Membuat tabel kosong baru dan menambahkannya ke stack.
* **`fun newThread(L: LuaState): LuaState`**: Membuat coroutine thread baru di dalam state `L`.
* **`fun newUserData(obj: Any)`**: Membungkus objek Kotlin sebagai userdata pada stack dengan siklus hidup yang diatur oleh GC Luau.

---

### C. `LuaTable`
Pembungkus (wrapper) aman untuk mempermudah manipulasi tabel Luau secara terstruktur.

* **`fun get(key: Any?): LuaValue`**: Mengambil nilai dari tabel berdasarkan kunci (bisa berupa string, angka, atau objek).
* **`fun set(key: Any?, value: Any?)`**: Menetapkan pasangan kunci dan nilai ke dalam tabel.
* **`fun getString(key: Any?): String?`** / **`fun getDouble(key: Any?): Double`** / **`fun getTable(key: Any?): LuaTable?`**
* **`fun close()`**: Melepaskan referensi registri internal untuk tabel tersebut.

---

### D. `LuaReference`
Mengamankan nilai stack agar tidak terhapus oleh siklus pembersihan stack dengan menyimpannya di tabel registri internal VM. Dibuat menggunakan `state.popReference()`. Harus ditutup menggunakan `close()` untuk mencegah kebocoran referensi registri.

---

## 4. Sistem Callback (Kotlin ke Lua)

Anda dapat mengekspos fungsi Kotlin ke dalam lingkungan runtime Luau menggunakan fungsionalitas callback.

### Fungsi Sinkron (`LuaFunction`)
Digunakan untuk callback standar yang langsung mengembalikan nilai.

```kotlin
val simpleAdd = LuaFunction { state ->
    val a = state.toNumber(1)
    val b = state.toNumber(2)
    state.pushNumber(a + b)
    1 // Mengembalikan 1 nilai
}
```

### Fungsi Penangguhan (`LuaYieldableFunction`)
Digunakan untuk fungsi asinkron yang mendukung mekanisme coroutine yield dan resume.

```kotlin
val asyncTask = object : LuaYieldableFunction {
    override fun invoke(state: LuaState): Int {
        // Melakukan penangguhan eksekusi di Luau
        state.pushString("Menunggu hasil...")
        return state.yield(1)
    }

    override fun resume(state: LuaState, status: Int): Int {
        // Melanjutkan eksekusi setelah resume dipanggil
        state.pushString("Tugas Asinkron Selesai!")
        return 1
    }
}
```

---

## 5. Contoh Kode Integrasi

### A. Eksekusi Kode Sederhana & Pembacaan Variabel Global
```kotlin
LuaEngine().use { engine ->
    val L = engine.mainState
    
    // Eksekusi kalkulasi di Luau
    engine.execute("resultVal = (10 + 5) * 2")
    
    // Ambil hasil dari tabel global environment
    L.getField(LuaState.GLOBALS_INDEX, "resultVal")
    val valNum = L.toValue(-1)
    
    if (valNum is LuaValue.Number) {
        println("Hasil dari Luau: ${valNum.value}") // Output: 30.0
    }
    L.pop(1)
}
```

### B. Registrasi Callback Kotlin & Pemanggilan di Lua
```kotlin
LuaEngine().use { engine ->
    val L = engine.mainState

    // Definisikan fungsi logging Kotlin
    val logFunc = LuaFunction { state ->
        val logText = state.toString(1) ?: ""
        println("[KOTLIN LOG]: $logText")
        0 // Tidak mengembalikan hasil ke Lua
    }

    // Daftarkan fungsi ke global environment Lua dengan nama "klog"
    L.pushFunction(logFunc, "klog")
    L.setField(LuaState.GLOBALS_INDEX, "klog")

    // Jalankan kode Lua yang memanggil fungsi klog
    engine.execute("klog('Halo dari lingkungan Luau VM!')")
}
```

### C. Penanganan Error & Print Traceback
```kotlin
LuaEngine().use { engine ->
    try {
        engine.execute("""
            function level2()
                error("Ups, kesalahan fatal terjadi!")
            end
            
            function level1()
                level2()
            end
            
            level1()
        """)
    } catch (e: LuaException) {
        println("Pesan Error: ${e.message}")
        println("Traceback Luau:")
        e.stackTrace.forEach { frame ->
            if (frame.className == "lua") {
                println("  di file: ${frame.fileName}, fungsi: ${frame.methodName}, baris: ${frame.lineNumber}")
            }
        }
    }
}
```

### D. Sandboxing untuk Keamanan Eksekusi
Membatasi akses pustaka standar untuk lingkungan thread tertentu (coroutine sandbox).

```kotlin
LuaEngine().use { engine ->
    val L = engine.mainState

    // Membuat thread (coroutine) baru
    val sandboxThread = LuaState.newThread(L)
    
    // Terapkan sandbox pada thread tersebut
    // Hal ini mengisolasi pustaka sensitif (seperti penulisan data/buffer) agar tidak diakses
    L.sandbox(sandboxThread)

    // Eksekusi kode secara aman di dalam sandboxThread
    sandboxThread.getField(LuaState.GLOBALS_INDEX, "print")
    if (!sandboxThread.isNil(-1)) {
        println("Fungsi 'print' aman diakses dalam sandbox")
    }
    sandboxThread.pop(1)
    
    sandboxThread.close()
}
```

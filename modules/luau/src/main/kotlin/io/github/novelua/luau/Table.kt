package io.github.novelua.luau

/**
 * Represents a Lua table.
 */
class Table internal constructor(internal var nativeHandle: Long) {
    val size: Int
        get() = if (nativeHandle != 0L) nativeSize(nativeHandle) else 0

    fun get(key: Any): Any? = if (nativeHandle != 0L) nativeGet(nativeHandle, key) else null

    fun set(key: Any, value: Any?) {
        if (nativeHandle != 0L) nativeSet(nativeHandle, key, value)
    }

    fun remove(key: Any) {
        if (nativeHandle != 0L) nativeRemove(nativeHandle, key)
    }

    fun clear() {
        if (nativeHandle != 0L) nativeClear(nativeHandle)
    }

    fun contains(key: Any): Boolean = if (nativeHandle != 0L) nativeContains(nativeHandle, key) else false

    fun keys(): List<Any> = if (nativeHandle != 0L) nativeKeys(nativeHandle) else emptyList()

    fun values(): List<Any> = if (nativeHandle != 0L) nativeValues(nativeHandle) else emptyList()

    @Suppress("deprecation")
    protected fun finalize() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    private external fun nativeSize(handle: Long): Int
    private external fun nativeGet(handle: Long, key: Any): Any?
    private external fun nativeSet(handle: Long, key: Any, value: Any?)
    private external fun nativeRemove(handle: Long, key: Any)
    private external fun nativeClear(handle: Long)
    private external fun nativeContains(handle: Long, key: Any): Boolean
    private external fun nativeKeys(handle: Long): List<Any>
    private external fun nativeValues(handle: Long): List<Any>
    private external fun nativeDestroy(handle: Long)

    companion object {
        init {
            try {
                System.loadLibrary("novelua_luau")
            } catch (_: Throwable) {
            }
        }
    }
}

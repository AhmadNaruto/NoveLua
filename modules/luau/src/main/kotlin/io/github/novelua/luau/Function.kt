package io.github.novelua.luau

/**
 * Represents a Lua function.
 */
class Function internal constructor(internal var nativeHandle: Long) {

    fun call(vararg args: Any?): Any? = if (nativeHandle != 0L) nativeCall(nativeHandle, args) else null

    fun invoke(vararg args: Any?): Any? = call(*args)

    @Suppress("deprecation")
    protected fun finalize() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    private external fun nativeCall(handle: Long, args: Array<out Any?>): Any?
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

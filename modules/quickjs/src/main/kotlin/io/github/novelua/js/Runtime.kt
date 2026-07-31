package io.github.novelua.js

/**
 * Represents Runtime.
 */
class Runtime : AutoCloseable {
    internal val nativeHandle: Long = QuickJSNative.createRuntime()

    /**
     * Executes createContext.
     */
    fun createContext(): Context {
        return Context(this)
    }

    /**
     * Executes gc.
     */
    fun gc() {
        QuickJSNative.gc(nativeHandle)
    }

    override fun close() {
        if (nativeHandle != 0L) {
            QuickJSNative.destroyRuntime(nativeHandle)
        }
    }
}

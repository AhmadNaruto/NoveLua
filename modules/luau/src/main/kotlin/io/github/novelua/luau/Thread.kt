package io.github.novelua.luau

import java.io.Closeable

/**
 * Represents a Luau coroutine thread.
 */
class Thread : Closeable, AutoCloseable {
    private var nativeHandle: Long = 0

    /**
     * Property status.
     */
    val status: String = "suspended"

    /**
     * Executes resume.
     */
    fun resume(vararg args: Any?) {}
    /**
     * Executes yield.
     */
    fun yield() {}

    override fun close() {
        if (nativeHandle != 0L) {
            nativeHandle = 0L
        }
    }
}

package io.github.novelua.luau

import java.io.Closeable

/**
 * Represents a Luau coroutine thread.
 */
class Thread internal constructor(internal var nativeHandle: Long) : Closeable, AutoCloseable {
    constructor() : this(0L)

    /**
     * Property status.
     */
    val status: String = "suspended"

    /**
     * Executes resume.
     */
    @Suppress("UNUSED_PARAMETER")
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

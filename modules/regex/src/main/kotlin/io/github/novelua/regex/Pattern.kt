package io.github.novelua.regex

/**
 * A compiled regular expression pattern using PCRE2.
 */
class Pattern internal constructor(
    private val pattern: String,
    private val options: Int
) : AutoCloseable {

    private var nativeHandle: Long = 0

    init {
        nativeHandle = nativeCreate(pattern, options)
        if (nativeHandle == 0L) {
            throw IllegalStateException("Failed to create Pattern native handle.")
        }
    }

    /**
     * Creates a matcher that will match the given input against this pattern.
     */
    fun matcher(input: String): Matcher {
        checkClosed()
        return Matcher(this, input, nativeHandle)
    }

    internal fun internalReplaceFirst(input: String, replacement: String): String {
        checkClosed()
        return nativeReplaceFirst(nativeHandle, input, replacement)
    }

    internal fun internalReplaceAll(input: String, replacement: String): String {
        checkClosed()
        return nativeReplaceAll(nativeHandle, input, replacement)
    }

    private fun checkClosed() {
        if (nativeHandle == 0L) {
            throw IllegalStateException("Pattern has already been closed.")
        }
    }

    override fun close() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0
        }
    }

    private external fun nativeCreate(pattern: String, options: Int): Long
    private external fun nativeDestroy(handle: Long)
    private external fun nativeReplaceFirst(handle: Long, input: String, replacement: String): String
    private external fun nativeReplaceAll(handle: Long, input: String, replacement: String): String

    companion object {
        init {
            try {
                System.loadLibrary("novelua_regex")
            } catch (_: Throwable) {
            }
        }
    }
}

package io.github.novelua.luau

/**
 * Compiles Luau source code.
 */
class Compiler {
    fun compile(source: String): ByteArray {
        return nativeCompile(source)
    }

    private external fun nativeCompile(source: String): ByteArray

    companion object {
        init {
            try {
                System.loadLibrary("novelua_luau")
            } catch (_: Throwable) {
            }
        }
    }
}

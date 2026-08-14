package io.github.novela.luau

import java.nio.charset.StandardCharsets

class LuaEngine : AutoCloseable {
    val mainState: LuaState

    init {
        val statePtr = nativeNewState()
        if (statePtr == 0L) {
            throw IllegalStateException("Failed to create Lua state")
        }
        mainState = LuaState(statePtr, isMainThread = true)
        nativeOpenLibs(statePtr)
    }

    fun compile(source: String): ByteArray {
        return compile(source.toByteArray(StandardCharsets.UTF_8))
    }

    fun compile(source: ByteArray): ByteArray {
        val sizeOut = LongArray(1)
        val resultPtr = nativeCompile(source, source.size, sizeOut)
        if (resultPtr == 0L) {
            throw LuaException("Failed to compile script")
        }
        val size = sizeOut[0].toInt()
        val bytecode = nativeReadBytesAndFree(resultPtr, size)
        
        if (bytecode.isNotEmpty() && bytecode[0] == 0.toByte()) {
            val errMsg = String(bytecode, 1, bytecode.size - 1, StandardCharsets.UTF_8)
            throw LuaException("Compile error: $errMsg")
        }
        return bytecode
    }

    fun execute(source: String, chunkName: String = "script") {
        val bytecode = compile(source)
        mainState.load(chunkName, bytecode)
        mainState.call(0, 0)
    }

    override fun close() {
        mainState.close()
    }

    companion object {
        init {
            System.loadLibrary("novelua")
        }

        @JvmStatic private external fun nativeNewState(): Long
        @JvmStatic private external fun nativeOpenLibs(statePtr: Long)
        @JvmStatic private external fun nativeCompile(source: ByteArray, length: Int, sizeOut: LongArray): Long
        @JvmStatic private external fun nativeReadBytesAndFree(resultPtr: Long, size: Int): ByteArray
    }
}

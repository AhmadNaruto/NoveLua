package io.github.novelua.luau

import java.io.Closeable

/**
 * Represents Luau Virtual Machine.
 */
class VM : Closeable, AutoCloseable {
    private var nativeHandle: Long = 0

    /**
     * Property globals.
     */
    val globals: Table = Table()
    /**
     * Property registry.
     */
    val registry: Table = Table()

    /**
     * Property isClosed.
     */
    var isClosed: Boolean = false
        private set

    init {
        nativeHandle = nativeCreate()
    }

    /**
     * Executes execute.
     */
    fun execute(script: String) {
        eval(script)
    }

    /**
     * Executes execute.
     */
    fun execute(script: ByteArray) {}

    /**
     * Executes compile.
     */
    fun compile(script: String): ByteArray = ByteArray(0)

    /**
     * Executes load.
     */
    fun load(bytecode: ByteArray) {}

    /**
     * Executes eval.
     */
    fun eval(script: String): Any? {
        checkClosed()
        return nativeEval(nativeHandle, script)
    }

    /**
     * Executes evaluate.
     */
    fun evaluate(script: String): Any? = eval(script)

    /**
     * Executes gc.
     */
    fun gc() {}

    /**
     * Executes createTable.
     */
    fun createTable(): Table = Table()

    /**
     * Executes createThread.
     */
    fun createThread(): Thread = Thread()

    /**
     * Executes getGlobal.
     */
    fun getGlobal(name: String): Any? = null

    /**
     * Executes setGlobal.
     */
    fun setGlobal(name: String, value: Any?) {}

    /**
     * Executes registerLibrary.
     */
    fun registerLibrary(library: Library) {
        library.register(this)
    }

    /**
     * Executes registerModule.
     */
    fun registerModule(module: Module) {
        module.load(this)
    }

    private fun checkClosed() {
        if (isClosed || nativeHandle == 0L) {
            throw IllegalStateException("VM is closed")
        }
    }

    override fun close() {
        if (!isClosed) {
            isClosed = true
            if (nativeHandle != 0L) {
                nativeDestroy(nativeHandle)
                nativeHandle = 0L
            }
        }
    }

    private external fun nativeCreate(): Long
    private external fun nativeDestroy(handle: Long)
    private external fun nativeEval(handle: Long, script: String): String?

    companion object {
        init {
            try {
                System.loadLibrary("novelua_luau")
            } catch (_: Throwable) {
            }
        }
    }
}

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
    val globals: Table
    /**
     * Property registry.
     */
    val registry: Table

    /**
     * Property isClosed.
     */
    var isClosed: Boolean = false
        private set

    init {
        nativeHandle = nativeCreate()
        globals = Table(nativeGetGlobals(nativeHandle))
        registry = Table(nativeGetRegistry(nativeHandle))
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
    fun execute(script: ByteArray) {
        checkClosed()
        load(script)
        execute()
    }

    fun execute() {
        checkClosed()
        nativeExecute(nativeHandle)
    }

    /**
     * Executes compile.
     */
    fun compile(script: String): ByteArray = Compiler().compile(script)

    /**
     * Executes load.
     */
    fun load(bytecode: ByteArray) {
        checkClosed()
        nativeLoad(nativeHandle, bytecode)
    }

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
    fun gc() {
        System.gc()
    }

    /**
     * Executes createTable.
     */
    fun createTable(): Table {
        checkClosed()
        return Table(nativeCreateTable(nativeHandle))
    }

    /**
     * Executes createThread.
     */
    fun createThread(): Thread {
        checkClosed()
        return Thread()
    }

    /**
     * Executes getGlobal.
     */
    fun getGlobal(name: String): Any? {
        checkClosed()
        return nativeGetGlobal(nativeHandle, name)
    }

    /**
     * Executes setGlobal.
     */
    fun setGlobal(name: String, value: Any?) {
        checkClosed()
        nativeSetGlobal(nativeHandle, name, value)
    }

    fun registerCallback(name: String, callback: VMCallback) {
        checkClosed()
        nativeRegisterCallback(nativeHandle, name, callback)
    }

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
    private external fun nativeLoad(handle: Long, bytecode: ByteArray)
    private external fun nativeExecute(handle: Long)
    private external fun nativeCreateTable(handle: Long): Long
    private external fun nativeGetGlobal(handle: Long, name: String): Any?
    private external fun nativeSetGlobal(handle: Long, name: String, value: Any?)
    private external fun nativeGetGlobals(handle: Long): Long
    private external fun nativeGetRegistry(handle: Long): Long
    private external fun nativeRegisterCallback(handle: Long, name: String, callback: VMCallback)

    companion object {
        init {
            try {
                System.loadLibrary("novelua_luau")
            } catch (_: Throwable) {
            }
        }
    }
}

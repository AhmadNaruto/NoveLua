package io.github.novela.luau

class LuaReference(
    val state: LuaState,
    val nativeRef: Int
) : AutoCloseable {
    private var isClosed = false

    override fun close() {
        if (!isClosed) {
            state.unref(nativeRef)
            isClosed = true
        }
    }

    protected fun finalize() {
        close()
    }
}

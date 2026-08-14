package io.github.novela.luau

class LuaThread(
    val state: LuaState,
    val reference: LuaReference
) : AutoCloseable {
    override fun close() {
        reference.close()
    }

    override fun toString(): String {
        return "thread: 0x${java.lang.Long.toHexString(reference.nativeRef.toLong())}"
    }
}

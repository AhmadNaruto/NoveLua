package io.github.novela.luau

enum class LuaCoStatus {
    RUNNING,
    SUSPENDED,
    NORMAL,
    FINISHED,
    ERROR;

    fun id(): Int = ordinal

    companion object {
        private val VALUES = values()

        fun byId(id: Int): LuaCoStatus {
            return if (id >= 0 && id < VALUES.size) VALUES[id] else SUSPENDED
        }
    }
}

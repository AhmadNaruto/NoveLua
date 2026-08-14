package io.github.novela.luau

enum class LuaStatus {
    OK,
    YIELD,
    ERRRUN,
    UNUSED0,
    ERRMEM,
    ERRERR,
    BREAK;

    fun id(): Int = ordinal

    companion object {
        private val VALUES = values()

        fun byId(id: Int): LuaStatus {
            return if (id >= 0 && id < VALUES.size) VALUES[id] else ERRERR
        }
    }
}

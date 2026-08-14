package io.github.novela.luau

enum class LuaType {
    NONE,
    NIL,
    BOOLEAN,
    LIGHTUSERDATA,
    NUMBER,
    INTEGER,
    VECTOR,
    STRING,
    TABLE,
    FUNCTION,
    USERDATA,
    THREAD,
    BUFFER,
    CLASS,
    OBJECT;

    fun id(): Int = ordinal - 1

    fun typeName(): String = name.lowercase()

    companion object {
        private val VALUES = values()

        fun byId(id: Int): LuaType {
            return if (id >= -1 && id + 1 < VALUES.size) VALUES[id + 1] else NONE
        }
    }
}

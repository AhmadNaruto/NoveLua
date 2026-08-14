package io.github.novela.luau

enum class LuaGcOp {
    STOP,
    RESTART,
    COLLECT,
    COUNT,
    COUNTB,
    IS_RUNNING,
    STEP,
    SET_GOAL,
    SET_STEP_MUL,
    SET_STEP_SIZE,
    IS_PAUSED;

    fun id(): Int = ordinal

    companion object {
        private val VALUES = values()

        fun byId(id: Int): LuaGcOp {
            return if (id >= 0 && id < VALUES.size) VALUES[id] else COLLECT
        }
    }
}

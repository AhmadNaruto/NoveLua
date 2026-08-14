package io.github.novela.luau

fun interface LuaFunction {
    fun invoke(state: LuaState): Int
}

interface LuaYieldableFunction : LuaFunction {
    fun resume(state: LuaState, status: Int): Int
}

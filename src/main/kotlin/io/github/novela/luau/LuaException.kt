package io.github.novela.luau

class LuaException(
    message: String,
    val status: Int = -1
) : RuntimeException(message)

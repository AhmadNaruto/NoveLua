package io.github.novelua.luau

/**
 * Functional interface to receive callbacks from Luau VM in Kotlin.
 */
fun interface VMCallback {
    fun call(args: Array<String>): String
}

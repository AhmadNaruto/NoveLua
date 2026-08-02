package io.github.novelua.js

/**
 * Functional interface to receive callbacks from QuickJS Context in Kotlin.
 */
fun interface JSCallback {
    fun call(args: kotlin.Array<String>): String
}

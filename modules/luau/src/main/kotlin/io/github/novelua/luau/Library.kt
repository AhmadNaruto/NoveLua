package io.github.novelua.luau

/**
 * Represents a native library.
 */
abstract class Library(val name: String) {
    open fun register(vm: VM) {}
}

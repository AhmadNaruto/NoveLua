package io.github.novelua.luau

/**
 * Represents a Lua module.
 */
abstract class Module(val name: String) {
    open fun load(vm: VM) {}
}

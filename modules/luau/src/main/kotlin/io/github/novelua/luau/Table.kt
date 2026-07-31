package io.github.novelua.luau

/**
 * Represents a Lua table.
 */
class Table {
    private var nativeHandle: Long = 0

    /**
     * Property size.
     */
    val size: Int = 0

    /**
     * Executes get.
     */
    fun get(key: Any): Any? = null
    /**
     * Executes set.
     */
    fun set(key: Any, value: Any?) {}
    /**
     * Executes remove.
     */
    fun remove(key: Any) {}
    /**
     * Executes clear.
     */
    fun clear() {}
    /**
     * Executes contains.
     */
    fun contains(key: Any): Boolean = false
    /**
     * Executes keys.
     */
    fun keys(): List<Any> = emptyList()
    /**
     * Executes values.
     */
    fun values(): List<Any> = emptyList()
}

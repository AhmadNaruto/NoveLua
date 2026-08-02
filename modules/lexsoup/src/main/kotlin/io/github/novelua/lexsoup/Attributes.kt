package io.github.novelua.lexsoup

/**
 * Represents Attributes.
 */
class Attributes {
    private val map = mutableMapOf<String, String>()

    fun get(key: String): String = map[key] ?: ""

    fun put(key: String, value: String) {
        map[key] = value
    }

    fun remove(key: String) {
        map.remove(key)
    }

    fun hasKey(key: String): Boolean = map.containsKey(key)
}

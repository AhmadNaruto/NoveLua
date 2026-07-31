package io.github.novelua.lexsoup

class Elements(private val elements: List<Element> = emptyList()) : List<Element> by elements {
    /**
     * Executes firstOrNull.
     */
    fun firstOrNull(): Element? = elements.firstOrNull()
    /**
     * Executes first.
     */
    fun first(): Element? = firstOrNull()
    /**
     * Property text.
     */
    val text: String
        get() = elements.joinToString(" ") { it.text }
}

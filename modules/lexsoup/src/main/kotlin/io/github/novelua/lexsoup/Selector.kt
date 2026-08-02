package io.github.novelua.lexsoup

/**
 * Evaluates CSS selectors.
 */
object Selector {
    /**
     * Executes select.
     */
    fun select(root: Element, css: String): Elements = root.select(css)
}

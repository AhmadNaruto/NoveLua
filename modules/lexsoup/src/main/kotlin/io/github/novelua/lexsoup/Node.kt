package io.github.novelua.lexsoup
/**
 * Represents Node.
 */
class Node {
    /**
     * Executes nodeName.
     */
    fun nodeName(): String = ""
    /**
     * Executes outerHtml.
     */
    fun outerHtml(): String = ""
    /**
     * Executes parent.
     */
    fun parent(): Node? = null
    /**
     * Executes childNodes.
     */
    fun childNodes(): List<Node> = emptyList()
    /**
     * Executes remove.
     */
    fun remove() {}
}

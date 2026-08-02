package io.github.novelua.lexsoup

/**
 * Represents a DOM TextNode.
 */
class TextNode internal constructor(nativeHandle: Long) : Node(nativeHandle) {

    fun text(): String = if (nativeHandle != 0L) nativeText(nativeHandle) else ""

    private external fun nativeText(handle: Long): String
}

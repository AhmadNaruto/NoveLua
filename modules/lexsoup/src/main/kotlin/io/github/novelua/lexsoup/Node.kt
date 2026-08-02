package io.github.novelua.lexsoup

/**
 * Represents a DOM Node.
 */
open class Node internal constructor(internal var nativeHandle: Long) {

    fun nodeName(): String = if (nativeHandle != 0L) nativeNodeName(nativeHandle) else ""

    fun outerHtml(): String = if (nativeHandle != 0L) nativeOuterHtml(nativeHandle) else ""

    open fun parent(): Node? {
        val h = if (nativeHandle != 0L) nativeParent(nativeHandle) else 0L
        return if (h != 0L) wrapNode(h) else null
    }

    fun childNodes(): List<Node> {
        val handles = if (nativeHandle != 0L) nativeChildNodes(nativeHandle) else longArrayOf()
        return handles.map { wrapNode(it) }
    }

    fun remove() {
        if (nativeHandle != 0L) nativeRemove(nativeHandle)
    }

    @Suppress("deprecation")
    protected fun finalize() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    private external fun nativeNodeName(handle: Long): String
    private external fun nativeOuterHtml(handle: Long): String
    private external fun nativeParent(handle: Long): Long
    private external fun nativeChildNodes(handle: Long): LongArray
    private external fun nativeRemove(handle: Long)
    private external fun nativeDestroy(handle: Long)
    private external fun nativeNodeType(handle: Long): Int

    companion object {
        init {
            try {
                System.loadLibrary("novelua_lexsoup")
            } catch (_: Throwable) {
            }
        }

        fun wrapNode(handle: Long): Node {
            if (handle == 0L) return Node(0L)
            val dummy = Node(handle)
            val type = dummy.nativeNodeType(handle)
            return when (type) {
                1 -> Element(handle)
                3 -> TextNode(handle)
                else -> dummy
            }
        }
    }
}

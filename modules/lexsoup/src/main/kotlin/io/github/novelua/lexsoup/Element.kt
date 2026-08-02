package io.github.novelua.lexsoup

class Element internal constructor(nativeHandle: Long) : Node(nativeHandle) {
    constructor() : this(0L)

    /**
     * Property tagName.
     */
    val tagName: String
        get() = if (nativeHandle != 0L) nativeTagName(nativeHandle) else ""

    /**
     * Property id.
     */
    val id: String
        get() = if (nativeHandle != 0L) nativeId(nativeHandle) else ""

    /**
     * Property className.
     */
    val className: String
        get() = if (nativeHandle != 0L) nativeClassName(nativeHandle) else ""

    /**
     * Property text.
     */
    val text: String
        get() = if (nativeHandle != 0L) nativeText(nativeHandle) else ""

    /**
     * Property html.
     */
    val html: String
        get() = if (nativeHandle != 0L) nativeHtml(nativeHandle) else ""

    /**
     * Property outerHtml.
     */
    val outerHtml: String
        get() = outerHtml()

    /**
     * Executes attr.
     */
    fun attr(key: String): String = if (nativeHandle != 0L) nativeAttr(nativeHandle, key) else ""

    fun attr(key: String, value: String): Element {
        if (nativeHandle != 0L) nativeSetAttr(nativeHandle, key, value)
        return this
    }

    fun hasAttr(key: String): Boolean = if (nativeHandle != 0L) nativeHasAttr(nativeHandle, key) else false

    fun removeAttr(key: String) {
        if (nativeHandle != 0L) nativeRemoveAttr(nativeHandle, key)
    }

    fun empty() {
        if (nativeHandle != 0L) nativeEmpty(nativeHandle)
    }

    fun children(): Elements {
        if (nativeHandle == 0L) return Elements()
        val handles = nativeChildren(nativeHandle)
        return Elements(handles.map { Element(it) })
    }

    fun child(index: Int): Element {
        val h = if (nativeHandle != 0L) nativeChild(nativeHandle, index) else 0L
        return if (h != 0L) Element(h) else Element(0L)
    }

    override fun parent(): Element? {
        val h = if (nativeHandle != 0L) nativeParentElement(nativeHandle) else 0L
        return if (h != 0L) Element(h) else null
    }

    fun next(): Element? {
        val h = if (nativeHandle != 0L) nativeNext(nativeHandle) else 0L
        return if (h != 0L) Element(h) else null
    }

    fun previous(): Element? {
        val h = if (nativeHandle != 0L) nativePrevious(nativeHandle) else 0L
        return if (h != 0L) Element(h) else null
    }

    /**
     * Executes select.
     */
    fun select(css: String): Elements {
        if (nativeHandle == 0L) return Elements()
        /**
         * Property handles.
         */
        val handles = nativeSelect(nativeHandle, css)
        return Elements(handles.map { Element(it) })
    }

    /**
     * Executes selectFirst.
     */
    fun selectFirst(css: String): Element? = select(css).firstOrNull()

    private external fun nativeTagName(handle: Long): String
    private external fun nativeId(handle: Long): String
    private external fun nativeClassName(handle: Long): String
    private external fun nativeText(handle: Long): String
    private external fun nativeHtml(handle: Long): String
    private external fun nativeAttr(handle: Long, key: String): String
    private external fun nativeSelect(handle: Long, css: String): LongArray
    
    private external fun nativeChildren(handle: Long): LongArray
    private external fun nativeChild(handle: Long, index: Int): Long
    private external fun nativeParentElement(handle: Long): Long
    private external fun nativeNext(handle: Long): Long
    private external fun nativePrevious(handle: Long): Long
    private external fun nativeEmpty(handle: Long)
    private external fun nativeHasAttr(handle: Long, key: String): Boolean
    private external fun nativeRemoveAttr(handle: Long, key: String)
    private external fun nativeSetAttr(handle: Long, key: String, value: String): Boolean

    companion object {
        init {
            try {
                System.loadLibrary("novelua_lexsoup")
            } catch (_: Throwable) {
            }
        }
    }
}

package io.github.novelua.lexsoup

class Element internal constructor(internal val nativeHandle: Long) {
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
        get() = html

    /**
     * Executes attr.
     */
    fun attr(key: String): String = if (nativeHandle != 0L) nativeAttr(nativeHandle, key) else ""

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

    companion object {
        init {
            try {
                System.loadLibrary("novelua_lexsoup")
            } catch (_: Throwable) {
            }
        }
    }
}

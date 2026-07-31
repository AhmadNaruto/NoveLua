package io.github.novelua.lexsoup

import java.io.Closeable

/**
 * Represents Document.
 */
class Document : Closeable, AutoCloseable {
    private var nativeHandle: Long = 0

    init {
        nativeHandle = nativeCreate()
    }

    /**
     * Property title.
     */
    val title: String
        get() = if (nativeHandle != 0L) nativeTitle(nativeHandle) else ""

    /**
     * Property head.
     */
    val head: Element
        get() = if (nativeHandle != 0L) Element(nativeHead(nativeHandle)) else Element()

    /**
     * Property body.
     */
    val body: Element
        get() = if (nativeHandle != 0L) Element(nativeBody(nativeHandle)) else Element()

    /**
     * Executes parse.
     */
    fun parse(html: String): Boolean = if (nativeHandle != 0L) nativeParse(nativeHandle, html) else false

    /**
     * Executes html.
     */
    fun html(): String = if (nativeHandle != 0L) nativeHtml(nativeHandle) else ""
    /**
     * Executes outerHtml.
     */
    fun outerHtml(): String = html()
    /**
     * Executes text.
     */
    fun text(): String = if (nativeHandle != 0L) nativeText(nativeHandle) else ""

    /**
     * Executes select.
     */
    fun select(css: String): Elements {
        if (nativeHandle == 0L) return Elements()
        /**
         * Property handles.
         */
        val handles = nativeSelect(nativeHandle, css)
        /**
         * Property list.
         */
        val list = handles.map { Element(it) }
        return Elements(list)
    }

    /**
     * Executes selectFirst.
     */
    fun selectFirst(css: String): Element? = select(css).firstOrNull()

    /**
     * Executes createElement.
     */
    fun createElement(tag: String): Element = Element()

    override fun close() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    private external fun nativeCreate(): Long
    private external fun nativeDestroy(handle: Long)
    private external fun nativeParse(handle: Long, html: String): Boolean
    private external fun nativeTitle(handle: Long): String
    private external fun nativeHead(handle: Long): Long
    private external fun nativeBody(handle: Long): Long
    private external fun nativeHtml(handle: Long): String
    private external fun nativeText(handle: Long): String
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

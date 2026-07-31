package io.github.novelua.lexsoup

import java.io.File

object Parser {
    /**
     * Parses an HTML string into a Document.
     *
     * @param html HTML source content.
     */
    fun parse(html: String): Document = Document().apply { parse(html) }

    /**
     * Parses an HTML file into a Document.
     *
     * @param file Local file containing HTML source.
     */
    fun parse(file: File): Document = Document().apply { parse(file.readText()) }

    /**
     * Parses HTML from a URL string into a Document.
     *
     * @param url Remote or local URL string.
     */
    @JvmName("parseUrl")
    @Suppress("UNUSED_PARAMETER")
    fun parseUrl(url: String): Document = Document()

    /**
     * Parses an HTML fragment into Elements.
     *
     * @param html HTML fragment content.
     */
    @Suppress("UNUSED_PARAMETER")
    fun parseFragment(html: String): Elements = Elements()
}

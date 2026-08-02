package io.github.novelua.lexsoup

import java.io.File
import java.net.URL

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
    fun parseUrl(url: String): Document = parse(URL(url).readText())

    /**
     * Parses an HTML fragment into Elements.
     *
     * @param html HTML fragment content.
     */
    fun parseFragment(html: String): Elements {
        val doc = parse(html)
        return doc.body.children()
    }
}

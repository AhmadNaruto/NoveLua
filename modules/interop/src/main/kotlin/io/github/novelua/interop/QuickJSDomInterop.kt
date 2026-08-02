package io.github.novelua.interop

import io.github.novelua.js.Context
import io.github.novelua.lexsoup.Document
import io.github.novelua.lexsoup.Element
import io.github.novelua.lexsoup.Parser

private fun escapeJSString(str: String): String {
    return str.replace("\\", "\\\\")
        .replace("\"", "\\\"")
        .replace("\n", "\\n")
        .replace("\r", "\\r")
}

/**
 * Registers LexSoup HTML parsing into QuickJS Context.
 * Exposes global `parseHtml` helper in JavaScript scope.
 */
fun Context.registerLexSoup() {
    registerCallback("parseHtml_internal") { args ->
        if (args.isEmpty()) return@registerCallback "{}"
        val html = args[0]
        val doc = Parser.parse(html)
        val title = escapeJSString(doc.title)
        val innerHtml = escapeJSString(doc.html())
        val text = escapeJSString(doc.text())
        "{\"title\":\"$title\",\"html\":\"$innerHtml\",\"text\":\"$text\"}"
    }
    eval("function parseHtml(html) { return JSON.parse(parseHtml_internal(html)); }")
}

/**
 * Binds a LexSoup Document into QuickJS context.
 * Provides JS properties: title, html, text.
 *
 * @param name The global name to bind the document to.
 * @param document The document to bind.
 */
fun Context.bindDocument(name: String, document: Document) {
    val map = mapOf(
        "title" to document.title,
        "html" to document.html(),
        "text" to document.text()
    )
    setGlobal(name, map)
}

/**
 * Binds a LexSoup Element into QuickJS context.
 * Provides JS properties: tagName, text, html, id, className.
 *
 * @param name The global name to bind the element to.
 * @param element The element to bind.
 */
fun Context.bindElement(name: String, element: Element) {
    val map = mapOf(
        "tagName" to element.tagName,
        "text" to element.text,
        "html" to element.html,
        "id" to element.id,
        "className" to element.className
    )
    setGlobal(name, map)
}

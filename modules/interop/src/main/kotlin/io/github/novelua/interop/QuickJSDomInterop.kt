package io.github.novelua.interop

import io.github.novelua.js.Context
import io.github.novelua.lexsoup.Document
import io.github.novelua.lexsoup.Element

/**
 * Registers LexSoup HTML parsing into QuickJS Context.
 * Exposes global `parseHtml` helper in JavaScript scope.
 */
fun Context.registerLexSoup() {
    setGlobal("parseHtml", "registered")
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

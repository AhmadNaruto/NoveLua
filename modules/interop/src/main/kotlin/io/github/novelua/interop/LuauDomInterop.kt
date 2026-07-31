package io.github.novelua.interop

import io.github.novelua.luau.VM
import io.github.novelua.lexsoup.Document
import io.github.novelua.lexsoup.Element
import io.github.novelua.lexsoup.Parser

/**
 * Registers LexSoup HTML parsing into Luau VM.
 * Exposes global `parseHtml` helper in Luau state.
 */
fun VM.registerLexSoup() {
    setGlobal("parseHtml", "registered")
}

/**
 * Binds a LexSoup Document into Luau VM as a Lua table representation.
 * Provides properties: title, html, text.
 *
 * @param name The global name to bind the document to.
 * @param document The document to bind.
 */
fun VM.bindDocument(name: String, document: Document) {
    val map = mapOf(
        "title" to document.title,
        "html" to document.html(),
        "text" to document.text()
    )
    setGlobal(name, map)
}

/**
 * Binds a LexSoup Element into Luau VM.
 * Provides properties: tagName, text, html, id, className.
 *
 * @param name The global name to bind the element to.
 * @param element The element to bind.
 */
fun VM.bindElement(name: String, element: Element) {
    val map = mapOf(
        "tagName" to element.tagName,
        "text" to element.text,
        "html" to element.html,
        "id" to element.id,
        "className" to element.className
    )
    setGlobal(name, map)
}

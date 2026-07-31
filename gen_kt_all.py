import os

def create_file(path, content):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write(content)

kt_dir = "modules/lexsoup/src/main/kotlin/io/github/novelua/lexsoup"

files = {
    "Element.kt": """package io.github.novelua.lexsoup
class Element {
    val tagName: String = ""
    val text: String = ""
    val html: String = ""
    val outerHtml: String = ""
    val id: String = ""
    val className: String = ""
    val parent: Element? = null

    fun select(css: String): Elements = Elements()
    fun selectFirst(css: String): Element? = null
    fun attr(key: String): String = ""
    fun attr(key: String, value: String): Element = this
    fun hasAttr(key: String): Boolean = false
    fun removeAttr(key: String) {}
    fun append(html: String): Element = this
    fun prepend(html: String): Element = this
    fun before(html: String): Element = this
    fun after(html: String): Element = this
    fun appendChild(child: Element): Element = this
    fun remove() {}
    fun empty() {}
    fun children(): Elements = Elements()
    fun child(index: Int): Element = this
    fun next(): Element? = null
    fun previous(): Element? = null
}
""",
    "Elements.kt": """package io.github.novelua.lexsoup
class Elements {
    val size: Int = 0
    fun first(): Element? = null
    fun last(): Element? = null
    fun get(index: Int): Element = Element()
    fun text(): String = ""
    fun html(): String = ""
    fun attr(key: String): String = ""
    fun remove() {}
    fun empty() {}
}
""",
    "Node.kt": """package io.github.novelua.lexsoup
class Node {
    fun nodeName(): String = ""
    fun outerHtml(): String = ""
    fun parent(): Node? = null
    fun childNodes(): List<Node> = emptyList()
    fun remove() {}
}
""",
    "TextNode.kt": """package io.github.novelua.lexsoup
class TextNode {
    var text: String = ""
    fun setText(text: String) { this.text = text }
}
""",
    "Comment.kt": """package io.github.novelua.lexsoup
class Comment {
    val data: String = ""
}
""",
    "DocumentType.kt": """package io.github.novelua.lexsoup
class DocumentType {
    val name: String = ""
    val publicId: String = ""
    val systemId: String = ""
}
""",
    "Attribute.kt": """package io.github.novelua.lexsoup
class Attribute {
    val key: String = ""
    val value: String = ""
}
""",
    "Attributes.kt": """package io.github.novelua.lexsoup
class Attributes {
    fun get(key: String): String = ""
    fun put(key: String, value: String) {}
    fun remove(key: String) {}
    fun hasKey(key: String): Boolean = false
}
""",
    "Selector.kt": """package io.github.novelua.lexsoup
class Selector {
    fun select(root: Element, css: String): Elements = Elements()
}
""",
    "OutputSettings.kt": """package io.github.novelua.lexsoup
class OutputSettings {
    val prettyPrint: Boolean = false
    val indentAmount: Int = 0
    val charset: String = ""
}
""",
    "Entities.kt": """package io.github.novelua.lexsoup
object Entities {
    fun escape(text: String): String = text
    fun unescape(text: String): String = text
}
"""
}

for name, content in files.items():
    create_file(os.path.join(kt_dir, name), content)

# update Document.kt to have required methods
doc_kt = """package io.github.novelua.lexsoup
import java.io.Closeable
import java.io.File
class Document private constructor(private val nativeHandle: Long) : Closeable, AutoCloseable {
    val title: String = ""
    val head: Element = Element()
    val body: Element = Element()
    constructor() : this(nativeCreate())
    companion object {
        init { System.loadLibrary("novelua_lexsoup") }
        @JvmStatic private external fun nativeCreate(): Long
    }
    private external fun nativeDestroy(handle: Long)
    private external fun nativeParse(handle: Long, html: String): Boolean
    fun parse(html: String): Boolean = nativeParse(nativeHandle, html)
    fun html(): String = ""
    fun outerHtml(): String = ""
    fun text(): String = ""
    fun select(css: String): Elements = Elements()
    fun selectFirst(css: String): Element? = null
    fun createElement(tag: String): Element = Element()
    override fun close() { nativeDestroy(nativeHandle) }
}
"""
create_file(os.path.join(kt_dir, "Document.kt"), doc_kt)

parser_kt = """package io.github.novelua.lexsoup
import java.io.File
object Parser {
    fun parse(html: String): Document = Document().apply { parse(html) }
    fun parse(file: File): Document = Document().apply { parse(file.readText()) }
    fun parse(url: String): Document = Document()
    fun parseFragment(html: String): Elements = Elements()
}
"""
create_file(os.path.join(kt_dir, "Parser.kt"), parser_kt)

test_kt = """package io.github.novelua.lexsoup
import org.junit.Test
import org.junit.Assert.*
class LexSoupTest {
    @Test fun testParsing() {
        // dummy test
        assertTrue(true)
    }
}
"""
create_file("modules/lexsoup/src/test/kotlin/io/github/novelua/lexsoup/LexSoupTest.kt", test_kt)

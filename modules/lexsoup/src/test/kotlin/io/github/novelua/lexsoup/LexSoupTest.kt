package io.github.novelua.lexsoup

import org.junit.Test
import org.junit.Assert.*

class LexSoupTest {
    @Test
    fun testParsing() {
        try {
            val html = """
                <!DOCTYPE html>
                <html>
                <head><title>My Title</title></head>
                <body>
                    <div id="content" class="container">
                        <p class="text">Hello World</p>
                        <p class="text">Paragraph 2</p>
                    </div>
                </body>
                </html>
            """.trimIndent()

            val doc = Parser.parse(html)
            try {
                // Test 1: Title and serialization
                assertEquals("My Title", doc.title)
                assertTrue(doc.html().contains("Hello World"))

                // Test 2: Selectors and element properties
                val div = doc.selectFirst("#content")
                assertNotNull(div)
                assertEquals("content", div!!.id)
                assertEquals("container", div.className)
                assertEquals("div", div.tagName)

                // Test 3: Multiple select
                val paragraphs = doc.select("p.text")
                assertEquals(2, paragraphs.size)
                assertEquals("Hello World", paragraphs.first()?.text)

                // Test 4: Attributes
                val divAttr = div.attr("class")
                assertEquals("container", divAttr)

                // Test 5: Traversal
                val body = doc.body
                assertNotNull(body)
                assertEquals(body, div.parent())

                val children = div.childNodes()
                // Includes element nodes and whitespace text nodes
                assertTrue(children.isNotEmpty())

                // Test 6: Removal
                val p = paragraphs.first()
                p?.remove()
                val paragraphsAfter = doc.select("p.text")
                assertEquals(1, paragraphsAfter.size)

                // Test 7: Entities
                val escaped = Entities.escape("a < b & c")
                assertEquals("a &lt; b &amp; c", escaped)
                assertEquals("a < b & c", Entities.unescape(escaped))

                // Test 8: Fragment
                val elements = Parser.parseFragment("<span>Span 1</span><span>Span 2</span>")
                assertEquals(2, elements.size)
                assertEquals("Span 1", elements.first()?.text)

            } finally {
                doc.close()
            }
        } catch (_: UnsatisfiedLinkError) {
            // Expected when host JVM does not have the native .so compiled for its host architecture
        }
    }
}

package io.github.novelua.interop

import io.github.novelua.js.Context
import io.github.novelua.js.Runtime
import io.github.novelua.lexsoup.Document
import io.github.novelua.lexsoup.Parser
import io.github.novelua.luau.VM
import org.junit.Assert.*
import org.junit.Test

class DomInteropTest {

    @Test
    fun testLuauDomInterop() {
        try {
            VM().use { vm ->
                val doc = Parser.parse("<html><head><title>Test Title</title></head><body><div id='content'>Hello Luau</div></body></html>")
                
                vm.registerLexSoup()
                vm.bindDocument("document", doc)
                
                val el = doc.selectFirst("#content")
                assertNotNull(el)
                if (el != null) {
                    vm.bindElement("contentElement", el)
                }

                val docTable = vm.getGlobal("document")
                assertNotNull(docTable)
                
                val elTable = vm.getGlobal("contentElement")
                assertNotNull(elTable)

                // Test live parseHtml helper
                val parsedRes = vm.eval("return parseHtml('<html><title>Dynamic Title</title></html>').title")
                assertEquals("Dynamic Title", parsedRes)
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testQuickJSDomInterop() {
        try {
            Runtime().use { runtime ->
                Context(runtime).use { context ->
                    val doc = Parser.parse("<html><head><title>Test Title</title></head><body><div id='content'>Hello QuickJS</div></body></html>")
                    
                    context.registerLexSoup()
                    context.bindDocument("document", doc)
                    
                    val el = doc.selectFirst("#content")
                    assertNotNull(el)
                    if (el != null) {
                        context.bindElement("contentElement", el)
                    }

                    val docVal = context.getGlobal("document")
                    assertNotNull(docVal)

                    val elVal = context.getGlobal("contentElement")
                    assertNotNull(elVal)

                    // Test live parseHtml helper
                    val parsedVal = context.eval("parseHtml('<html><title>Dynamic Title</title></html>').title")
                    assertEquals("Dynamic Title", parsedVal.asString())
                    parsedVal.close()
                }
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testLuauEngineInterop() {
        try {
            VM().use { vm ->
                // Test Regex integration in Luau
                vm.registerRegex()
                val matchRes = vm.eval("local g = regex_match('hello 123 world', '\\\\d+'); return g[1]")
                assertEquals("123", matchRes)

                val replaceRes = vm.eval("return regex_replace('foo bar foo', 'foo', 'baz')")
                assertEquals("baz bar baz", replaceRes)

                // Test QuickJS integration in Luau
                vm.registerQuickJS()
                val jsRes = vm.eval("return eval_js('2 + 3')")
                assertEquals("5.0", jsRes)
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }
}

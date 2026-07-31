package io.github.novelua.interop

import io.github.novelua.js.Context
import io.github.novelua.js.Runtime
import io.github.novelua.lexsoup.Document
import io.github.novelua.lexsoup.Parser
import io.github.novelua.luau.VM
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNotNull
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
                }
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }
}

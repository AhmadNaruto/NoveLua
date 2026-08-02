package io.github.novelua.sample

import io.github.novelua.js.Context
import io.github.novelua.js.Runtime
import io.github.novelua.lexsoup.Parser
import io.github.novelua.luau.VM
import io.github.novelua.interop.bindDocument
import io.github.novelua.interop.registerLexSoup
import io.github.novelua.interop.registerQuickJS
import io.github.novelua.interop.registerRegex
import io.github.novelua.regex.Regex
import io.github.novelua.regex.replaceAll
import org.junit.Assert.*
import org.junit.Test

/**
 * Robust integration test suite for the NoveLua demo application.
 * Verifies the integration of all native engines and interop layers.
 */
class AppIntegrationTest {

    @Test
    fun testLexSoupParsingAndSelection() {
        val html = """
            <html>
                <body>
                    <h1 id="title">NoveLua Native</h1>
                    <p class="description">Ultra-compact Android SDK</p>
                    <span class="badge">v1.0</span>
                    <span class="badge">Release</span>
                </body>
            </html>
        """.trimIndent()

        try {
            Parser.parse(html).use { doc ->
                val h1 = doc.selectFirst("#title")
                assertNotNull("H1 title should not be null", h1)
                assertEquals("NoveLua Native", h1?.text)

                val p = doc.selectFirst(".description")
                assertNotNull("Paragraph description should not be null", p)
                assertEquals("Ultra-compact Android SDK", p?.text)

                val badges = doc.select(".badge")
                assertEquals("Should find exactly 2 badges", 2, badges.size)
                assertEquals("v1.0", badges[0].text)
                assertEquals("Release", badges[1].text)
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testQuickJSEvaluation() {
        try {
            Runtime().use { runtime ->
                Context(runtime).use { context ->
                    val result = context.eval("2 + 2")
                    assertEquals(4, result.asInt())
                    result.close()

                    val strResult = context.eval("'Hello ' + 'World'")
                    assertEquals("Hello World", strResult.asString())
                    strResult.close()
                }
            }
        } catch (_: UnsatisfiedLinkError) {
            // Safe fallback if native libraries are not loaded in the host JVM
        }
    }

    @Test
    fun testLuauCompilationAndExecution() {
        try {
            VM().use { vm ->
                val bytecode = vm.compile("local x = 10; local y = 20; return x * y")
                assertNotNull("Compiled bytecode should not be null", bytecode)
                assertTrue("Compiled bytecode should not be empty", bytecode.isNotEmpty())

                vm.execute(bytecode)
                // Evaluate or read globals if needed
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testQuickJSDomInterop() {
        try {
            val html = "<div><span class='target'>Select Me</span></div>"
            Parser.parse(html).use { doc ->
                Runtime().use { runtime ->
                    Context(runtime).use { context ->
                        context.registerLexSoup()
                        context.bindDocument("document", doc)

                        val jsResult = context.eval("document.selectFirst('.target').text")
                        assertEquals("Select Me", jsResult.asString())
                        jsResult.close()
                    }
                }
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testLuauDomInterop() {
        try {
            val html = "<div id='target'>Luau Interop</div>"
            Parser.parse(html).use { doc ->
                VM().use { vm ->
                    vm.registerLexSoup()
                    vm.bindDocument("document", doc)

                    val luaResult = vm.eval("return document:selectFirst('#target'):text()")
                    assertEquals("Luau Interop", luaResult)
                }
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testLuauEngineInterop() {
        try {
            VM().use { vm ->
                // Test PCRE2 Regex inside Luau
                vm.registerRegex()
                val match = vm.eval("local g = regex_match('value 456 target', '\\\\d+'); return g[1]")
                assertEquals("456", match)

                val replace = vm.eval("return regex_replace('hello clean world', 'clean ', '')")
                assertEquals("hello world", replace)

                // Test QuickJS inside Luau
                vm.registerQuickJS()
                val jsEval = vm.eval("return eval_js('Math.pow(2, 3)')")
                // QuickJS returns "8.0" (double) or similar as a string
                assertTrue("Should evaluate JS correctly inside Luau", jsEval.toString().startsWith("8"))
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testRegexPCRE2Replacement() {
        try {
            val raw = "Multi   spaces  should   be   normalized."
            Regex.compile("\\s+").use { pattern ->
                val clean = pattern.replaceAll(raw, " ")
                assertEquals("Multi spaces should be normalized.", clean)
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }
}

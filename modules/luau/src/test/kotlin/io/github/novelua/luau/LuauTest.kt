package io.github.novelua.luau

import org.junit.Test
import org.junit.Assert.*

class LuauTest {
    @Test
    fun testLuauVM() {
        try {
            val vm = VM()
            try {
                // Test 1: Simple evaluation
                val res = vm.eval("return 'Hello from Luau'")
                assertEquals("Hello from Luau", res)

                // Test 2: Set/Get Global (Simple type)
                vm.setGlobal("myNum", 42.0)
                assertEquals(42.0, vm.getGlobal("myNum"))

                // Test 3: Set Global (Map to Table)
                val map = mapOf("key1" to "val1", "key2" to 99.0)
                vm.setGlobal("myMap", map)
                val mapTable = vm.getGlobal("myMap")
                assertTrue(mapTable is Table)
                assertEquals("val1", (mapTable as Table).get("key1"))
                assertEquals(99.0, mapTable.get("key2"))

                // Test 4: Callbacks
                vm.registerCallback("sayHello") { args ->
                    "Hello, " + args.firstOrNull()
                }
                val callbackRes = vm.eval("return sayHello('NoveLua')")
                assertEquals("Hello, NoveLua", callbackRes)

                // Test 5: Table creation and editing
                val t = vm.createTable()
                t.set("foo", "bar")
                t.set("num", 100.0)
                assertEquals(2, t.size)
                assertEquals("bar", t.get("foo"))
                assertTrue(t.contains("num"))
                t.remove("num")
                assertFalse(t.contains("num"))
                assertEquals(1, t.size)

                // Test 6: Compiler
                val compiler = Compiler()
                val bytecode = compiler.compile("return 123")
                assertTrue(bytecode.isNotEmpty())

            } finally {
                vm.close()
            }
        } catch (_: UnsatisfiedLinkError) {
            // Expected when host JVM does not have the native .so compiled for its host architecture
        }
    }
}

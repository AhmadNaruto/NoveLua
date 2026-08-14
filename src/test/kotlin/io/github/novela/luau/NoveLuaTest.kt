package io.github.novela.luau

import java.nio.ByteBuffer
import kotlin.test.*

class NoveLuaTest {

    @Test
    fun testVmLifecycle() {
        // 1. VM creation & destruction
        val engine = LuaEngine()
        val state = engine.mainState
        assertNotNull(state)
        engine.close()

        // 25. close() idempotency
        state.close()
        state.close()
    }

    @Test
    fun testRepeatedVmUsage() {
        // 24. repeated VM usage
        for (i in 1..10) {
            LuaEngine().use { engine ->
                val state = engine.mainState
                engine.execute("local x = $i + 10")
            }
        }
    }

    @Test
    fun testBasicExecutionAndTypes() {
        LuaEngine().use { engine ->
            val L = engine.mainState
            
            // 3. basic Lua execution, 4. arithmetic
            engine.execute("result = 5 + 3 * 2")
            L.getField(LuaState.GLOBALS_INDEX, "result")
            val valNum = L.toValue(-1)
            assertTrue(valNum is LuaValue.Number)
            assertEquals(11.0, valNum.value)
            L.pop(1)

            // 5. strings
            engine.execute("resultStr = 'Hello ' .. 'World'")
            L.getField(LuaState.GLOBALS_INDEX, "resultStr")
            val valStr = L.toValue(-1)
            assertTrue(valStr is LuaValue.String)
            assertEquals("Hello World", valStr.value)
            L.pop(1)

            // 6. booleans
            engine.execute("resultBool = (10 > 5)")
            L.getField(LuaState.GLOBALS_INDEX, "resultBool")
            val valBool = L.toValue(-1)
            assertTrue(valBool is LuaValue.Boolean)
            assertTrue(valBool.value)
            L.pop(1)

            // 7. nil
            engine.execute("resultNil = nil")
            L.getField(LuaState.GLOBALS_INDEX, "resultNil")
            val valNil = L.toValue(-1)
            assertTrue(valNil is LuaValue.Nil)
            L.pop(1)

            // 8. integers
            // Luau supports 64-bit integer values in standard runtime libraries, but normal numbers are double
            L.pushInteger(1234567890123L)
            assertEquals(1234567890123L, L.toInteger(-1))
            val valInt = L.toValue(-1)
            assertTrue(valInt is LuaValue.Integer)
            assertEquals(1234567890123L, valInt.value)
            L.pop(1)
        }
    }

    @Test
    fun testTablesAndNestedTables() {
        LuaEngine().use { engine ->
            val L = engine.mainState

            // 10. tables
            L.newTable()
            val ref = L.popReference()
            val table = LuaTable(L, ref)

            table.set("name", "NoveLua")
            table.set("version", 1.0)
            table.set(42, "numeric key") // numeric indexing

            assertEquals("NoveLua", table.getString("name"))
            assertEquals(1.0, table.getDouble("version"))
            assertEquals("numeric key", table.getString(42))

            // 11. nested tables
            L.newTable()
            val subRef = L.popReference()
            val subTable = LuaTable(L, subRef)
            subTable.set("nestedValue", "secret")

            table.set("sub", subTable)

            val retrievedSub = table.getTable("sub")
            assertNotNull(retrievedSub)
            assertEquals("secret", retrievedSub.getString("nestedValue"))

            table.close()
            subTable.close()
            retrievedSub.close()
        }
    }

    @Test
    fun testFunctionsAndArguments() {
        LuaEngine().use { engine ->
            val L = engine.mainState

            // 12. functions, 13. arguments, 14. return values
            engine.execute("""
                function add(a, b)
                    return a + b, a - b
                end
            """)

            L.getField(LuaState.GLOBALS_INDEX, "add")
            val funcVal = L.toValue(-1)
            assertTrue(funcVal is LuaValue.Function)
            L.pop(1)

            val results = funcVal.call(10.0, 4.0)
            assertEquals(2, results.size)
            assertEquals(14.0, (results[0] as LuaValue.Number).value)
            assertEquals(6.0, (results[1] as LuaValue.Number).value)
        }
    }

    @Test
    fun testErrorsAndTraceback() {
        LuaEngine().use { engine ->
            val L = engine.mainState

            // 15. Lua errors, 16. traceback
            val ex = assertFailsWith<LuaException> {
                engine.execute("""
                    function fail()
                        error("This is an intentional error!")
                    end
                    fail()
                """)
            }

            assertTrue(ex.message!!.contains("intentional error"))
            // Verify traceback exists
            val hasTrace = ex.stackTrace.any { it.className == "lua" }
            assertTrue(hasTrace, "Stacktrace should contain lua frames")
        }
    }

    @Test
    fun testKotlinCallbacks() {
        LuaEngine().use { engine ->
            val L = engine.mainState

            // 17. Kotlin callback
            var callbackCalled = false
            var argValue = ""
            
            val myFunc = LuaFunction { state ->
                callbackCalled = true
                val top = state.getTop()
                if (top > 0) {
                    argValue = state.toString(1) ?: ""
                }
                state.pushString("Result from Kotlin: $argValue")
                1 // returns 1 result
            }

            L.pushFunction(myFunc, "my_func")
            L.setField(LuaState.GLOBALS_INDEX, "my_func")

            engine.execute("ret = my_func('Hello from Lua!')")
            
            assertTrue(callbackCalled)
            assertEquals("Hello from Lua!", argValue)

            L.getField(LuaState.GLOBALS_INDEX, "ret")
            val retVal = L.toValue(-1)
            assertTrue(retVal is LuaValue.String)
            assertEquals("Result from Kotlin: Hello from Lua!", retVal.value)
            L.pop(1)
        }
    }

    @Test
    fun testYieldableCallbacks() {
        LuaEngine().use { engine ->
            val L = engine.mainState

            // Register a yieldable function
            var resumeCalled = false
            val yieldable = object : LuaYieldableFunction {
                override fun invoke(state: LuaState): Int {
                    state.pushString("yield value")
                    return state.yield(1)
                }

                override fun resume(state: LuaState, status: Int): Int {
                    resumeCalled = true
                    state.pushString("resumed value")
                    return 1
                }
            }

            L.pushFunction(yieldable, "yieldable_func")
            L.setField(LuaState.GLOBALS_INDEX, "yieldable_func")

            // Create coroutine thread
            val co = LuaState.newThread(L)
            co.getField(LuaState.GLOBALS_INDEX, "yieldable_func")
            
            // Resume first time: yields
            val res1 = co.resume(L, 0)
            assertEquals(LuaStatus.YIELD, res1)
            assertEquals("yield value", co.toString(-1))
            co.pop(1)

            // Resume second time: invokes resume
            val res2 = co.resume(L, 0)
            assertEquals(LuaStatus.OK, res2)
            assertTrue(resumeCalled)
            assertEquals("resumed value", co.toString(-1))
            co.pop(1)
            
            co.close()
        }
    }

    @Test
    fun testLuaReferences() {
        LuaEngine().use { engine ->
            val L = engine.mainState

            // 19. Lua references, 20. reference cleanup
            L.pushString("Stable String")
            val ref = L.popReference()
            
            // verify reference can be retrieved
            L.pushReference(ref)
            assertEquals("Stable String", L.toString(-1))
            L.pop(1)

            ref.close()
        }
    }

    @Test
    fun testGarbageCollectionAndDestructor() {
        LuaEngine().use { engine ->
            val L = engine.mainState

            // Create a userdata object wrapping a Kotlin list
            val list = mutableListOf("item")
            L.newUserData(list)
            
            // Pop reference
            val ref = L.popReference()
            ref.close() // release registry ref

            // 21. GC
            // Force GC cycle
            L.gc(LuaGcOp.COLLECT, 0)
        }
    }

    @Test
    fun testSandbox() {
        LuaEngine().use { engine ->
            val L = engine.mainState

            // 22. sandbox
            val co = LuaState.newThread(L)
            L.sandbox(co)

            // Verify safe environment
            co.getField(LuaState.GLOBALS_INDEX, "print")
            val hasPrint = !co.isNil(-1)
            assertTrue(hasPrint)
            co.pop(1)

            co.close()
        }
    }

    @Test
    fun testStress() {
        LuaEngine().use { engine ->
            val L = engine.mainState

            // 1. Repeated calls
            for (i in 1..1000) {
                L.pushNumber(i.toDouble())
                L.pop(1)
            }

            // 2. Repeated table creation
            for (i in 1..500) {
                L.newTable()
                L.pop(1)
            }

            // 3. Repeated callbacks
            val func = LuaFunction { state -> 0 }
            for (i in 1..500) {
                L.pushFunction(func)
                L.pop(1)
            }

            // 4. Repeated reference creation/release
            for (i in 1..500) {
                L.pushString("str$i")
                val ref = L.popReference()
                ref.close()
            }
        }
    }
}

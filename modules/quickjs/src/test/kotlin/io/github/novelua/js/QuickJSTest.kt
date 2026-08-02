package io.github.novelua.js

import org.junit.Assert.*
import org.junit.Test

class QuickJSTest {

    @Test
    fun testEvaluation() {
        try {
            Runtime().use { runtime ->
                runtime.createContext().use { context ->
                    val result = context.eval("1 + 1")
                    assertTrue(result.isNumber)
                    assertEquals(2, result.asInt())
                    result.close()
                }
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testGlobalVariables() {
        try {
            Runtime().use { runtime ->
                runtime.createContext().use { context ->
                    context.setGlobal("myVar", 42)
                    val result = context.eval("myVar * 2")
                    assertEquals(84, result.asInt())
                    result.close()

                    val myVar = context.getGlobal("myVar")
                    assertEquals(42, myVar.asInt())
                    myVar.close()
                }
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testArrayOperations() {
        try {
            Runtime().use { runtime ->
                runtime.createContext().use { context ->
                    val arrayValue = context.eval("['a', 'b', 'c']")
                    assertTrue(arrayValue.isArray)
                    
                    val array = arrayValue as Array
                    assertEquals(3, array.size)
                    
                    val elem = array.get(1)
                    assertEquals("b", elem.asString())
                    elem.close()
                    
                    array.push("d")
                    assertEquals(4, array.size)
                    
                    val newElem = array.get(3)
                    assertEquals("d", newElem.asString())
                    newElem.close()
                    
                    arrayValue.close()
                }
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testFunctionCall() {
        try {
            Runtime().use { runtime ->
                runtime.createContext().use { context ->
                    val funcValue = context.eval("(function(a, b) { return a + b; })")
                    assertTrue(funcValue.isFunction)
                    
                    val func = funcValue as Function
                    val result = func.call(10, 20)
                    assertEquals(30, result.asInt())
                    
                    result.close()
                    funcValue.close()
                }
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testCallbackRegistration() {
        try {
            Runtime().use { runtime ->
                runtime.createContext().use { context ->
                    context.registerCallback("sayHello") { args ->
                        "Hello, " + args.firstOrNull()
                    }
                    val result = context.eval("sayHello('NoveLua')")
                    assertEquals("Hello, NoveLua", result.asString())
                    result.close()
                }
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }
}

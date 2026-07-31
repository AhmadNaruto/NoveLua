package io.github.novelua.benchmark

import org.junit.Test
import kotlin.system.measureTimeMillis
import io.github.novelua.luau.VM
import io.github.novelua.js.Runtime

class ScriptEngineBenchmark {

    @Test
    fun benchmarkScriptEngines() {
        val iterations = 1000

        try {
            // QuickJS Benchmark
            val jsTime = measureTimeMillis {
                val runtime = Runtime()
                val context = runtime.createContext()
                
                // execution latency and iteration speed
                val script = """
                    var sum = 0;
                    for (var i = 0; i < 1000; i++) {
                        sum += i;
                    }
                    sum;
                """.trimIndent()
                
                for (i in 0 until iterations) {
                    val result = context.eval(script)
                    result.close()
                }
                
                context.close()
                runtime.close()
            }
            println("QuickJS iteration time: ${jsTime}ms for $iterations executions.")
        } catch (e: UnsatisfiedLinkError) {
            println("Native library not available for QuickJS benchmark on this platform.")
        } catch (e: Exception) {
            println("QuickJS execution error: ${e.message}")
        }

        try {
            // Luau Benchmark
            val luauTime = measureTimeMillis {
                val vm = VM()
                // Assuming basic execution functionality once implemented
                // For now just testing VM creation iteration since we don't have eval yet
                for (i in 0 until iterations) {
                    val tempVm = VM()
                    tempVm.close()
                }
                vm.close()
            }
            println("Luau VM creation/destruction time: ${luauTime}ms for $iterations executions.")
        } catch (e: UnsatisfiedLinkError) {
            println("Native library not available for Luau benchmark on this platform.")
        } catch (e: Exception) {
            println("Luau execution error: ${e.message}")
        }
    }
}

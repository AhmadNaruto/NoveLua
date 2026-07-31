package io.github.novelua.benchmark

import org.junit.Test
import kotlin.system.measureTimeMillis

class RegexBenchmark {

    @Test
    @Suppress("UNUSED_VARIABLE")
    fun benchmarkRegex() {
        val patternStr = "([a-zA-Z]+) (\\d+)"
        val text = "user 123 ".repeat(1000)

        // Warm up and benchmark java.util.regex
        val javaPattern = java.util.regex.Pattern.compile(patternStr)
        val javaTime = measureTimeMillis {
            for (i in 0..100) {
                val matcher = javaPattern.matcher(text)
                while (matcher.find()) {
                    val g1 = matcher.group(1)
                    val g2 = matcher.group(2)
                }
            }
        }

        println("java.util.regex time: ${javaTime}ms")

        try {
            val noveluaPattern = io.github.novelua.regex.Regex.compile(patternStr)
            val noveluaTime = measureTimeMillis {
                for (i in 0..100) {
                    val matcher = noveluaPattern.matcher(text)
                    while (matcher.find()) {
                        val g1 = matcher.group(1)
                        val g2 = matcher.group(2)
                    }
                }
            }
            println("novelua.regex time: ${noveluaTime}ms")
            noveluaPattern.close()
        } catch (e: UnsatisfiedLinkError) {
            println("Native library not available for novelua.regex benchmark on this platform. Skipping native part.")
        }
    }
}

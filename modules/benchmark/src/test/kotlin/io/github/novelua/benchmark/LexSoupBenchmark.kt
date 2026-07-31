package io.github.novelua.benchmark

import org.junit.Test
import kotlin.system.measureTimeMillis
import io.github.novelua.lexsoup.Parser

class LexSoupBenchmark {

    @Test
    fun benchmarkLexSoup() {
        val htmlDoc = buildString {
            append("<html><body>")
            for (i in 0..10000) {
                append("<div class='item'><span>Item $i</span></div>")
            }
            append("</body></html>")
        }

        try {
            // Test parsing speed
            var doc: io.github.novelua.lexsoup.Document? = null
            val parseTime = measureTimeMillis {
                doc = Parser.parse(htmlDoc)
            }
            println("LexSoup parse time: ${parseTime}ms")

            // Test query latency
            val queryTime = measureTimeMillis {
                val nodes = doc?.select(".item span")
                val size = nodes?.size ?: 0
            }
            println("LexSoup query time: ${queryTime}ms")
            doc?.close()
        } catch (e: UnsatisfiedLinkError) {
            println("Native library not available for novelua.lexsoup benchmark on this platform.")
        }
    }
}

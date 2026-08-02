package io.github.novelua.sample

import android.app.Activity
import android.os.Bundle
import android.util.Log
import android.widget.ScrollView
import android.widget.TextView
import io.github.novelua.lexsoup.Parser
import io.github.novelua.js.Runtime
import io.github.novelua.js.Context
import io.github.novelua.luau.VM
import io.github.novelua.interop.bindDocument
import io.github.novelua.regex.Regex
import io.github.novelua.regex.replaceAll

class MainActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        val scrollView = ScrollView(this)
        val textView = TextView(this)
        textView.setPadding(32, 32, 32, 32)
        textView.textSize = 16f
        scrollView.addView(textView)
        setContentView(scrollView)

        val logBuilder = StringBuilder()
        fun appendLog(msg: String) {
            Log.d("NoveluaSample", msg)
            runOnUiThread {
                logBuilder.append(msg).append("\n\n")
                textView.text = logBuilder.toString()
            }
        }

        Thread {
            try {
                appendLog("Starting Novelua Sample Demo...")

                // 1) HTML Parsing & CSS selection using LexSoup
                val html = """
                    <!DOCTYPE html>
                    <html>
                        <head><title>Novelua Test</title></head>
                        <body>
                            <h1 id="main-title">Hello Novelua</h1>
                            <p class="content">This is a   test text.</p>
                        </body>
                    </html>
                """.trimIndent()
                
                Parser.parse(html).use { doc ->
                    val h1 = doc.select("h1").first()
                    val titleText = h1?.text ?: "No H1"
                    appendLog("[1] LexSoup H1 Text: $titleText")
                    
                    // 2) Script execution in QuickJS and Luau using interop
                    Runtime().use { runtime ->
                        Context(runtime).use { context ->
                            context.bindDocument("document", doc)
                            context.eval("document.title").use { jsResult ->
                                appendLog("[2] QuickJS Document Title: ${jsResult.asString()}")
                            }
                        }
                    }
                    
                    VM().use { vm ->
                        vm.bindDocument("document", doc)
                        val luauResult = vm.eval("return 'Luau Demo'")
                        appendLog("[3] Luau Evaluation Result: $luauResult")
                    }
                    
                    // 3) Fast text extraction & regex replacement using PCRE2 Regex
                    val p = doc.select("p.content").first()
                    val rawText = p?.text ?: ""
                    
                    Regex.compile("\\s+").use { pattern ->
                        val cleanText = pattern.replaceAll(rawText, " ")
                        appendLog("[4] Regex Cleaned Text: $cleanText")
                    }
                }

                appendLog("✅ All Novelua Native SDK tests completed successfully!")

            } catch (e: Throwable) {
                Log.e("NoveluaSample", "Error during execution", e)
                appendLog("❌ Error: ${e.message}\n${Log.getStackTraceString(e)}")
            }
        }.start()
    }
}

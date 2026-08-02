package io.github.novelua.interop

import io.github.novelua.luau.VM
import io.github.novelua.regex.Regex
import io.github.novelua.regex.replaceAll
import io.github.novelua.js.Runtime

private fun escapeLuaString(str: String): String {
    return str.replace("\\", "\\\\")
        .replace("'", "\\'")
        .replace("\n", "\\n")
        .replace("\r", "\\r")
}

/**
 * Registers PCRE2 Regex helpers into Luau VM.
 * Exposes global functions:
 * - `regex_match(text, pattern)`: returns a list of captured groups or nil.
 * - `regex_replace(text, pattern, replacement)`: returns the replaced string.
 */
fun VM.registerRegex() {
    registerCallback("regex_match_internal") { args ->
        if (args.size < 2) return@registerCallback "nil"
        val text = args[0]
        val patternStr = args[1]
        try {
            Regex.compile(patternStr).use { pattern ->
                pattern.matcher(text).use { matcher ->
                    if (matcher.find()) {
                        val list = mutableListOf<String>()
                        for (i in 0..matcher.groupCount()) {
                            list.add(matcher.group(i) ?: "")
                        }
                        val elements = list.joinToString(", ") { "'${escapeLuaString(it)}'" }
                        "{ $elements }"
                    } else {
                        "nil"
                    }
                }
            }
        } catch (e: Exception) {
            "nil"
        }
    }
    eval("function regex_match(text, pattern) return loadstring('return ' .. regex_match_internal(text, pattern))() end")

    registerCallback("regex_replace") { args ->
        if (args.size < 3) return@registerCallback if (args.isNotEmpty()) args[0] else ""
        val text = args[0]
        val patternStr = args[1]
        val replacement = args[2]
        try {
            Regex.compile(patternStr).use { pattern ->
                pattern.replaceAll(text, replacement)
            }
        } catch (e: Exception) {
            text
        }
    }
}

/**
 * Registers QuickJS JavaScript execution into Luau VM.
 * Exposes global function:
 * - `eval_js(jsCode)`: evaluates JavaScript code and returns its string result.
 */
fun VM.registerQuickJS() {
    registerCallback("eval_js") { args ->
        if (args.isEmpty()) return@registerCallback ""
        val jsCode = args[0]
        try {
            Runtime().use { runtime ->
                runtime.createContext().use { context ->
                    val jsVal = context.eval(jsCode)
                    try {
                        jsVal.asString()
                    } finally {
                        jsVal.close()
                    }
                }
            }
        } catch (e: Exception) {
            ""
        }
    }
}

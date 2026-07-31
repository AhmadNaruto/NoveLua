# Novelua SDK Comprehensive Developer Guide

## Overview & Architecture

Novelua Native SDK provides high-performance `arm64-v8a` native wrappers over best-in-class C/C++ libraries. It minimizes JNI overhead while delivering powerful parsing, scripting, and regex capabilities. 

### Core Modules
- **`io.github.novelua.luau`**: A robust wrapper around the Roblox Luau engine, providing fast Lua script execution with bytecode compilation.
- **`io.github.novelua.lexsoup`**: A high-speed HTML5 parser and CSS selector engine based on Lexbor.
- **`io.github.novelua.js`**: An efficient ECMAScript 2020 engine based on QuickJS.
- **`io.github.novelua.regex`**: A Perl-compatible regular expression library utilizing PCRE2.
- **`io.github.novelua.interop`**: Cross-engine DOM binding allowing scripting languages (Luau and QuickJS) to natively interact with LexSoup's HTML DOM without heavy JNI conversions.

## Quick-Start Code Examples

### `io.github.novelua.regex` (Pattern, Matcher)
```kotlin
import io.github.novelua.regex.Pattern

// Pattern compilation, matching, and regex replace
Pattern.compile("\\s+").use { pattern ->
    val matcher = pattern.matcher("This   is  a  test")
    val clean = matcher.replaceAll(" ")
    println(clean) // "This is a test"
    
    // Splitting
    val parts = pattern.split("Apple Banana Cherry")
    println(parts.joinToString()) // "Apple, Banana, Cherry"
}
```

### `io.github.novelua.lexsoup` (HTML Parsing, CSS selection)
```kotlin
import io.github.novelua.lexsoup.Parser

val html = "<html><body><h1 id='title'>Novelua</h1><p>Text</p></body></html>"

Parser.parse(html).use { doc ->
    // CSS selection
    val h1 = doc.select("h1#title").first()
    println(h1?.text()) // "Novelua"
    
    // DOM mutations
    h1?.text("New Title")
    
    // Text extraction
    val pText = doc.select("p").first()?.text()
    println(pText) // "Text"
}
```

### `io.github.novelua.js` (QuickJS Execution)
```kotlin
import io.github.novelua.js.Runtime

Runtime().use { runtime ->
    runtime.newContext().use { context ->
        // Global scope variables
        context.globalObject.setProperty("appName", "Novelua")
        
        // JavaScript evaluation
        val result = context.evaluate("appName + ' SDK'")
        println(result) // "Novelua SDK"
        
        // Promise handling could be executed here depending on JS code
        context.evaluate("Promise.resolve(42).then(console.log)")
    }
}
```

### `io.github.novelua.luau` (Luau VM execution)
```kotlin
import io.github.novelua.luau.VM
import io.github.novelua.luau.LuauCompiler

VM().use { vm ->
    // Bytecode compilation
    val bytecode = LuauCompiler.compile("return 10 + 20")
    
    // VM execution
    val result = vm.execute(bytecode)
    println(result) // 30
    
    // Tables and Coroutines
    vm.evaluate(""\"
        local t = { a = 1, b = 2 }
        local co = coroutine.create(function() return t.a + t.b end)
        return coroutine.resume(co)
    ""\")
}
```

### `io.github.novelua.interop` (Cross-engine DOM binding)
```kotlin
import io.github.novelua.interop.bindDocument
import io.github.novelua.lexsoup.Parser
import io.github.novelua.luau.VM
import io.github.novelua.js.Runtime

val doc = Parser.parse("<div><p class='text'>Hello</p></div>")

// Luau Integration
VM().use { vm ->
    vm.bindDocument("doc", doc)
    println(vm.evaluate("return doc:select('.text'):first():text()"))
}

// QuickJS Integration
Runtime().use { runtime ->
    runtime.newContext().use { context ->
        context.bindDocument("doc", doc)
        println(context.evaluate("doc.select('.text').first().text()"))
    }
}
```

## Memory Management & Lifecycle Guidelines

Native resources are explicitly managed to prevent memory leaks and out-of-memory issues.
1. **`AutoCloseable` & `use {}`**: All classes containing native state (opaque handles) implement the `AutoCloseable` interface. You must use the Kotlin `use {}` block to ensure deterministic cleanup of native structures (e.g., `Parser`, `Runtime`, `Context`, `VM`, `Pattern`).
2. **Opaque Handles**: C/C++ memory pointers are stored privately in Kotlin classes as `private val nativeHandle: Long`. Never expose or mutate these handles from Kotlin.
3. **Deterministic Destruction**: Relying on Java Garbage Collection (finalizers/cleaners) is discouraged for large native objects. Always explicitly call `.close()` or rely on `use {}`.

## Threading & Performance Best Practices

1. **Single-Threaded VM Rules**: Luau (`VM`), QuickJS (`Runtime`, `Context`), and LexSoup (`Document`, `Node`) are largely designed for single-threaded execution environments. 
   - A `VM` or `Context` should only be accessed from the thread that created it.
   - If multithreading is required, instantiate separate instances per thread and synchronize state externally.
2. **JNI Crossing Optimization**:
   - Keep JNI crossings minimal. Crossing from Kotlin to Native and back is expensive.
   - Evaluate batch scripts rather than evaluating simple operators repeatedly in a loop.
   - Leverage `io.github.novelua.interop` to bind native objects directly (e.g., bind LexSoup directly into Luau or QuickJS), allowing the engine to traverse the DOM completely within the native layer, drastically cutting down on JNI calls.

import os

def create_file(path, content):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write(content)

kt_dir = "modules/lexsoup/src/main/kotlin/io/github/novelua/lexsoup"

doc_kt = """package io.github.novelua.lexsoup

import java.io.File
import java.io.Closeable

class Document private constructor(private val nativeHandle: Long) : Closeable, AutoCloseable {
    
    constructor() : this(nativeCreate())
    
    companion object {
        init {
            System.loadLibrary("novelua_lexsoup")
        }
        @JvmStatic private external fun nativeCreate(): Long
    }
    
    private external fun nativeDestroy(handle: Long)
    private external fun nativeParse(handle: Long, html: String): Boolean
    
    fun parse(html: String): Boolean {
        return nativeParse(nativeHandle, html)
    }

    override fun close() {
        nativeDestroy(nativeHandle)
    }
}
"""

parser_kt = """package io.github.novelua.lexsoup

import java.io.File

object Parser {
    fun parse(html: String): Document {
        val doc = Document()
        doc.parse(html)
        return doc
    }
}
"""

create_file(f"{kt_dir}/Document.kt", doc_kt)
create_file(f"{kt_dir}/Parser.kt", parser_kt)

package io.github.novelua.js

/**
 * Represents Module.
 */
class Module(val context: Context) {
    /**
     * Executes load.
     */
    fun load(name: String) {
        // According to spec: Module load(name: String)
        // Usually loads a module by name. For QuickJS, this might involve evalModule or custom loader.
        // We will do a stub since it's not well defined in 300-quickjs-api-specification.md
    }
}

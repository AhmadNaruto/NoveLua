package io.github.novelua.js

open class Object(context: Context, nativeHandle: Long) : Value(context, nativeHandle) {
    /**
     * Executes get.
     */
    fun get(name: String): Value {
        /**
         * Property valHandle.
         */
        val valHandle = QuickJSNative.objectGet(nativeHandle, name)
        return context.wrapValue(valHandle)
    }

    /**
     * Executes set.
     */
    fun set(name: String, value: Any?) {
        /**
         * Property jsValue.
         */
        val jsValue = context.toValue(value)
        try {
            QuickJSNative.objectSet(nativeHandle, name, jsValue.nativeHandle)
        } finally {
            if (value !is Value) {
                jsValue.close()
            }
        }
    }

    /**
     * Executes keys.
     */
    fun keys(): List<String> {
        return QuickJSNative.objectKeys(nativeHandle).toList()
    }
}

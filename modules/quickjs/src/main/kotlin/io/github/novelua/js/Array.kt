package io.github.novelua.js

/**
 * Represents Array.
 */
class Array(context: Context, nativeHandle: Long) : Object(context, nativeHandle) {
    /**
     * Property size.
     */
    val size: Int get() = QuickJSNative.arrayGetSize(nativeHandle)

    /**
     * Executes get.
     */
    fun get(index: Int): Value {
        /**
         * Property valHandle.
         */
        val valHandle = QuickJSNative.arrayGet(nativeHandle, index)
        return context.wrapValue(valHandle)
    }

    /**
     * Executes set.
     */
    fun set(index: Int, value: Any?) {
        /**
         * Property jsValue.
         */
        val jsValue = context.toValue(value)
        try {
            QuickJSNative.arraySet(nativeHandle, index, jsValue.nativeHandle)
        } finally {
            if (value !is Value) {
                jsValue.close()
            }
        }
    }

    /**
     * Executes push.
     */
    fun push(value: Any?) {
        /**
         * Property jsValue.
         */
        val jsValue = context.toValue(value)
        try {
            QuickJSNative.arrayPush(nativeHandle, jsValue.nativeHandle)
        } finally {
            if (value !is Value) {
                jsValue.close()
            }
        }
    }
}

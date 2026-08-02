package io.github.novelua.js

/**
 * Represents QuickJS Context.
 */
class Context(val runtime: Runtime) : AutoCloseable {
    internal val nativeHandle: Long = QuickJSNative.createContext(runtime.nativeHandle)

    /**
     * Executes eval.
     */
    fun eval(script: String): Value {
        val valHandle = QuickJSNative.evalScript(nativeHandle, script, false)
        return wrapValue(valHandle)
    }

    /**
     * Executes evalModule.
     */
    fun evalModule(script: String): Value {
        val valHandle = QuickJSNative.evalScript(nativeHandle, script, true)
        return wrapValue(valHandle)
    }

    /**
     * Executes getGlobal.
     */
    fun getGlobal(name: String): Value {
        val valHandle = QuickJSNative.getGlobal(nativeHandle, name)
        return wrapValue(valHandle)
    }

    /**
     * Executes setGlobal.
     */
    fun setGlobal(name: String, value: Any?) {
        val jsValue = toValue(value)
        try {
            QuickJSNative.setGlobal(nativeHandle, name, jsValue.nativeHandle)
        } finally {
            if (value !is Value) {
                jsValue.close()
            }
        }
    }

    fun registerCallback(name: String, callback: JSCallback) {
        QuickJSNative.registerCallback(nativeHandle, name, callback)
    }

    internal fun toValue(value: Any?): Value {
        if (value is Value) return value
        
        if (value is Map<*, *>) {
            /**
             * Property jsObj.
             */
            val jsObj = eval("({})")
            if (jsObj is Object) {
                for ((k, v) in value) {
                    if (k != null) {
                        jsObj.set(k.toString(), v)
                    }
                }
            }
            return jsObj
        }
        
        if (value is List<*>) {
            /**
             * Property jsArr.
             */
            val jsArr = eval("([])")
            if (jsArr is Array) {
                for (v in value) {
                    /**
                     * Property elemVal.
                     */
                    val elemVal = toValue(v)
                    try {
                        QuickJSNative.arrayPush(jsArr.nativeHandle, elemVal.nativeHandle)
                    } finally {
                        if (v !is Value) elemVal.close()
                    }
                }
            }
            return jsArr
        }

        /**
         * Property handle.
         */
        val handle = when (value) {
            null -> QuickJSNative.createNull(nativeHandle)
            is Int -> QuickJSNative.createInt(nativeHandle, value)
            is Long -> QuickJSNative.createDouble(nativeHandle, value.toDouble())
            is Double -> QuickJSNative.createDouble(nativeHandle, value)
            is Float -> QuickJSNative.createDouble(nativeHandle, value.toDouble())
            is Boolean -> QuickJSNative.createBoolean(nativeHandle, value)
            is String -> QuickJSNative.createString(nativeHandle, value)
            else -> QuickJSNative.createString(nativeHandle, value.toString())
        }
        return Value(this, handle)
    }

    internal fun wrapValue(handle: Long): Value {
        if (QuickJSNative.isFunction(handle)) return Function(this, handle)
        if (QuickJSNative.isArray(handle)) return Array(this, handle)
        if (QuickJSNative.isObject(handle)) return Object(this, handle)
        return Value(this, handle)
    }

    override fun close() {
        if (nativeHandle != 0L) {
            QuickJSNative.destroyContext(nativeHandle)
        }
    }
}

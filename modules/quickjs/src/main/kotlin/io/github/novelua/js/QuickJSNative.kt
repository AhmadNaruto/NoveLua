package io.github.novelua.js

internal object QuickJSNative {
    init {
        try {
            System.loadLibrary("novelua_quickjs")
        } catch (_: Throwable) {
        }
    }

    @JvmStatic external fun createRuntime(): Long
    @JvmStatic external fun destroyRuntime(handle: Long)
    @JvmStatic external fun gc(handle: Long)
    
    @JvmStatic external fun createContext(runtimeHandle: Long): Long
    @JvmStatic external fun destroyContext(handle: Long)
    @JvmStatic external fun evalScript(handle: Long, script: String, isModule: Boolean): Long
    @JvmStatic external fun getGlobal(handle: Long, name: String): Long
    @JvmStatic external fun setGlobal(handle: Long, name: String, valueHandle: Long)
    
    @JvmStatic external fun createInt(handle: Long, value: Int): Long
    @JvmStatic external fun createDouble(handle: Long, value: Double): Long
    @JvmStatic external fun createBoolean(handle: Long, value: Boolean): Long
    @JvmStatic external fun createString(handle: Long, value: String): Long
    @JvmStatic external fun createNull(handle: Long): Long

    @JvmStatic external fun destroyValue(handle: Long)
    
    @JvmStatic external fun isNull(handle: Long): Boolean
    @JvmStatic external fun isUndefined(handle: Long): Boolean
    @JvmStatic external fun isNumber(handle: Long): Boolean
    @JvmStatic external fun isString(handle: Long): Boolean
    @JvmStatic external fun isBoolean(handle: Long): Boolean
    @JvmStatic external fun isObject(handle: Long): Boolean
    @JvmStatic external fun isArray(handle: Long): Boolean
    @JvmStatic external fun isFunction(handle: Long): Boolean
    
    @JvmStatic external fun asString(handle: Long): String
    @JvmStatic external fun asInt(handle: Long): Int
    @JvmStatic external fun asLong(handle: Long): Long
    @JvmStatic external fun asDouble(handle: Long): Double
    @JvmStatic external fun asBoolean(handle: Long): Boolean

    @JvmStatic external fun objectKeys(handle: Long): kotlin.Array<String>
    @JvmStatic external fun objectGet(handle: Long, name: String): Long
    @JvmStatic external fun objectSet(handle: Long, name: String, valueHandle: Long)
    
    @JvmStatic external fun arrayGetSize(handle: Long): Int
    @JvmStatic external fun arrayGet(handle: Long, index: Int): Long
    @JvmStatic external fun arraySet(handle: Long, index: Int, valueHandle: Long)
    @JvmStatic external fun arrayPush(handle: Long, valueHandle: Long)
    
    @JvmStatic external fun functionCall(handle: Long, args: LongArray): Long
}

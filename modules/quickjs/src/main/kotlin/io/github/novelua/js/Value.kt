package io.github.novelua.js

open class Value(
    /**
     * Property context.
     */
    val context: Context,
    internal val nativeHandle: Long
) : AutoCloseable {

    /**
     * Property isNull.
     */
    val isNull: Boolean get() = QuickJSNative.isNull(nativeHandle)
    /**
     * Property isUndefined.
     */
    val isUndefined: Boolean get() = QuickJSNative.isUndefined(nativeHandle)
    /**
     * Property isNumber.
     */
    val isNumber: Boolean get() = QuickJSNative.isNumber(nativeHandle)
    /**
     * Property isString.
     */
    val isString: Boolean get() = QuickJSNative.isString(nativeHandle)
    /**
     * Property isBoolean.
     */
    val isBoolean: Boolean get() = QuickJSNative.isBoolean(nativeHandle)
    /**
     * Property isObject.
     */
    val isObject: Boolean get() = QuickJSNative.isObject(nativeHandle)
    /**
     * Property isArray.
     */
    val isArray: Boolean get() = QuickJSNative.isArray(nativeHandle)
    /**
     * Property isFunction.
     */
    val isFunction: Boolean get() = QuickJSNative.isFunction(nativeHandle)

    /**
     * Executes asString.
     */
    fun asString(): String = QuickJSNative.asString(nativeHandle)
    /**
     * Executes asInt.
     */
    fun asInt(): Int = QuickJSNative.asInt(nativeHandle)
    /**
     * Executes asLong.
     */
    fun asLong(): Long = QuickJSNative.asLong(nativeHandle)
    /**
     * Executes asDouble.
     */
    fun asDouble(): Double = QuickJSNative.asDouble(nativeHandle)
    /**
     * Executes asBoolean.
     */
    fun asBoolean(): Boolean = QuickJSNative.asBoolean(nativeHandle)

    override fun close() {
        if (nativeHandle != 0L) {
            QuickJSNative.destroyValue(nativeHandle)
        }
    }
}

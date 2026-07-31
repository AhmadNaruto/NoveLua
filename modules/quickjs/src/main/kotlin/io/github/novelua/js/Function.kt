package io.github.novelua.js

/**
 * Represents Function.
 */
class Function(context: Context, nativeHandle: Long) : Object(context, nativeHandle) {
    /**
     * Executes call.
     */
    fun call(vararg args: Any?): Value {
        /**
         * Property jsArgs.
         */
        val jsArgs = args.map { context.toValue(it) }
        /**
         * Property handles.
         */
        val handles = LongArray(jsArgs.size) { jsArgs[it].nativeHandle }
        try {
            /**
             * Property valHandle.
             */
            val valHandle = QuickJSNative.functionCall(nativeHandle, handles)
            return context.wrapValue(valHandle)
        } finally {
            for (i in args.indices) {
                if (args[i] !is Value) {
                    jsArgs[i].close()
                }
            }
        }
    }
}

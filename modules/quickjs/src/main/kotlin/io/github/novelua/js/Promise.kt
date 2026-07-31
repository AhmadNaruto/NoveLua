package io.github.novelua.js

/**
 * Represents Promise.
 */
class Promise(context: Context, nativeHandle: Long) : Object(context, nativeHandle) {
    /**
     * Executes then.
     */
    fun then(callback: Function) {
        /**
         * Property thenFunc.
         */
        val thenFunc = get("then") as Function
        thenFunc.call(callback)
        thenFunc.close()
    }

    /**
     * Executes catch.
     */
    fun catch(callback: Function) {
        /**
         * Property catchFunc.
         */
        val catchFunc = get("catch") as Function
        catchFunc.call(callback)
        catchFunc.close()
    }
}

package io.github.novela.luau

import java.nio.ByteBuffer
import java.nio.ByteOrder

class LuaState internal constructor(
    internal val nativeHandle: Long,
    val isMainThread: Boolean = false
) : AutoCloseable {

    private var isClosed = false

    override fun close() {
        if (!isClosed) {
            if (isMainThread) {
                nativeClose(nativeHandle)
            }
            isClosed = true
        }
    }

    // Stack operations
    fun getTop(): Int = nativeGetTop(nativeHandle)
    fun setTop(idx: Int) = nativeSetTop(nativeHandle, idx)
    fun pop(n: Int) = setTop(-n - 1)
    fun pushValue(idx: Int) = nativePushValue(nativeHandle, idx)
    fun remove(idx: Int) = nativeRemove(nativeHandle, idx)
    fun insert(idx: Int) = nativeInsert(nativeHandle, idx)
    fun replace(idx: Int) = nativeReplace(nativeHandle, idx)
    fun absIndex(idx: Int): Int = nativeAbsIndex(nativeHandle, idx)

    // Pushing values
    fun pushNil() = nativePushNil(nativeHandle)
    fun pushBoolean(b: Boolean) = nativePushBoolean(nativeHandle, b)
    fun pushNumber(n: Double) = nativePushNumber(nativeHandle, n)
    fun pushInteger(i: Long) = nativePushInteger(nativeHandle, i)
    fun pushString(s: String) = nativePushString(nativeHandle, s)
    fun pushVector(x: Float, y: Float, z: Float) = nativePushVector(nativeHandle, x, y, z)
    fun pushLightUserData(ptr: Long) = nativePushLightUserData(nativeHandle, ptr)

    fun newUserData(value: Any, tag: Int = 0) {
        nativeNewUserData(nativeHandle, value, tag)
    }

    fun newBuffer(size: Int): ByteBuffer {
        val buf = nativeNewBuffer(nativeHandle, size.toLong())
        checkException()
        return buf.order(ByteOrder.nativeOrder())
    }

    // Types & inspection
    fun type(idx: Int): LuaType = LuaType.byId(nativeType(nativeHandle, idx))
    fun typeName(idx: Int): String = nativeTypeName(nativeHandle, idx)
    
    fun isNil(idx: Int): Boolean = type(idx) == LuaType.NIL
    fun isBoolean(idx: Int): Boolean = type(idx) == LuaType.BOOLEAN
    fun isNumber(idx: Int): Boolean = type(idx) == LuaType.NUMBER
    fun isString(idx: Int): Boolean = type(idx) == LuaType.STRING
    fun isTable(idx: Int): Boolean = type(idx) == LuaType.TABLE
    fun isFunction(idx: Int): Boolean = type(idx) == LuaType.FUNCTION
    fun isThread(idx: Int): Boolean = type(idx) == LuaType.THREAD
    fun isUserData(idx: Int): Boolean = type(idx) == LuaType.USERDATA
    fun isLightUserData(idx: Int): Boolean = type(idx) == LuaType.LIGHTUSERDATA

    // Conversions
    fun toBoolean(idx: Int): Boolean = nativeToBoolean(nativeHandle, idx)
    fun toNumber(idx: Int): Double = nativeToNumber(nativeHandle, idx)
    fun toInteger(idx: Int): Long = nativeToInteger(nativeHandle, idx)
    fun toString(idx: Int): String? = nativeToString(nativeHandle, idx)
    fun toVector(idx: Int): FloatArray = nativeToVector(nativeHandle, idx)
    fun toUserdata(idx: Int): Any? = nativeToUserdata(nativeHandle, idx)
    fun userDataTag(idx: Int): Int = nativeUserDataTag(nativeHandle, idx)
    fun toBuffer(idx: Int): ByteBuffer = nativeToBuffer(nativeHandle, idx).order(ByteOrder.nativeOrder())

    // Comparisons
    fun equal(idx1: Int, idx2: Int): Boolean {
        val res = nativeEqual(nativeHandle, idx1, idx2)
        checkException()
        return res
    }

    fun rawEqual(idx1: Int, idx2: Int): Boolean = nativeRawEqual(nativeHandle, idx1, idx2)

    fun lessThan(idx1: Int, idx2: Int): Boolean {
        val res = nativeLessThan(nativeHandle, idx1, idx2)
        checkException()
        return res
    }

    fun objLen(idx: Int): Int {
        val res = nativeObjLen(nativeHandle, idx)
        checkException()
        return res
    }

    // Tables
    fun createTable(narr: Int, nrec: Int) {
        nativeCreateTable(nativeHandle, narr, nrec)
        checkException()
    }
    fun newTable() = createTable(0, 0)

    fun getTable(idx: Int): LuaType {
        val t = nativeGetTable(nativeHandle, idx)
        checkException()
        return LuaType.byId(t)
    }

    fun getField(idx: Int, k: String): LuaType {
        val t = nativeGetField(nativeHandle, idx, k)
        checkException()
        return LuaType.byId(t)
    }

    fun rawGet(idx: Int): LuaType = LuaType.byId(nativeRawGet(nativeHandle, idx))
    fun rawGetI(idx: Int, n: Int): LuaType = LuaType.byId(nativeRawGetI(nativeHandle, idx, n))

    fun setTable(idx: Int) {
        nativeSetTable(nativeHandle, idx)
        checkException()
    }

    fun setField(idx: Int, k: String) {
        nativeSetField(nativeHandle, idx, k)
        checkException()
    }

    fun rawSet(idx: Int) {
        nativeRawSet(nativeHandle, idx)
        checkException()
    }

    fun rawSetI(idx: Int, n: Int) {
        nativeRawSetI(nativeHandle, idx, n)
        checkException()
    }

    fun getReadOnly(idx: Int): Boolean = nativeGetReadOnly(nativeHandle, idx)
    fun setReadOnly(idx: Int, enabled: Boolean) = nativeSetReadOnly(nativeHandle, idx, enabled)

    fun getMetaTable(idx: Int): Boolean = nativeGetMetaTable(nativeHandle, idx)
    fun setMetaTable(idx: Int) = nativeSetMetaTable(nativeHandle, idx)

    // Table API helpers
    internal fun getTableVal(table: LuaTable, key: Any?): LuaValue {
        pushReference(table.reference)
        push(key)
        getTable(-2)
        val value = toValue(-1)
        pop(2)
        return value
    }

    internal fun setTableVal(table: LuaTable, key: Any?, value: Any?) {
        pushReference(table.reference)
        push(key)
        push(value)
        setTable(-3)
        pop(1)
    }

    // Callbacks & references
    fun pushFunction(callback: LuaFunction, debugName: String = "kotlin_closure") {
        if (callback is LuaYieldableFunction) {
            nativePushYieldableFunction(nativeHandle, callback, debugName)
        } else {
            nativePushFunction(nativeHandle, callback, debugName)
        }
    }

    fun popReference(): LuaReference {
        val refId = nativeRef(nativeHandle, -1)
        pop(1)
        return LuaReference(this, refId)
    }

    fun pushReference(ref: LuaReference) {
        nativeGetRef(nativeHandle, ref.nativeRef)
    }

    fun unref(refId: Int) {
        nativeUnref(nativeHandle, refId)
    }

    // Value mapping
    fun push(value: Any?) {
        when (value) {
            null -> pushNil()
            is Boolean -> pushBoolean(value)
            is Double -> pushNumber(value)
            is Float -> pushNumber(value.toDouble())
            is Long -> pushInteger(value)
            is Int -> pushInteger(value.toLong())
            is String -> pushString(value)
            is LuaValue.Table -> pushReference(value.table.reference)
            is LuaValue.Function -> pushReference(value.reference)
            is LuaValue.Thread -> pushReference(value.thread.reference)
            is LuaTable -> pushReference(value.reference)
            is LuaThread -> pushReference(value.reference)
            is LuaValue -> pushLuaValue(value)
            is FloatArray -> {
                if (value.size == 3) pushVector(value[0], value[1], value[2])
                else throw IllegalArgumentException("FloatArray must have size 3 to be pushed as vector")
            }
            is ByteBuffer -> {
                // We should pass buffer size or address
                // Simply push as userdata or custom buffer if needed
                throw UnsupportedOperationException("Direct push of ByteBuffer not supported yet. Use newBuffer.")
            }
            is LuaFunction -> pushFunction(value)
            else -> newUserData(value)
        }
    }

    private fun pushLuaValue(v: LuaValue) {
        when (v) {
            is LuaValue.Nil -> pushNil()
            is LuaValue.Boolean -> pushBoolean(v.value)
            is LuaValue.Number -> pushNumber(v.value)
            is LuaValue.Integer -> pushInteger(v.value)
            is LuaValue.String -> pushString(v.value)
            is LuaValue.Table -> pushReference(v.table.reference)
            is LuaValue.Function -> pushReference(v.reference)
            is LuaValue.Thread -> pushReference(v.thread.reference)
            is LuaValue.Userdata -> newUserData(v.value, v.tag)
            is LuaValue.Buffer -> {
                // Pushing buffer not yet supported natively without allocation
                throw UnsupportedOperationException("Direct push of LuaValue.Buffer not supported yet. Use newBuffer.")
            }
            is LuaValue.Vector -> pushVector(v.x, v.y, v.z)
        }
    }

    fun toValue(idx: Int): LuaValue {
        return when (type(idx)) {
            LuaType.NIL -> LuaValue.Nil
            LuaType.BOOLEAN -> LuaValue.Boolean(toBoolean(idx))
            LuaType.NUMBER -> LuaValue.Number(toNumber(idx))
            LuaType.INTEGER -> LuaValue.Integer(toInteger(idx))
            LuaType.STRING -> LuaValue.String(toString(idx) ?: "")
            LuaType.TABLE -> {
                pushValue(idx)
                val ref = popReference()
                LuaValue.Table(LuaTable(this, ref))
            }
            LuaType.FUNCTION -> {
                pushValue(idx)
                val ref = popReference()
                LuaValue.Function(this, ref)
            }
            LuaType.THREAD -> {
                pushValue(idx)
                val ref = popReference()
                LuaValue.Thread(LuaThread(this, ref))
            }
            LuaType.USERDATA -> {
                val udata = toUserdata(idx)
                val tag = userDataTag(idx)
                LuaValue.Userdata(udata ?: Unit, tag)
            }
            LuaType.BUFFER -> LuaValue.Buffer(toBuffer(idx))
            LuaType.VECTOR -> {
                val coords = toVector(idx)
                LuaValue.Vector(coords[0], coords[1], coords[2])
            }
            else -> LuaValue.Nil
        }
    }

    // Call execution
    fun load(chunkName: String, bytecode: ByteArray) {
        val res = nativeLoad(nativeHandle, chunkName, bytecode)
        if (res != 0) {
            throw LuaException("Failed to load bytecode: status $res", res)
        }
    }

    fun call(nargs: Int, nresults: Int) {
        val res = nativeCall(nativeHandle, nargs, nresults)
        val status = nativeGetStatus(nativeHandle)
        if (status != 0) {
            propagateExceptionInner(LuaStatus.byId(status))
        } else if (res != 0) {
            propagateExceptionInner(LuaStatus.byId(res))
        }
    }

    fun callFunction(func: LuaValue.Function, args: Array<out Any?>): Array<LuaValue> {
        val startTop = getTop()
        pushReference(func.reference)
        for (arg in args) {
            push(arg)
        }
        call(args.size, -1) // -1 means LUA_MULTRET (returns all results)
        val numResults = getTop() - startTop
        val results = Array(numResults) { i ->
            toValue(startTop + 1 + i)
        }
        setTop(startTop)
        return results
    }

    // Yield & Resume
    fun yield(nresults: Int): Int {
        val res = nativeYield(nativeHandle, nresults)
        checkException()
        return res
    }

    fun resume(from: LuaState?, nargs: Int): LuaStatus {
        val status = nativeResume(nativeHandle, from?.nativeHandle ?: 0L, nargs)
        checkException()
        return LuaStatus.byId(status)
    }

    fun status(): LuaStatus = LuaStatus.byId(nativeStatus(nativeHandle))
    fun isYieldable(): Boolean = nativeIsYieldable(nativeHandle)

    // GC & Memory
    fun gc(op: LuaGcOp, data: Int): Int {
        val res = nativeGc(nativeHandle, op.id(), data)
        checkException()
        return res
    }

    fun totalBytes(category: Int): Long = nativeTotalBytes(nativeHandle, category)

    // Sandboxing
    fun sandbox(thread: LuaState) = nativeSandbox(nativeHandle, thread.nativeHandle)
    fun sandboxThread() = nativeSandboxThread(nativeHandle)

    // Error checking
    private fun checkException() {
        val status = nativeGetStatus(nativeHandle)
        if (status != 0) {
            propagateExceptionInner(LuaStatus.byId(status))
        }
    }

    internal fun propagateExceptionInner(status: LuaStatus) {
        val rawMsg = toString(-1) ?: "Unknown Luau error"
        pop(1) // pop the error message

        // Parse traceback lines if present
        val lines = rawMsg.split("\n")
        val message = lines.firstOrNull() ?: rawMsg

        val luaFrames = mutableListOf<StackTraceElement>()
        // Matcher for: "[string \"chunkName\"]:line function funcName" or "file.lua:line"
        val regex = """(?:\[string "([^"]+)"\]|([^:]+)):(\d+)(?: function (\S+))?""".toRegex()
        for (line in lines) {
            val match = regex.find(line) ?: continue
            val chunkName = if (match.groupValues[1].isNotEmpty()) match.groupValues[1] else match.groupValues[2]
            val lineNumber = match.groupValues[3].toInt()
            val funcName = if (match.groupValues[4].isNotEmpty()) match.groupValues[4] else "main"
            luaFrames.add(StackTraceElement("lua", funcName, chunkName, lineNumber))
        }

        val ex = LuaException(message, status.id())
        if (luaFrames.isNotEmpty()) {
            val javaFrames = ex.stackTrace
            val merged = Array(luaFrames.size + javaFrames.size) { i ->
                if (i < luaFrames.size) luaFrames[i] else javaFrames[i - luaFrames.size]
            }
            ex.stackTrace = merged
        }
        throw ex
    }

    companion object {
        const val REGISTRY_INDEX = -10000
        const val GLOBALS_INDEX = -10002

        init {
            System.loadLibrary("novelua")
        }

        @JvmStatic
        fun newThread(L: LuaState): LuaState {
            val threadPtr = nativeNewThread(L.nativeHandle)
            L.checkException()
            return LuaState(threadPtr, false)
        }

        // Native lifecycle
        @JvmStatic private external fun nativeClose(statePtr: Long)
        @JvmStatic private external fun nativeNewThread(statePtr: Long): Long

        // Native stack operations
        @JvmStatic private external fun nativeGetTop(statePtr: Long): Int
        @JvmStatic private external fun nativeSetTop(statePtr: Long, idx: Int)
        @JvmStatic private external fun nativePushValue(statePtr: Long, idx: Int)
        @JvmStatic private external fun nativeRemove(statePtr: Long, idx: Int)
        @JvmStatic private external fun nativeInsert(statePtr: Long, idx: Int)
        @JvmStatic private external fun nativeReplace(statePtr: Long, idx: Int)
        @JvmStatic private external fun nativeAbsIndex(statePtr: Long, idx: Int): Int

        // Native pushing
        @JvmStatic private external fun nativePushNil(statePtr: Long)
        @JvmStatic private external fun nativePushBoolean(statePtr: Long, b: Boolean)
        @JvmStatic private external fun nativePushNumber(statePtr: Long, n: Double)
        @JvmStatic private external fun nativePushInteger(statePtr: Long, i: Long)
        @JvmStatic private external fun nativePushString(statePtr: Long, s: String)
        @JvmStatic private external fun nativePushVector(statePtr: Long, x: Float, y: Float, z: Float)
        @JvmStatic private external fun nativePushLightUserData(statePtr: Long, ptr: Long)
        @JvmStatic private external fun nativeNewUserData(statePtr: Long, value: Any, tag: Int)
        @JvmStatic private external fun nativeNewBuffer(statePtr: Long, size: Long): ByteBuffer

        // Native types & conversions
        @JvmStatic private external fun nativeType(statePtr: Long, idx: Int): Int
        @JvmStatic private external fun nativeTypeName(statePtr: Long, idx: Int): String
        @JvmStatic private external fun nativeToBoolean(statePtr: Long, idx: Int): Boolean
        @JvmStatic private external fun nativeToNumber(statePtr: Long, idx: Int): Double
        @JvmStatic private external fun nativeToInteger(statePtr: Long, idx: Int): Long
        @JvmStatic private external fun nativeToString(statePtr: Long, idx: Int): String?
        @JvmStatic private external fun nativeToVector(statePtr: Long, idx: Int): FloatArray
        @JvmStatic private external fun nativeToUserdata(statePtr: Long, idx: Int): Any?
        @JvmStatic private external fun nativeUserDataTag(statePtr: Long, idx: Int): Int
        @JvmStatic private external fun nativeToBuffer(statePtr: Long, idx: Int): ByteBuffer

        // Comparisons
        @JvmStatic private external fun nativeEqual(statePtr: Long, idx1: Int, idx2: Int): Boolean
        @JvmStatic private external fun nativeRawEqual(statePtr: Long, idx1: Int, idx2: Int): Boolean
        @JvmStatic private external fun nativeLessThan(statePtr: Long, idx1: Int, idx2: Int): Boolean
        @JvmStatic private external fun nativeObjLen(statePtr: Long, idx: Int): Int

        // Tables
        @JvmStatic private external fun nativeCreateTable(statePtr: Long, narr: Int, nrec: Int)
        @JvmStatic private external fun nativeGetTable(statePtr: Long, idx: Int): Int
        @JvmStatic private external fun nativeGetField(statePtr: Long, idx: Int, k: String): Int
        @JvmStatic private external fun nativeRawGet(statePtr: Long, idx: Int): Int
        @JvmStatic private external fun nativeRawGetI(statePtr: Long, idx: Int, n: Int): Int
        @JvmStatic private external fun nativeSetTable(statePtr: Long, idx: Int)
        @JvmStatic private external fun nativeSetField(statePtr: Long, idx: Int, k: String)
        @JvmStatic private external fun nativeRawSet(statePtr: Long, idx: Int)
        @JvmStatic private external fun nativeRawSetI(statePtr: Long, idx: Int, n: Int)
        @JvmStatic private external fun nativeGetReadOnly(statePtr: Long, idx: Int): Boolean
        @JvmStatic private external fun nativeSetReadOnly(statePtr: Long, idx: Int, enabled: Boolean)
        @JvmStatic private external fun nativeGetMetaTable(statePtr: Long, idx: Int): Boolean
        @JvmStatic private external fun nativeSetMetaTable(statePtr: Long, idx: Int)

        // Callback registrations
        @JvmStatic private external fun nativePushFunction(statePtr: Long, callback: LuaFunction, debugName: String)
        @JvmStatic private external fun nativePushYieldableFunction(statePtr: Long, callback: LuaYieldableFunction, debugName: String)
        
        // Registry helpers
        @JvmStatic private external fun nativeRef(statePtr: Long, idx: Int): Int
        @JvmStatic private external fun nativeGetRef(statePtr: Long, refId: Int)
        @JvmStatic private external fun nativeUnref(statePtr: Long, refId: Int)

        // Call, Load, Yield, Resume
        @JvmStatic private external fun nativeLoad(statePtr: Long, chunkName: String, bytecode: ByteArray): Int
        @JvmStatic private external fun nativeCall(statePtr: Long, nargs: Int, nresults: Int): Int
        @JvmStatic private external fun nativeYield(statePtr: Long, nresults: Int): Int
        @JvmStatic private external fun nativeResume(statePtr: Long, fromPtr: Long, nargs: Int): Int
        @JvmStatic private external fun nativeStatus(statePtr: Long): Int
        @JvmStatic private external fun nativeIsYieldable(statePtr: Long): Boolean

        // GC & Memory
        @JvmStatic private external fun nativeGc(statePtr: Long, op: Int, data: Int): Int
        @JvmStatic private external fun nativeTotalBytes(statePtr: Long, category: Int): Long

        // Sandboxing
        @JvmStatic private external fun nativeSandbox(statePtr: Long, threadPtr: Long)
        @JvmStatic private external fun nativeSandboxThread(statePtr: Long)

        // Error checking helper
        @JvmStatic private external fun nativeGetStatus(statePtr: Long): Int
        @JvmStatic private external fun nativePop(statePtr: Long, n: Int)
    }
}

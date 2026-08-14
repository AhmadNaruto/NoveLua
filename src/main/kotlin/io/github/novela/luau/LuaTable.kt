package io.github.novela.luau

class LuaTable(
    val state: LuaState,
    val reference: LuaReference
) : AutoCloseable {

    fun get(key: Any?): LuaValue = state.getTableVal(this, key)
    fun set(key: Any?, value: Any?) = state.setTableVal(this, key, value)

    fun getString(key: Any?): String? = (get(key) as? LuaValue.String)?.value
    fun getLong(key: Any?): Long? = when (val v = get(key)) {
        is LuaValue.Integer -> v.value
        is LuaValue.Number -> v.value.toLong()
        else -> null
    }
    fun getDouble(key: Any?): Double? = when (val v = get(key)) {
        is LuaValue.Number -> v.value
        is LuaValue.Integer -> v.value.toDouble()
        else -> null
    }
    fun getBoolean(key: Any?): Boolean? = (get(key) as? LuaValue.Boolean)?.value
    fun getTable(key: Any?): LuaTable? = (get(key) as? LuaValue.Table)?.table
    fun getFunction(key: Any?): LuaValue.Function? = get(key) as? LuaValue.Function

    override fun close() {
        reference.close()
    }

    override fun toString(): String {
        return "table: 0x${java.lang.Long.toHexString(reference.nativeRef.toLong())}"
    }
}

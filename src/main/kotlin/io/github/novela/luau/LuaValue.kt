package io.github.novela.luau

import java.nio.ByteBuffer

sealed class LuaValue {
    object Nil : LuaValue() {
        override fun toString(): kotlin.String = "nil"
    }

    data class Boolean(val value: kotlin.Boolean) : LuaValue() {
        override fun toString(): kotlin.String = value.toString()
    }

    data class Number(val value: Double) : LuaValue() {
        override fun toString(): kotlin.String = value.toString()
    }

    data class Integer(val value: Long) : LuaValue() {
        override fun toString(): kotlin.String = value.toString()
    }

    data class String(val value: kotlin.String) : LuaValue() {
        override fun toString(): kotlin.String = value
    }

    class Table(val table: LuaTable) : LuaValue() {
        fun get(key: Any?): LuaValue = table.get(key)
        fun set(key: Any?, value: Any?) = table.set(key, value)
        override fun toString(): kotlin.String = table.toString()
    }

    class Function(val state: LuaState, val reference: LuaReference) : LuaValue() {
        fun call(vararg args: Any?): Array<LuaValue> = state.callFunction(this, args)
        override fun toString(): kotlin.String = "function: 0x${java.lang.Long.toHexString(reference.nativeRef.toLong())}"
    }

    class Thread(val thread: LuaThread) : LuaValue() {
        override fun toString(): kotlin.String = thread.toString()
    }

    data class Userdata(val value: Any, val tag: Int) : LuaValue() {
        override fun toString(): kotlin.String = "userdata: $value (tag=$tag)"
    }

    class Buffer(val value: ByteBuffer) : LuaValue() {
        override fun toString(): kotlin.String = "buffer: size=${value.capacity()}"
    }

    data class Vector(val x: Float, val y: Float, val z: Float) : LuaValue() {
        override fun toString(): kotlin.String = "vector($x, $y, $z)"
    }
}

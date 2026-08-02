package io.github.novelua.luau

/**
 * Represents Userdata.
 */
class Userdata internal constructor(internal var nativeHandle: Long) {
    val typeName: String = "userdata"
    fun unwrap(): Any? = null
}

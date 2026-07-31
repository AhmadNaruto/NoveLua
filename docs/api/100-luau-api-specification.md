# 100-luau-api-specification.md

# Luau API Specification

Version: 1.0

Package

io.github.novelua.luau

Native Library

Luau

---

# VM

Description

Represents a Luau Virtual Machine.

Properties

- globals: Table
- registry: Table
- isClosed: Boolean

Functions

- execute(script: String)
- execute(script: ByteArray)
- compile(script: String): ByteArray
- load(bytecode: ByteArray)
- eval(script: String): Any?
- gc()
- close()

- globals(): Table
- registry(): Table

- createTable(): Table
- createThread(): Thread

- getGlobal(name: String): Any?
- setGlobal(name: String, value: Any?)

- registerLibrary(library: Library)
- registerModule(module: Module)

---

# Compiler

Description

Compiles Luau source code.

Functions

- compile(source: String): ByteArray
- compile(source: ByteArray): ByteArray

---

# Table

Description

Represents a Lua table.

Properties

- size: Int

Functions

- get(key: Any): Any?
- set(key: Any, value: Any?)
- remove(key: Any)
- clear()

- contains(key: Any): Boolean

- keys(): List<Any>
- values(): List<Any>

---

# Function

Description

Represents a Lua function.

Functions

- call(vararg args: Any?): Any?
- invoke(vararg args: Any?): Any?

---

# Thread

Description

Represents a Luau coroutine.

Properties

- status: String

Functions

- resume(vararg args: Any?)
- yield()
- close()

---

# Userdata

Description

Represents native userdata.

Properties

- typeName: String

Functions

- unwrap(): Any?

---

# Library

Description

Represents a native library.

Properties

- name: String

Functions

- register(vm: VM)

---

# Module

Description

Represents a Lua module.

Properties

- name: String

Functions

- load(vm: VM)

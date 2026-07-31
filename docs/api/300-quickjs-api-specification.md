# 300-quickjs-api-specification.md

# QuickJS API Specification

Version: 1.0

Package

io.github.novelua.js

Native Library

QuickJS

---

# Runtime

Functions

- createContext(): Context
- gc()
- close()

---

# Context

Functions

- eval(script: String): Value
- evalModule(script: String): Value

- getGlobal(name: String): Value
- setGlobal(name: String, value: Any?)

- close()

---

# Value

Properties

- isNull: Boolean
- isUndefined: Boolean
- isNumber: Boolean
- isString: Boolean
- isBoolean: Boolean
- isObject: Boolean
- isArray: Boolean
- isFunction: Boolean

Functions

- asString(): String
- asInt(): Int
- asLong(): Long
- asDouble(): Double
- asBoolean(): Boolean

---

# Object

Functions

- get(name: String): Value
- set(name: String, value: Any?)
- keys(): List<String>

---

# Array

Properties

- size: Int

Functions

- get(index: Int): Value
- set(index: Int, value: Any?)
- push(value: Any?)

---

# Function

Functions

- call(vararg args: Any?): Value

---

# Promise

Functions

- then(callback: Function)
- catch(callback: Function)

---

# Module

Functions

- load(name: String)

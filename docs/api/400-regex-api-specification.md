# 400-regex-api-specification.md

# Regex API Specification

Version: 1.0

Package

io.github.novelua.regex

Native Library

PCRE2

---

# Regex

Functions

- compile(pattern: String): Pattern

---

# Pattern

Properties

- pattern: String

Functions

- matcher(input: String): Matcher

- matches(input: String): Boolean
- find(input: String): Boolean

- replace(input: String, replacement: String): String

- split(input: String): List<String>

---

# Matcher

Functions

- matches(): Boolean
- find(): Boolean

- group(index: Int): String

- start(): Int
- end(): Int

- reset()

---

# Match

Properties

- value: String
- start: Int
- end: Int

---

# Group

Properties

- index: Int
- value: String

---

# Replace

Functions

- replaceFirst(input: String, replacement: String): String
- replaceAll(input: String, replacement: String): String

---

# Split

Functions

- split(input: String): List<String>

---

# Options

Properties

- ignoreCase: Boolean
- multiline: Boolean
- dotAll: Boolean
- unicode: Boolean

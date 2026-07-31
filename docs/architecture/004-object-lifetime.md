# Object Lifetime

Semua object memiliki owner yang jelas.

---

## Luau

```
VM

↓

Table

↓

Function

↓

Thread
```

VM memiliki seluruh object.

---

## QuickJS

```
Runtime

↓

Context

↓

Value
```

Runtime memiliki seluruh Context.

Context memiliki seluruh Value.

---

## LexSoup

```
Parser

↓

Document

↓

Element

↓

Elements
```

Document memiliki seluruh node.

Element tidak memiliki Document.

---

## Regex

```
Regex

↓

Match
```

Regex memiliki Match.

---

Tidak boleh ada ownership ambiguity.

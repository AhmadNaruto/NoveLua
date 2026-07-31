# 200-lexsoup-api-specification.md

# LexSoup API Specification

Version: 1.0

Package

io.github.novelua.lexsoup

Native Library

Lexbor

---

# Parser

Functions

- parse(html: String): Document
- parse(file: File): Document
- parse(url: String): Document
- parseFragment(html: String): Elements

---

# Document

Properties

- title: String
- head: Element
- body: Element

Functions

- html(): String
- outerHtml(): String
- text(): String

- select(css: String): Elements
- selectFirst(css: String): Element?

- createElement(tag: String): Element

- body(): Element
- head(): Element

---

# Element

Properties

- tagName: String
- text: String
- html: String
- outerHtml: String
- id: String
- className: String
- parent: Element?

Functions

- select(css: String): Elements
- selectFirst(css: String): Element?

- attr(key: String): String
- attr(key: String, value: String): Element

- hasAttr(key: String): Boolean
- removeAttr(key: String)

- append(html: String): Element
- prepend(html: String): Element

- before(html: String): Element
- after(html: String): Element

- appendChild(child: Element): Element
- remove()

- empty()

- children(): Elements
- child(index: Int): Element

- parent(): Element?
- next(): Element?
- previous(): Element?

---

# Elements

Properties

- size: Int

Functions

- first(): Element?
- last(): Element?
- get(index: Int): Element

- text(): String
- html(): String

- attr(key: String): String

- remove()
- empty()

---

# Node

Functions

- nodeName(): String
- outerHtml(): String

- parent(): Node?
- childNodes(): List<Node>

- remove()

---

# TextNode

Properties

- text: String

Functions

- setText(text: String)

---

# Comment

Properties

- data: String

---

# DocumentType

Properties

- name: String
- publicId: String
- systemId: String

---

# Attribute

Properties

- key: String
- value: String

---

# Attributes

Functions

- get(key: String): String
- put(key: String, value: String)
- remove(key: String)
- hasKey(key: String): Boolean

---

# Selector

Functions

- select(root: Element, css: String): Elements

---

# OutputSettings

Properties

- prettyPrint: Boolean
- indentAmount: Int
- charset: String

---

# Entities

Functions

- escape(text: String): String
- unescape(text: String): String

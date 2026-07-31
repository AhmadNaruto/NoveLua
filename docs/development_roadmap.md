# Novelua Native SDK — Master Implementation Roadmap & Progress Tracking Document

**Document Version:** 1.0.0  
**Target ABI:** `arm64-v8a`  
**Languages:** Kotlin, C++20, C17, JNI, CMake, Gradle (Kotlin DSL)  
**Status:** Approved Architecture Plan  

---

## 1. Project Overview

### 1.1 Project Objective
The **Novelua Native SDK** is a modular, high-performance, native-first Android SDK providing lightweight and memory-safe Kotlin wrappers over four core third-party native C/C++ libraries:
1. **Luau** — Lightweight embeddable Lua variant engine & compiler (`io.github.novelua.luau`).
2. **Lexbor** (LexSoup) — High-speed HTML5 parser and DOM manipulator (`io.github.novelua.lexsoup`).
3. **QuickJS** — Embeddable JavaScript engine (`io.github.novelua.js`).
4. **PCRE2** (Regex) — Perl Compatible Regular Expressions library (`io.github.novelua.regex`).

### 1.2 Development Strategy
- **Native-First & Zero Magic:** No reflection, no annotation processing, no code generation toolchains, no auto-bind generators. All wrappers are manually crafted for performance and total API control.
- **Explicit JNI Bindings:** All JNI functions use manual `RegisterNatives` registration during `JNI_OnLoad`. Legacy automatic `Java_com_...` function exports are strictly prohibited.
- **Opaque Handle Lifetime Management:** Native objects are owned via C++ RAII containers and exposed to Kotlin as opaque `Long` pointers (`nativeHandle`). Kotlin classes implement `AutoCloseable` or `Closeable`.
- **Zero Cross-Module Native Dependencies:** Modules (`luau`, `lexsoup`, `quickjs`, `regex`) are completely decoupled at the native layer. Shared utilities exist exclusively in `native/common`.

### 1.3 Implementation Order Strategy
The project follows a **bottom-up foundational approach**:
1. **Infrastructure & Submodules:** Establish Git submodules, root Gradle version catalogs, top-level CMake configuration, and target toolchain verification for `arm64-v8a`.
2. **Native Common Layer:** Implement thread-safe JNI string conversion (`JString`), exception throwers (`ThrowException`), and local reference scoping helpers (`ScopedLocalRef`).
3. **Leaf Module 1 (`regex`):** Implement PCRE2 wrapper as the smallest, least stateful module to validate the JNI engine pipeline.
4. **Leaf Module 2 (`lexsoup`):** Implement Lexbor DOM parser and element manipulator.
5. **Leaf Module 3 (`quickjs`):** Implement QuickJS JavaScript execution context, value conversions, and objects.
6. **Composite Host Module 4 (`luau`):** Implement Luau VM, state management, table, coroutines/threads, functions, compiler, and host library/module registration.
7. **SDK Integration & Benchmarks:** End-to-end multi-module verification, stress testing, leak checking, and benchmarking.

### 1.4 Milestones & Expected Deliverables
- **M0: Core Foundation Ready** — Gradle catalog, CMake toolchain, Git submodules initialized, `native/common` JNI helpers compiled and tested.
- **M1: `regex` Module Delivered** — PCRE2 pattern compilation, matching, group extraction, replace, and split working via Kotlin API.
- **M2: `lexsoup` Module Delivered** — HTML document parsing, CSS selection, DOM mutations, node/element traversal fully functional.
- **M3: `quickjs` Module Delivered** — JavaScript runtime evaluation, global scope access, JS value mapping, and promise callbacks verified.
- **M4: `luau` Module Delivered** — Luau VM execution, bytecode compilation, table read/write, coroutines, and custom native library binding operational.
- **M5: SDK Production Release** — Full test coverage, zero memory leaks, zero compiler warnings, full KDoc documentation, and release AAR artifacts built for `arm64-v8a`.

---

## 2. Development Phases

```
Phase 1: Infra & Build System Setup
        │
        ▼
Phase 2: JNI Common Utility Layer
        │
        ▼
Phase 3: Regex Module (PCRE2 Wrapper)
        │
        ▼
Phase 4: LexSoup Module (Lexbor Wrapper)
        │
        ▼
Phase 5: QuickJS Module (QuickJS Wrapper)
        │
        ▼
Phase 6: Luau Module (Luau VM & Compiler Wrapper)
        │
        ▼
Phase 7: Integration, Verification & Release Packaging
```

### Phase 1: Repository Infrastructure & Build System
- **Objective:** Configure project workspace, initialize 4 third-party Git submodules, set up Gradle Version Catalog, CMake build scripts targeting `arm64-v8a`, and C++20/C17 compiler configurations.
- **Dependencies:** None.
- **Estimated Complexity:** Low
- **Expected Output:** Fully buildable Android Gradle multi-module project with CMake integrations.
- **Acceptance Criteria:** `gradlew assembleRelease` executes cleanly for `arm64-v8a` target; submodules checked out at pinned commit hashes.

### Phase 2: Native Common Utility Layer (`native/common`)
- **Objective:** Implement core JNI utilities shared across modules: string conversions (UTF-8 / UTF-16), `ScopedLocalRef`, exception throwers (`throwIllegalArgument`, `throwIllegalState`, `throwRuntime`), handle validation, and `JavaVM` cache.
- **Dependencies:** Phase 1
- **Estimated Complexity:** Medium
- **Expected Output:** Static native library `libnovelua_common.a` / shared header library accessible to all native modules.
- **Acceptance Criteria:** Unit tests covering native exception throwing, string allocation, and reference cleanup without local ref table overflow.

### Phase 3: `regex` Module (PCRE2 Wrapper)
- **Objective:** Build C++ wrapper and JNI bindings for PCRE2 matching, replacing, splitting, and pattern options.
- **Dependencies:** Phase 2
- **Estimated Complexity:** Medium
- **Expected Output:** `io.github.novelua.regex` package containing `Regex`, `Pattern`, `Matcher`, `Match`, `Group`, `Replace`, `Split`, `Options`.
- **Acceptance Criteria:** 100% API coverage matching `400-regex-api-specification.md`; zero PCRE2 memory leaks.

### Phase 4: `lexsoup` Module (Lexbor Wrapper)
- **Objective:** Wrap Lexbor HTML5 C parser library into C++ DOM models and register JNI functions to expose `Parser`, `Document`, `Element`, `Elements`, `Node`, `TextNode`, `Comment`, `DocumentType`, `Attribute`, `Attributes`, `Selector`, `OutputSettings`, `Entities`.
- **Dependencies:** Phase 2
- **Estimated Complexity:** High
- **Expected Output:** `io.github.novelua.lexsoup` package providing HTML parsing and CSS selector engine.
- **Acceptance Criteria:** Passes all HTML DOM mutation and CSS selection tests according to `200-lexsoup-api-specification.md`.

### Phase 5: `quickjs` Module (QuickJS Wrapper)
- **Objective:** Integrate QuickJS engine into C++ runtime context wrappers, handling JavaScript JSValue conversions, garbage collection, and promise callbacks.
- **Dependencies:** Phase 2
- **Estimated Complexity:** High
- **Expected Output:** `io.github.novelua.js` package containing `Runtime`, `Context`, `Value`, `Object`, `Array`, `Function`, `Promise`, `Module`.
- **Acceptance Criteria:** JS evaluation, object key-value access, array mutation, and promise resolution matching `300-quickjs-api-specification.md`.

### Phase 6: `luau` Module (Luau Wrapper)
- **Objective:** Wrap Luau VM (`lua_State`), Bytecode Compiler, Table management, Coroutine Threads, Functions, Userdata, and Host Library/Module binding system.
- **Dependencies:** Phase 2
- **Estimated Complexity:** High
- **Expected Output:** `io.github.novelua.luau` package with `VM`, `Compiler`, `Table`, `Function`, `Thread`, `Userdata`, `Library`, `Module`.
- **Acceptance Criteria:** Full execution of bytecode, global table manip, coroutine yield/resume, and native function registration per `100-luau-api-specification.md`.

### Phase 7: Integration, Verification & Release Packaging
- **Objective:** Execute multi-threaded stability tests, memory leak checks (AddressSanitizer / Valgrind), verify KDoc coverage, and package final AARs.
- **Dependencies:** Phases 3, 4, 5, 6
- **Estimated Complexity:** Medium
- **Expected Output:** Final published AAR artifacts and comprehensive test suites.
- **Acceptance Criteria:** All unit tests pass; zero ASan memory leak warnings; 100% KDoc compliance.

---

## 3. Task Breakdown

### Phase 1: Repository Infrastructure & Build System
- **Task 1.1:** Initialize Git submodules for `thirdparty/luau`, `thirdparty/quickjs`, `thirdparty/pcre2`, `thirdparty/lexbor`.
- **Task 1.2:** Configure Root Gradle Build Script and Version Catalog (`gradle/libs.versions.toml`).
- **Task 1.3:** Create Gradle module directories (`modules/regex`, `modules/lexsoup`, `modules/quickjs`, `modules/luau`).
- **Task 1.4:** Create Native source directories (`native/common`, `native/regex`, `native/lexsoup`, `native/quickjs`, `native/luau`).
- **Task 1.5:** Configure CMake infrastructure scripts in `cmake/` targeting C++20 (`-std=c++20`), C17 (`-std=c17`), and `arm64-v8a` ABI flags.

### Phase 2: Native Common Utility Layer
- **Task 2.1:** Implement `native/common/include/novelua/common/jni_env.hpp` (JavaVM global cache, safe `JNIEnv` retrieval per thread).
- **Task 2.2:** Implement `native/common/include/novelua/common/jstring_utils.hpp` & `.cpp` (UTF-8 string conversions, `JString` RAII wrapper).
- **Task 2.3:** Implement `native/common/include/novelua/common/scoped_local_ref.hpp` (Template-based local ref auto-delete).
- **Task 2.4:** Implement `native/common/include/novelua/common/exceptions.hpp` & `.cpp` (Native error to Kotlin exception mapping via JNI).
- **Task 2.5:** Setup common library CMake target `novelua_common` and unit test runner for JNI utilities.

### Phase 3: `regex` Module (PCRE2)
- **Task 3.1:** Create `native/regex/CMakeLists.txt` linking `pcre2-8` submodule static library.
- **Task 3.2:** Implement native C++ PCRE2 wrapper classes in `novelua::regex` namespace (`pcre2_engine.hpp/.cpp`, `pattern_wrapper.hpp/.cpp`).
- **Task 3.3:** Implement `regex_jni.cpp` registering native methods via `RegisterNatives`.
- **Task 3.4:** Implement Kotlin classes: `Regex`, `Pattern`, `Matcher`, `Match`, `Group`, `Replace`, `Split`, `Options` in `io.github.novelua.regex`.
- **Task 3.5:** Write Android instrumentation/unit tests covering pattern matching, regex replace, splitting, and unicode flag handling.

### Phase 4: `lexsoup` Module (Lexbor)
- **Task 4.1:** Create `native/lexsoup/CMakeLists.txt` compiling and linking Lexbor HTML & CSS modules.
- **Task 4.2:** Implement native C++ Lexbor wrappers in `novelua::lexsoup` (`document.hpp/.cpp`, `element.hpp/.cpp`, `selector.hpp/.cpp`).
- **Task 4.3:** Implement `lexsoup_jni.cpp` registering native functions for Document, Element, and Selector operations via `RegisterNatives`.
- **Task 4.4:** Implement Kotlin DOM classes: `Parser`, `Document`, `Element`, `Elements`, `Node`, `TextNode`, `Comment`, `DocumentType`, `Attribute`, `Attributes`, `Selector`, `OutputSettings`, `Entities` in `io.github.novelua.lexsoup`.
- **Task 4.5:** Write comprehensive HTML parsing, CSS selector query, and DOM manipulation unit tests.

### Phase 5: `quickjs` Module (QuickJS)
- **Task 5.1:** Create `native/quickjs/CMakeLists.txt` compiling QuickJS core C sources.
- **Task 5.2:** Implement native C++ QuickJS wrappers in `novelua::quickjs` (`runtime_wrapper.hpp/.cpp`, `context_wrapper.hpp/.cpp`, `value_wrapper.hpp/.cpp`).
- **Task 5.3:** Implement `quickjs_jni.cpp` mapping JNI functions to QuickJS C++ wrappers via `RegisterNatives`.
- **Task 5.4:** Implement Kotlin wrappers: `Runtime`, `Context`, `Value`, `Object`, `Array`, `Function`, `Promise`, `Module` in `io.github.novelua.js`.
- **Task 5.5:** Write unit tests for JS script evaluation, global variable modification, array operations, and asynchronous promise handling.

### Phase 6: `luau` Module (Luau)
- **Task 6.1:** Create `native/luau/CMakeLists.txt` building Luau Compiler, VM, and Ast modules from submodules.
- **Task 6.2:** Implement native C++ Luau wrappers in `novelua::luau` (`vm_wrapper.hpp/.cpp`, `compiler_wrapper.hpp/.cpp`, `table_wrapper.hpp/.cpp`).
- **Task 6.3:** Implement `luau_jni.cpp` registering native handles and methods via `RegisterNatives`.
- **Task 6.4:** Implement Kotlin classes: `VM`, `Compiler`, `Table`, `Function`, `Thread`, `Userdata`, `Library`, `Module` in `io.github.novelua.luau`.
- **Task 6.5:** Write unit tests for Luau script execution, bytecode compilation, table get/set, coroutine yield/resume, and custom native library loading.

### Phase 7: Integration, Verification & Documentation
- **Task 7.1:** Perform cross-module memory leak auditing using AddressSanitizer (ASan) and Valgrind on `arm64-v8a`.
- **Task 7.2:** Audit compiler warnings (`-Wall -Wextra -Werror`) across all native targets and Kotlin code (`-Werror`).
- **Task 7.3:** Verify 100% KDoc coverage for all public classes and methods across all 4 modules.
- **Task 7.4:** Validate strict adherence to `010-coding-standard.md` (naming conventions, package names, visibility, AutoCloseable usages).
- **Task 7.5:** Generate release AAR builds for all modules (`luau`, `lexsoup`, `quickjs`, `regex`).

---

## 4. Dependency Graph

```
Phase 1: Repository Infrastructure & Build System
  │ (Submodules, Version Catalog, CMake Toolchain arm64-v8a)
  ▼
Phase 2: Native Common Utility Layer (native/common)
  │ (JString, ScopedLocalRef, Exception Throwers, JavaVM Cache)
  ├───────────────────────┬───────────────────────┬───────────────────────┐
  ▼                       ▼                       ▼                       ▼
Phase 3: regex          Phase 4: lexsoup        Phase 5: quickjs        Phase 6: luau
(PCRE2 Wrapper)         (Lexbor Wrapper)        (QuickJS Wrapper)       (Luau VM Wrapper)
  │                       │                       │                       │
  └───────────────────────┴───────────────────────┴───────────────────────┘
                                  │
                                  ▼
                Phase 7: Integration, Testing & Packaging
```

---

## 5. Progress Checklist

### Phase 1: Repository Infrastructure & Build System
- [ ] **Task 1.1:** Add Git Submodule: `thirdparty/luau`
- [ ] **Task 1.1:** Add Git Submodule: `thirdparty/quickjs`
- [ ] **Task 1.1:** Add Git Submodule: `thirdparty/pcre2`
- [ ] **Task 1.1:** Add Git Submodule: `thirdparty/lexbor`
- [ ] **Task 1.2:** Configure Root `build.gradle.kts` & `settings.gradle.kts`
- [ ] **Task 1.2:** Populate `gradle/libs.versions.toml` with AGP, Kotlin, NDK version settings
- [ ] **Task 1.3:** Create Android Library Module: `modules/regex`
- [ ] **Task 1.3:** Create Android Library Module: `modules/lexsoup`
- [ ] **Task 1.3:** Create Android Library Module: `modules/quickjs`
- [ ] **Task 1.3:** Create Android Library Module: `modules/luau`
- [ ] **Task 1.4:** Create Directory: `native/common`
- [ ] **Task 1.4:** Create Directory: `native/regex`
- [ ] **Task 1.4:** Create Directory: `native/lexsoup`
- [ ] **Task 1.4:** Create Directory: `native/quickjs`
- [ ] **Task 1.4:** Create Directory: `native/luau`
- [ ] **Task 1.5:** Configure CMake files in `cmake/` targeting `arm64-v8a`, C++20, C17

### Phase 2: Native Common Utility Layer
- [ ] **Task 2.1:** Implement `jni_env.hpp` (JavaVM caching and thread-safe environment retrieval)
- [ ] **Task 2.2:** Implement `jstring_utils.hpp` and `jstring_utils.cpp` (RAII string conversions)
- [ ] **Task 2.3:** Implement `scoped_local_ref.hpp` (RAII local reference wrapper)
- [ ] **Task 2.4:** Implement `exceptions.hpp` and `exceptions.cpp` (JNI Exception throwing helpers)
- [ ] **Task 2.5:** Configure `CMakeLists.txt` for `novelua_common` target and write common unit tests

### Phase 3: `regex` Module (PCRE2 Wrapper)
- [ ] **Task 3.1:** Create `native/regex/CMakeLists.txt` linking `pcre2-8`
- [ ] **Task 3.2:** Implement C++ class `pcre2_engine` in `novelua::regex`
- [ ] **Task 3.3:** Implement `regex_jni.cpp` using `RegisterNatives`
- [ ] **Task 3.4:** Implement Kotlin `Regex` class in `io.github.novelua.regex`
- [ ] **Task 3.4:** Implement Kotlin `Pattern` class
- [ ] **Task 3.4:** Implement Kotlin `Matcher` class
- [ ] **Task 3.4:** Implement Kotlin `Match` class
- [ ] **Task 3.4:** Implement Kotlin `Group` class
- [ ] **Task 3.4:** Implement Kotlin `Replace` class
- [ ] **Task 3.4:** Implement Kotlin `Split` class
- [ ] **Task 3.4:** Implement Kotlin `Options` class
- [ ] **Task 3.5:** Write Unit Tests for `regex` module

### Phase 4: `lexsoup` Module (Lexbor Wrapper)
- [ ] **Task 4.1:** Create `native/lexsoup/CMakeLists.txt` linking Lexbor
- [ ] **Task 4.2:** Implement C++ wrappers in `novelua::lexsoup` (`document`, `element`, `selector`)
- [ ] **Task 4.3:** Implement `lexsoup_jni.cpp` using `RegisterNatives`
- [ ] **Task 4.4:** Implement Kotlin `Parser` class in `io.github.novelua.lexsoup`
- [ ] **Task 4.4:** Implement Kotlin `Document` class
- [ ] **Task 4.4:** Implement Kotlin `Element` class
- [ ] **Task 4.4:** Implement Kotlin `Elements` class
- [ ] **Task 4.4:** Implement Kotlin `Node` class
- [ ] **Task 4.4:** Implement Kotlin `TextNode` class
- [ ] **Task 4.4:** Implement Kotlin `Comment` class
- [ ] **Task 4.4:** Implement Kotlin `DocumentType` class
- [ ] **Task 4.4:** Implement Kotlin `Attribute` class
- [ ] **Task 4.4:** Implement Kotlin `Attributes` class
- [ ] **Task 4.4:** Implement Kotlin `Selector` class
- [ ] **Task 4.4:** Implement Kotlin `OutputSettings` class
- [ ] **Task 4.4:** Implement Kotlin `Entities` class
- [ ] **Task 4.5:** Write Unit Tests for `lexsoup` module

### Phase 5: `quickjs` Module (QuickJS Wrapper)
- [ ] **Task 5.1:** Create `native/quickjs/CMakeLists.txt` compiling QuickJS
- [ ] **Task 5.2:** Implement C++ wrappers in `novelua::quickjs` (`runtime`, `context`, `value`)
- [ ] **Task 5.3:** Implement `quickjs_jni.cpp` using `RegisterNatives`
- [ ] **Task 5.4:** Implement Kotlin `Runtime` class in `io.github.novelua.js`
- [ ] **Task 5.4:** Implement Kotlin `Context` class
- [ ] **Task 5.4:** Implement Kotlin `Value` class
- [ ] **Task 5.4:** Implement Kotlin `Object` class
- [ ] **Task 5.4:** Implement Kotlin `Array` class
- [ ] **Task 5.4:** Implement Kotlin `Function` class
- [ ] **Task 5.4:** Implement Kotlin `Promise` class
- [ ] **Task 5.4:** Implement Kotlin `Module` class
- [ ] **Task 5.5:** Write Unit Tests for `quickjs` module

### Phase 6: `luau` Module (Luau Wrapper)
- [ ] **Task 6.1:** Create `native/luau/CMakeLists.txt` compiling Luau
- [ ] **Task 6.2:** Implement C++ wrappers in `novelua::luau` (`vm`, `compiler`, `table`, `thread`)
- [ ] **Task 6.3:** Implement `luau_jni.cpp` using `RegisterNatives`
- [ ] **Task 6.4:** Implement Kotlin `VM` class in `io.github.novelua.luau`
- [ ] **Task 6.4:** Implement Kotlin `Compiler` class
- [ ] **Task 6.4:** Implement Kotlin `Table` class
- [ ] **Task 6.4:** Implement Kotlin `Function` class
- [ ] **Task 6.4:** Implement Kotlin `Thread` class
- [ ] **Task 6.4:** Implement Kotlin `Userdata` class
- [ ] **Task 6.4:** Implement Kotlin `Library` class
- [ ] **Task 6.4:** Implement Kotlin `Module` class
- [ ] **Task 6.5:** Write Unit Tests for `luau` module

### Phase 7: Integration, Verification & Release Packaging
- [ ] **Task 7.1:** Run AddressSanitizer (ASan) Memory Leak Audit across all native modules
- [ ] **Task 7.2:** Fix all compiler warnings under `-Wall -Wextra -Werror`
- [ ] **Task 7.3:** Verify 100% KDoc documentation on public APIs
- [ ] **Task 7.4:** Conduct strict Architecture & Coding Standard Compliance Code Review
- [ ] **Task 7.5:** Build final release AARs for `arm64-v8a`

---

## 6. Task Metrics & Implementation Effort

| Task ID | Task Description | Priority | Complexity | Est. Effort (Hours) |
|---|---|---|---|---|
| **1.1** | Add Git Submodules (Luau, QuickJS, PCRE2, Lexbor) | High | Low | 2.0 |
| **1.2** | Root Gradle & Version Catalog setup | High | Low | 2.0 |
| **1.3** | Create 4 Gradle module folders (`modules/*`) | High | Low | 1.0 |
| **1.4** | Create 5 Native folders (`native/*`) | High | Low | 1.0 |
| **1.5** | Configure CMake scripts for `arm64-v8a` / C++20 / C17 | High | Medium | 4.0 |
| **2.1** | Implement `jni_env.hpp` JavaVM thread cache | High | Medium | 3.0 |
| **2.2** | Implement `jstring_utils` RAII converters | High | Medium | 4.0 |
| **2.3** | Implement `scoped_local_ref` template helper | High | Low | 2.0 |
| **2.4** | Implement JNI Exception throwing utilities | High | Low | 2.0 |
| **2.5** | `novelua_common` CMake target & JNI unit tests | High | Medium | 3.0 |
| **3.1** | `native/regex` CMake setup with PCRE2 | High | Low | 2.0 |
| **3.2** | C++ `pcre2_engine` wrapper in `novelua::regex` | High | Medium | 6.0 |
| **3.3** | `regex_jni.cpp` `RegisterNatives` binding | High | Medium | 4.0 |
| **3.4** | Kotlin API (`io.github.novelua.regex.*`) | High | Medium | 6.0 |
| **3.5** | `regex` module unit tests | High | Medium | 4.0 |
| **4.1** | `native/lexsoup` CMake setup with Lexbor | High | Medium | 3.0 |
| **4.2** | C++ Lexbor wrappers (`document`, `element`, `selector`) | High | High | 12.0 |
| **4.3** | `lexsoup_jni.cpp` `RegisterNatives` binding | High | High | 8.0 |
| **4.4** | Kotlin API (`io.github.novelua.lexsoup.*`) | High | High | 10.0 |
| **4.5** | `lexsoup` module unit tests | High | High | 8.0 |
| **5.1** | `native/quickjs` CMake setup with QuickJS | High | Medium | 3.0 |
| **5.2** | C++ QuickJS wrappers (`runtime`, `context`, `value`) | High | High | 12.0 |
| **5.3** | `quickjs_jni.cpp` `RegisterNatives` binding | High | High | 8.0 |
| **5.4** | Kotlin API (`io.github.novelua.js.*`) | High | High | 10.0 |
| **5.5** | `quickjs` module unit tests | High | High | 8.0 |
| **6.1** | `native/luau` CMake setup with Luau Compiler/VM | High | Medium | 3.0 |
| **6.2** | C++ Luau wrappers (`vm`, `compiler`, `table`, `thread`) | High | High | 14.0 |
| **6.3** | `luau_jni.cpp` `RegisterNatives` binding | High | High | 10.0 |
| **6.4** | Kotlin API (`io.github.novelua.luau.*`) | High | High | 12.0 |
| **6.5** | `luau` module unit tests | High | High | 8.0 |
| **7.1** | AddressSanitizer (ASan) Memory Leak Audit | High | Medium | 6.0 |
| **7.2** | Compiler Warning Audit & Fixes (`-Werror`) | Medium | Low | 4.0 |
| **7.3** | Public KDoc documentation verification | Medium | Low | 4.0 |
| **7.4** | Architecture & Coding Standard Compliance Audit | High | Medium | 4.0 |
| **7.5** | Final Release AAR Packaging (`arm64-v8a`) | High | Low | 2.0 |
| **TOTAL** | | | | **183.0 Hours** |

---

## 7. Definition of Done (DoD)

Every task in this roadmap is considered **DONE** only when all of the following 8 criteria are strictly fulfilled:

1. **Implementation Completed:** All C++, C17, JNI, and Kotlin classes specified in the task are completely implemented without missing methods or stubbed implementations (`TODO()`).
2. **Build Success:** Clean compilation under Android Gradle Plugin and CMake targeting `arm64-v8a`.
3. **Unit Tests Passed:** 100% test execution pass rate on target devices/emulator.
4. **Zero Compiler Warnings:** Code builds cleanly with zero warnings under `-Wall -Wextra -Werror` (C/C++) and `-Werror` (Kotlin).
5. **Zero Memory Leaks:** Verified clean under AddressSanitizer (ASan) with zero dangling handles, unclosed native resources, or leaked `jobject` references.
6. **Architecture Compliance:** Fully complies with `000-architecture-overview.md` to `009-dependency-rules.md`:
   - Manual `RegisterNatives` used exclusively.
   - Native handles stored strictly as Kotlin `Long` (`private val nativeHandle: Long`).
   - Native objects implement C++ RAII.
   - Kotlin classes wrapping native handles implement `AutoCloseable`/`Closeable`.
   - Zero reflection, zero annotation processing, zero dynamic codegen.
7. **Coding Standard Compliance:** Fully complies with `010-coding-standard.md`:
   - Proper package naming (`io.github.novelua.*`).
   - Correct C++ snake_case filenames (`document.cpp`) and Kotlin PascalCase (`Document.kt`).
   - `novelua::*` C++ namespaces utilized.
   - `#pragma once` used in all headers.
8. **Documentation Updated:** Complete KDoc comments on all public classes, functions, and properties.

---

## 8. Risk Analysis & Mitigation Matrix

### 8.1 Technical Risks
- **Risk:** C++20 and C17 toolchain compatibility issues across different NDK releases for `arm64-v8a`.  
  - *Mitigation:* Pin NDK version explicitly in `libs.versions.toml` and enforce `-std=c++20` and `-std=c17` flags directly in root `cmake/toolchain.cmake`.

### 8.2 Architecture Risks
- **Risk:** Unintended native cross-module dependencies (e.g., `lexsoup` calling `quickjs` functions directly).  
  - *Mitigation:* Enforce strict CMake target isolation. Each module's `CMakeLists.txt` is only permitted to link against its own third-party submodule and `novelua_common`. Cross-module communication is strictly restricted to the Kotlin layer.

### 8.3 JNI Risks
- **Risk 1:** Local reference table overflow (`JNI local reference table summary (512 entries) overflow`).  
  - *Mitigation:* Require all JNI functions processing loops or multiple Java objects to wrap object creations with `ScopedLocalRef` or explicitly call `env->DeleteLocalRef()`.
- **Risk 2:** Thread safety crashes when caching `JNIEnv*` across threads.  
  - *Mitigation:* Enforce the rule in `006-threading-model.md` and `010-coding-standard.md`: `JavaVM*` may be cached globally, but `JNIEnv*` must never be stored across invocations. Use `novelua::common::GetJNIEnv()` to retrieve thread-local environments dynamically.

### 8.4 Memory Risks
- **Risk:** Double free or dangling pointer access when Kotlin objects are garbage-collected while native code holds pointers.  
  - *Mitigation:* Enforce RAII wrapper classes with `std::unique_ptr` ownership in C++. Native handles exposed to Kotlin as `Long` represent ownership pointers created via `new Wrapper()` and destroyed strictly in the wrapper's `close()` native JNI call.

### 8.5 Performance Risks
- **Risk:** High JNI boundary crossing overhead during bulk DOM queries or string parsing.  
  - *Mitigation:* Follow `008-performance-guidelines.md`. Avoid multi-call JNI loops; batch array/string conversions in a single native call, cache all `jclass`, `jmethodID`, and `jfieldID` during `JNI_OnLoad`, and use `std::string_view` to avoid unneeded string copies.

### 8.6 Maintenance Risks
- **Risk:** Hard-to-debug crashes occurring inside native third-party libraries (e.g. QuickJS memory corruption).  
  - *Mitigation:* Keep third-party libraries completely pristine inside `thirdparty/` as Git submodules without inline edits. Wrap all native library entry points with exception/error adapters (`novelua::common::ThrowException`) to translate C errors into Kotlin runtime exceptions cleanly.

---

## 9. Recommended Development Order & Rationale

```
Step 1: Setup Infra (Phase 1)
   │
   ▼
Step 2: Implement Common JNI (Phase 2)
   │
   ▼
Step 3: Implement Regex Module (Phase 3)
   │  └── Rationale: Minimal state, single-class C++ wrapper, verifies JNI pipeline.
   ▼
Step 4: Implement LexSoup Module (Phase 4)
   │  └── Rationale: Introduces complex tree/node relationships & C memory structures.
   ▼
Step 5: Implement QuickJS Module (Phase 5)
   │  └── Rationale: Introduces dynamic JS values, runtime memory GC & callback management.
   ▼
Step 6: Implement Luau Module (Phase 6)
   │  └── Rationale: Most complex host environment (VM, stack, bytecode, coroutines, host libraries).
   ▼
Step 7: Final QA & Packaging (Phase 7)
```

### Rationale:
1. **Infrastructure & Common First:** Establishing `native/common` early prevents duplicate JNI utility code across modules and enforces a single standard for string conversions, reference management, and exception handling.
2. **`regex` as the Calibration Benchmark:** PCRE2 is self-contained and stateless per pattern execution. Implementing `regex` first establishes and validates the module template (`CMakeLists.txt`, `RegisterNatives`, Kotlin `Long` handle pattern) with minimal debugging noise.
3. **Incremental State & Ownership Complexity:**
   - `lexsoup` introduces hierarchical document/node ownership trees.
   - `quickjs` adds dynamic engine evaluation, values, and garbage collector interop.
   - `luau` represents the most complex module, requiring stack manipulation, bytecode compilation, coroutine yielding, table iterators, and native library bindings.
4. **Parallel Execution Readiness:** Once Phase 2 (`native/common`) is complete, Phases 3, 4, 5, and 6 are native-decoupled and can be implemented concurrently by independent developer subagents if desired.

---

## 10. Practical Daily Progress Plan (15-Day Roadmap)

### Day 1: Workspace & Submodule Setup
- Task 1.1: Initialize Git submodules (`luau`, `quickjs`, `pcre2`, `lexbor`).
- Task 1.2: Set up root `build.gradle.kts`, `settings.gradle.kts`, and `gradle/libs.versions.toml`.
- Task 1.3: Create directory structures for `modules/regex`, `modules/lexsoup`, `modules/quickjs`, `modules/luau`.

### Day 2: Native Structure & CMake Toolchain
- Task 1.4: Create directory structures for `native/common`, `native/regex`, `native/lexsoup`, `native/quickjs`, `native/luau`.
- Task 1.5: Configure root CMake build infrastructure scripts for `arm64-v8a` target with C++20 and C17 standard flags.

### Day 3: Common Native Utility Layer (`native/common`)
- Task 2.1: Implement `jni_env.hpp` (`JavaVM` global caching & thread-local `JNIEnv*` retrieval).
- Task 2.2: Implement `jstring_utils.hpp/.cpp` (RAII string conversions).
- Task 2.3: Implement `scoped_local_ref.hpp`.
- Task 2.4: Implement `exceptions.hpp/.cpp`.
- Task 2.5: Build `novelua_common` CMake target and run common JNI unit tests.

### Day 4: `regex` Module C++ & JNI Implementation
- Task 3.1: Configure `native/regex/CMakeLists.txt` to link `pcre2-8`.
- Task 3.2: Implement `pcre2_engine` C++ wrapper class in `novelua::regex`.
- Task 3.3: Implement `regex_jni.cpp` using `RegisterNatives`.

### Day 5: `regex` Module Kotlin API & Testing
- Task 3.4: Implement Kotlin API classes (`Regex`, `Pattern`, `Matcher`, `Match`, `Group`, `Replace`, `Split`, `Options`) in `io.github.novelua.regex`.
- Task 3.5: Implement and execute comprehensive unit tests for `regex` module.

### Day 6: `lexsoup` Native C++ Document & Element Wrappers
- Task 4.1: Configure `native/lexsoup/CMakeLists.txt` to compile and link Lexbor library.
- Task 4.2: Implement native C++ document and element wrapper classes in `novelua::lexsoup`.

### Day 7: `lexsoup` Native Selector & JNI Binding
- Task 4.2 (cont.): Implement native C++ CSS selector wrapper.
- Task 4.3: Implement `lexsoup_jni.cpp` registering JNI functions via `RegisterNatives`.

### Day 8: `lexsoup` Kotlin API & Testing
- Task 4.4: Implement Kotlin API classes (`Parser`, `Document`, `Element`, `Elements`, `Node`, `TextNode`, `Comment`, `DocumentType`, `Attribute`, `Attributes`, `Selector`, `OutputSettings`, `Entities`) in `io.github.novelua.lexsoup`.
- Task 4.5: Implement and execute comprehensive DOM and CSS selector unit tests for `lexsoup`.

### Day 9: `quickjs` Native Runtime & Context Wrappers
- Task 5.1: Configure `native/quickjs/CMakeLists.txt` to compile QuickJS core sources.
- Task 5.2: Implement C++ `Runtime` and `Context` wrapper classes in `novelua::quickjs`.

### Day 10: `quickjs` Native Value Wrappers & JNI Binding
- Task 5.2 (cont.): Implement C++ `Value` wrapper for JS dynamic values.
- Task 5.3: Implement `quickjs_jni.cpp` registering native functions via `RegisterNatives`.

### Day 11: `quickjs` Kotlin API & Testing
- Task 5.4: Implement Kotlin API classes (`Runtime`, `Context`, `Value`, `Object`, `Array`, `Function`, `Promise`, `Module`) in `io.github.novelua.js`.
- Task 5.5: Implement and execute unit tests for JS script evaluation, global context access, and promise callbacks.

### Day 12: `luau` Native VM & Compiler Wrappers
- Task 6.1: Configure `native/luau/CMakeLists.txt` to compile Luau compiler and VM sources.
- Task 6.2: Implement C++ VM, Compiler, and Table wrapper classes in `novelua::luau`.

### Day 13: `luau` Native Threading, Userdata & JNI Binding
- Task 6.2 (cont.): Implement C++ Thread/Coroutine and Userdata wrappers.
- Task 6.3: Implement `luau_jni.cpp` registering native functions via `RegisterNatives`.

### Day 14: `luau` Kotlin API & Testing
- Task 6.4: Implement Kotlin API classes (`VM`, `Compiler`, `Table`, `Function`, `Thread`, `Userdata`, `Library`, `Module`) in `io.github.novelua.luau`.
- Task 6.5: Implement and execute unit tests for bytecode compilation, Luau execution, table manipulation, and host module registration.

### Day 15: Quality Assurance, Leak Audits & Release Packaging
- Task 7.1: Run AddressSanitizer (ASan) memory leak audits on `arm64-v8a`.
- Task 7.2: Fix all compiler warnings under `-Wall -Wextra -Werror` and `-Werror`.
- Task 7.3: Complete KDoc documentation audit.
- Task 7.4: Perform final architecture and coding standard compliance verification.
- Task 7.5: Build production release AAR packages for all 4 SDK modules.

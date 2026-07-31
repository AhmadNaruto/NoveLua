## C++ Runtime Policy

Identify which native library or build configuration introduces the dependency on libc++_shared.so. If the dependency is unnecessary, remove it. If it is required, explain precisely why and propose an alternative solution.

This project must not ship `libc++_shared.so` unless there is a technically unavoidable reason.

The preferred configuration is:

- Use `c++_static` as the C++ runtime.
- Produce a fully self-contained native library.
- Do not package `libc++_shared.so`.
- Eliminate any dependency on the shared C++ runtime whenever possible.

If static linking cannot be used, provide a detailed technical justification identifying the exact dependency that requires `libc++_shared.so`.

Treat any inclusion of `libc++_shared.so` as a build issue rather than the default configuration.

Include a before/after comparison of:

- Native library size
- APK size
- Startup time
- Runtime memory usage

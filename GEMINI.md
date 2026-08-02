# NoveLua Repository Rules

## Strict Execution Rules
1. NEVER run multiple gradle/gradlew commands concurrently. Always check for and terminate any existing background gradle/gradlew task before launching a new one.
2. NEVER make direct modifications to files inside Git submodules (e.g., inside the `thirdparty/` directory). Instead, apply changes via patch files if possible.

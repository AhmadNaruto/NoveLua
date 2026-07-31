package io.github.novelua.regex

/**
 * Entry point for regular expressions.
 */
object Regex {

    /**
     * Compiles the given regular expression into a pattern.
     */
    fun compile(pattern: String): Pattern {
        return compile(pattern, Options())
    }

    /**
     * Compiles the given regular expression into a pattern with options.
     */
    fun compile(pattern: String, options: Options): Pattern {
        return Pattern(pattern, options.toBitmask())
    }
}

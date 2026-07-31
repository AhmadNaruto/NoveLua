package io.github.novelua.regex

/**
 * Replaces the first subsequence of the input sequence that matches the pattern with the given replacement string.
 */
fun Pattern.replaceFirst(input: String, replacement: String): String {
    return this.internalReplaceFirst(input, replacement)
}

/**
 * Replaces every subsequence of the input sequence that matches the pattern with the given replacement string.
 */
fun Pattern.replaceAll(input: String, replacement: String): String {
    return this.internalReplaceAll(input, replacement)
}

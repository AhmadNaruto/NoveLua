package io.github.novelua.regex

/**
 * Splits the given input sequence around matches of this pattern.
 */
fun Pattern.split(input: String): Array<String> {
    val result = mutableListOf<String>()
    /**
     * Property matcher.
     */
    val matcher = this.matcher(input)
    /**
     * Property lastEnd.
     */
    var lastEnd = 0
    while (matcher.find()) {
        result.add(input.substring(lastEnd, matcher.start()))
        lastEnd = matcher.end()
    }
    result.add(input.substring(lastEnd))
    return result.toTypedArray()
}

package io.github.novelua.regex

/**
 * Represents a successful match of a regular expression.
 *
 * @property value The full text matched by the expression.
 * @property start The starting index of the match in the input string.
 * @property end The ending index of the match in the input string.
 * @property groups An array of captured groups, if any.
 */
data class Match(
    val value: String,
    /**
     * Property start.
     */
    val start: Int,
    /**
     * Property end.
     */
    val end: Int,
    /**
     * Property groups.
     */
    val groups: Array<Group>
) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as Match

        if (value != other.value) return false
        if (start != other.start) return false
        if (end != other.end) return false
        if (!groups.contentEquals(other.groups)) return false

        return true
    }

    override fun hashCode(): Int {
        /**
         * Property result.
         */
        var result = value.hashCode()
        result = 31 * result + start
        result = 31 * result + end
        result = 31 * result + groups.contentDeepHashCode()
        return result
    }
}

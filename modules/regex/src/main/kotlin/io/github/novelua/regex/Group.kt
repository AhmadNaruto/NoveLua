package io.github.novelua.regex

/**
 * Represents a matched group within a regular expression.
 *
 * @property index The 0-based index of the group.
 * @property value The text matched by the group.
 */
data class Group(
    val index: Int,
    /**
     * Property value.
     */
    val value: String
)

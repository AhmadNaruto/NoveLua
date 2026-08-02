package io.github.novelua.regex

/**
 * An engine that performs match operations on a character sequence by interpreting a Pattern.
 */
class Matcher internal constructor(
    private val pattern: Pattern,
    private val input: String,
    private val nativeHandle: Long
) : AutoCloseable {

    private var currentMatch: Match? = null
    private var searchOffset: Int = 0

    /**
     * Attempts to match the entire region against the pattern.
     */
    fun matches(): Boolean {
        val match = nativeMatch(nativeHandle, input, 0)
        if (match != null && match.start == 0 && match.end == input.length) {
            currentMatch = match
            searchOffset = match.end
            return true
        }
        currentMatch = null
        return false
    }

    /**
     * Attempts to find the next subsequence of the input sequence that matches the pattern.
     */
    fun find(): Boolean {
        if (searchOffset > input.length) {
            currentMatch = null
            return false
        }
        
        /**
         * Property match.
         */
        val match = nativeMatch(nativeHandle, input, searchOffset)
        if (match != null) {
            currentMatch = match
            searchOffset = match.end
            if (match.start == match.end) {
                searchOffset++
            }
            return true
        }
        currentMatch = null
        return false
    }

    /**
     * Returns the input subsequence captured by the given group during the previous match operation.
     */
    fun group(index: Int): String? {
        val match = currentMatch ?: throw IllegalStateException("No successful match.")
        if (index == 0) return match.value
        return match.groups.find { it.index == index }?.value
    }

    /**
     * Returns the number of capturing groups in this matcher's pattern.
     */
    fun groupCount(): Int {
        val match = currentMatch ?: return 0
        return match.groups.maxOfOrNull { it.index } ?: 0
    }

    /**
     * Returns the start index of the previous match.
     */
    fun start(): Int {
        return currentMatch?.start ?: throw IllegalStateException("No successful match.")
    }

    /**
     * Returns the offset after the last character matched.
     */
    fun end(): Int {
        return currentMatch?.end ?: throw IllegalStateException("No successful match.")
    }

    /**
     * Resets this matcher.
     */
    fun reset() {
        currentMatch = null
        searchOffset = 0
    }

    override fun close() {
        // No-op for now as native resources are managed by Pattern
    }

    private external fun nativeMatch(handle: Long, input: String, offset: Int): Match?
}

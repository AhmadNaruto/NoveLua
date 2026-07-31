package io.github.novelua.regex

/**
 * Options for regular expression compilation.
 *
 * @property ignoreCase Case-insensitive matching.
 * @property multiline Multiline matching.
 * @property dotAll Dot matches all characters including newline.
 * @property unicode Enable Unicode support.
 */
data class Options(
    val ignoreCase: Boolean = false,
    /**
     * Property multiline.
     */
    val multiline: Boolean = false,
    /**
     * Property dotAll.
     */
    val dotAll: Boolean = false,
    /**
     * Property unicode.
     */
    val unicode: Boolean = false
) {
    internal fun toBitmask(): Int {
        /**
         * Property mask.
         */
        var mask = 0
        if (ignoreCase) mask = mask or 0x00000008 // PCRE2_CASELESS
        if (multiline) mask = mask or 0x00000400 // PCRE2_MULTILINE
        if (dotAll) mask = mask or 0x00000020 // PCRE2_DOTALL
        if (unicode) mask = mask or 0x00080000 // PCRE2_UTF
        return mask
    }
}

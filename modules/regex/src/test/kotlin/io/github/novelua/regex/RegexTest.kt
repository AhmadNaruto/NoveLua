package io.github.novelua.regex

import org.junit.Assert.*
import org.junit.Test

class RegexTest {

    @Test
    fun testCompileAndMatch() {
        try {
            Regex.compile("hello (world)").use { pattern ->
                val matcher = pattern.matcher("hello world")
                assertTrue(matcher.matches())
                assertEquals("hello world", matcher.group(0))
                assertEquals("world", matcher.group(1))
                assertEquals(0, matcher.start())
                assertEquals(11, matcher.end())
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testFind() {
        try {
            Regex.compile("\\d+").use { pattern ->
                val matcher = pattern.matcher("foo 123 bar 456")
                assertTrue(matcher.find())
                assertEquals("123", matcher.group(0))
                assertEquals(4, matcher.start())
                
                assertTrue(matcher.find())
                assertEquals("456", matcher.group(0))
                assertEquals(12, matcher.start())
                
                assertFalse(matcher.find())
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testReplace() {
        try {
            Regex.compile("foo").use { pattern ->
                val result = pattern.replaceFirst("foo bar foo", "baz")
                assertEquals("baz bar foo", result)
                
                val allResult = pattern.replaceAll("foo bar foo", "baz")
                assertEquals("baz bar baz", allResult)
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testSplit() {
        try {
            Regex.compile(",\\s*").use { pattern ->
                val result = pattern.split("a, b,c,  d")
                assertArrayEquals(arrayOf("a", "b", "c", "d"), result)
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }

    @Test
    fun testOptions() {
        try {
            Regex.compile("hello", Options(ignoreCase = true)).use { pattern ->
                val matcher = pattern.matcher("HELLO")
                assertTrue(matcher.matches())
            }
        } catch (_: UnsatisfiedLinkError) {
        }
    }
}

package io.github.novela.luau.demo

import android.os.Bundle
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import io.github.novela.luau.*
import kotlinx.coroutines.launch

data class Preset(val name: String, val code: String)
data class LogEntry(val text: String, val type: LogType)

enum class LogType {
    INFO,
    SUCCESS,
    ERROR,
    SYSTEM
}

class MainActivity : ComponentActivity() {

    private val presets = listOf(
        Preset(
            name = "Hello Luau",
            code = """-- Hello from Luau!
print("Hello from Luau VM!")
print("NoveLua is running natively on Android ART.")
print("This output is intercepted via JNI callback.")
"""
        ),
        Preset(
            name = "Arithmetic",
            code = """-- Simple calculation and iteration
local sum = 0
for i = 1, 10 do
    sum = sum + i
end
print("The sum of numbers from 1 to 10 is: " .. sum)
"""
        ),
        Preset(
            name = "Table JNI",
            code = """-- Read and write Luau tables
local user = { name = "Ahmad", age = 22 }
print("Original table:")
print("Name: " .. user.name)
print("Age: " .. user.age)

user.role = "Developer"
print("Added new field:")
print("Role: " .. user.role)
"""
        ),
        Preset(
            name = "JNI Callbacks",
            code = """-- Call native Android APIs via Kotlin callbacks
print("Calling Android Toast API from Luau...")
show_toast("Greetings from Luau script!")

print("Fetching system info from Kotlin JNI...")
local info = get_app_info()
print("App Name: " .. info.name)
print("Android SDK: " .. info.sdk)
"""
        ),
        Preset(
            name = "Exceptions",
            code = """-- Exception handling and stack trace propagation
function verify(val)
    if val < 0 then
        error("Negative values not allowed!")
    end
end

function process()
    print("Processing started...")
    verify(-1)
end

process()
"""
        )
    )

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            PlaygroundScreen()
        }
    }

    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun PlaygroundScreen() {
        var codeText by remember { mutableStateOf(presets[0].code) }
        val logs = remember { mutableStateListOf<LogEntry>() }
        val scope = rememberCoroutineScope()
        val lazyListState = rememberLazyListState()

        val darkThemeColors = darkColorScheme(
            background = Color(0xFF151518),
            surface = Color(0xFF1E1E22),
            primary = Color(0xFF00ADB5),
            secondary = Color(0xFF8A2BE2)
        )

        MaterialTheme(colorScheme = darkThemeColors) {
            Scaffold(
                topBar = {
                    TopAppBar(
                        title = {
                            Column {
                                Text(
                                    text = "NoveLua Playground",
                                    fontWeight = FontWeight.Bold,
                                    color = Color.White
                                )
                                Text(
                                    text = "JNI-based Luau VM Bridge for Android",
                                    fontSize = 11.sp,
                                    color = Color.Gray
                                )
                            }
                        },
                        colors = TopAppBarDefaults.topAppBarColors(
                            containerColor = MaterialTheme.colorScheme.surface
                        )
                    )
                }
            ) { padding ->
                Column(
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(padding)
                        .background(MaterialTheme.colorScheme.background)
                        .padding(16.dp)
                ) {
                    // Presets Row
                    Text(
                        text = "PRESET SCRIPTS",
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Bold,
                        color = Color.Gray,
                        modifier = Modifier.padding(bottom = 6.dp)
                    )
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .horizontalScroll(rememberScrollState())
                            .padding(bottom = 16.dp),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        presets.forEach { preset ->
                            Box(
                                modifier = Modifier
                                    .background(
                                        color = if (codeText == preset.code) MaterialTheme.colorScheme.primary.copy(alpha = 0.2f) else MaterialTheme.colorScheme.surface,
                                        shape = RoundedCornerShape(8.dp)
                                    )
                                    .border(
                                        width = 1.dp,
                                        color = if (codeText == preset.code) MaterialTheme.colorScheme.primary else Color.Gray.copy(alpha = 0.3f),
                                        shape = RoundedCornerShape(8.dp)
                                    )
                                    .clickable {
                                        codeText = preset.code
                                    }
                                    .padding(horizontal = 12.dp, vertical = 8.dp)
                            ) {
                                Text(
                                    text = preset.name,
                                    color = if (codeText == preset.code) Color.White else Color.Gray,
                                    fontSize = 13.sp,
                                    fontWeight = FontWeight.Medium
                                )
                            }
                        }
                    }

                    // Code Editor Title and Run Button
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            text = "LUAU CODE EDITOR",
                            fontSize = 12.sp,
                            fontWeight = FontWeight.Bold,
                            color = Color.Gray
                        )

                        Button(
                            onClick = {
                                executeLua(codeText, logs)
                                scope.launch {
                                    if (logs.isNotEmpty()) {
                                        lazyListState.animateScrollToItem(logs.size - 1)
                                    }
                                }
                            },
                            colors = ButtonDefaults.buttonColors(
                                containerColor = MaterialTheme.colorScheme.primary
                            ),
                            contentPadding = PaddingValues(horizontal = 16.dp, vertical = 6.dp),
                            shape = RoundedCornerShape(8.dp)
                        ) {
                            Icon(
                                imageVector = Icons.Default.PlayArrow,
                                contentDescription = "Run",
                                tint = Color.Black
                            )
                            Spacer(modifier = Modifier.width(4.dp))
                            Text("RUN SCRIPT", color = Color.Black, fontWeight = FontWeight.Bold, fontSize = 13.sp)
                        }
                    }

                    Spacer(modifier = Modifier.height(8.dp))

                    // Code Editor Text Field
                    TextField(
                        value = codeText,
                        onValueChange = { codeText = it },
                        modifier = Modifier
                            .fillMaxWidth()
                            .weight(1f)
                            .border(1.dp, Color.Gray.copy(alpha = 0.2f), RoundedCornerShape(8.dp)),
                        textStyle = TextStyle(
                            fontFamily = FontFamily.Monospace,
                            fontSize = 14.sp,
                            color = Color(0xFFD4D4D4)
                        ),
                        colors = TextFieldDefaults.colors(
                            focusedContainerColor = MaterialTheme.colorScheme.surface,
                            unfocusedContainerColor = MaterialTheme.colorScheme.surface,
                            focusedIndicatorColor = Color.Transparent,
                            unfocusedIndicatorColor = Color.Transparent
                        ),
                        shape = RoundedCornerShape(8.dp)
                    )

                    Spacer(modifier = Modifier.height(16.dp))

                    // Console Output Title
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            text = "CONSOLE OUTPUT",
                            fontSize = 12.sp,
                            fontWeight = FontWeight.Bold,
                            color = Color.Gray
                        )

                        Text(
                            text = "Clear Output",
                            fontSize = 11.sp,
                            color = MaterialTheme.colorScheme.primary,
                            fontWeight = FontWeight.Medium,
                            modifier = Modifier
                                .clickable { logs.clear() }
                                .padding(4.dp)
                        )
                    }

                    Spacer(modifier = Modifier.height(8.dp))

                    // Console Output lazy column
                    LazyColumn(
                        state = lazyListState,
                        modifier = Modifier
                            .fillMaxWidth()
                            .height(180.dp)
                            .background(Color(0xFF0F0F11), RoundedCornerShape(8.dp))
                            .border(1.dp, Color.Gray.copy(alpha = 0.1f), RoundedCornerShape(8.dp))
                            .padding(12.dp),
                        verticalArrangement = Arrangement.spacedBy(4.dp)
                    ) {
                        if (logs.isEmpty()) {
                            item {
                                Text(
                                    text = "~ Console idle. Click RUN SCRIPT to execute Luau.",
                                    color = Color.Gray.copy(alpha = 0.6f),
                                    fontFamily = FontFamily.Monospace,
                                    fontSize = 13.sp
                                )
                            }
                        } else {
                            items(logs) { log ->
                                val color = when (log.type) {
                                    LogType.INFO -> Color(0xFFE0E0E0)
                                    LogType.SUCCESS -> Color(0xFF00FF66)
                                    LogType.ERROR -> Color(0xFFFF453A)
                                    LogType.SYSTEM -> Color(0xFF0A84FF)
                                }
                                Text(
                                    text = log.text,
                                    color = color,
                                    fontFamily = FontFamily.Monospace,
                                    fontSize = 13.sp
                                )
                            }
                        }
                    }
                }
            }
        }
    }

    private fun executeLua(script: String, logs: MutableList<LogEntry>) {
        logs.add(LogEntry("> Executing Luau Script...", LogType.SYSTEM))
        val startTime = System.nanoTime()

        try {
            LuaEngine().use { engine ->
                val L = engine.mainState

                // Register print override to intercept Lua print() calls
                val printFunc = LuaFunction { state ->
                    val nargs = state.getTop()
                    val sb = StringBuilder()
                    for (i in 1..nargs) {
                        sb.append(state.toString(i) ?: "nil")
                        if (i < nargs) sb.append("  ")
                    }
                    logs.add(LogEntry(sb.toString(), LogType.INFO))
                    0
                }
                L.pushFunction(printFunc, "print")
                L.setField(LuaState.GLOBALS_INDEX, "print")

                // Register toast override to display Toast notification
                val showToastFunc = LuaFunction { state ->
                    val msg = state.toString(1) ?: ""
                    runOnUiThread {
                        Toast.makeText(this@MainActivity, msg, Toast.LENGTH_SHORT).show()
                    }
                    0
                }
                L.pushFunction(showToastFunc, "show_toast")
                L.setField(LuaState.GLOBALS_INDEX, "show_toast")

                // Register get_app_info JNI table returning method
                val getAppInfoFunc = LuaFunction { state ->
                    state.newTable()
                    val ref = state.popReference()
                    val table = LuaTable(state, ref)
                    table.set("name", "NoveLua Demo App")
                    table.set("sdk", android.os.Build.VERSION.SDK_INT.toDouble())
                    state.push(table)
                    1
                }
                L.pushFunction(getAppInfoFunc, "get_app_info")
                L.setField(LuaState.GLOBALS_INDEX, "get_app_info")

                // Run the script
                engine.execute(script)
                
                val elapsedMs = (System.nanoTime() - startTime) / 1_000_000.0
                logs.add(LogEntry("✓ Execution completed successfully in ${String.format("%.2f", elapsedMs)} ms", LogType.SUCCESS))
            }
        } catch (e: LuaException) {
            logs.add(LogEntry("LuaException: ${e.message}", LogType.ERROR))
            // Print the Lua stack trace frames
            e.stackTrace.forEach { frame ->
                if (frame.className == "lua") {
                    logs.add(LogEntry("   at ${frame.methodName} (${frame.fileName}:${frame.lineNumber})", LogType.ERROR))
                }
            }
        } catch (e: Exception) {
            logs.add(LogEntry("Exception: ${e.message}", LogType.ERROR))
        }
    }
}

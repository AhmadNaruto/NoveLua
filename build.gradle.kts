plugins {
    id("com.android.library") version "8.7.3"
    kotlin("android") version "2.0.0"
}

val compileNatives = tasks.register("compileNatives") {
    doLast {
        val buildDir = file("${layout.buildDirectory.get()}/novelua-build")
        val outputDir = file("${layout.buildDirectory.get()}/novelua")
        buildDir.mkdirs()
        outputDir.mkdirs()
        
        // Run cmake configuration
        val p1 = ProcessBuilder("cmake", "../..")
            .directory(buildDir)
            .inheritIO()
            .start()
        val code1 = p1.waitFor()
        if (code1 != 0) throw GradleException("cmake configuration failed with exit code $code1")
        
        // Run cmake build
        val p2 = ProcessBuilder("cmake", "--build", ".")
            .directory(buildDir)
            .inheritIO()
            .start()
        val code2 = p2.waitFor()
        if (code2 != 0) throw GradleException("cmake build failed with exit code $code2")
        
        // Find libnovelua.so and copy it
        val libFile = buildDir.walkTopDown().find { it.name.contains("libnovelua") }
            ?: throw GradleException("Could not find libnovelua in build directory")
        libFile.copyTo(file("${outputDir}/${libFile.name}"), overwrite = true)
    }
}

android {
    namespace = "io.github.novela.luau"
    compileSdk = 34

    defaultConfig {
        minSdk = 21

        externalNativeBuild {
            cmake {
                arguments("-DANDROID_STL=c++_shared")
            }
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    kotlin {
        jvmToolchain(17)
    }

    externalNativeBuild {
        cmake {
            path = file("CMakeLists.txt")
        }
    }

    testOptions {
        unitTests.all {
            useJUnitPlatform()
            dependsOn(compileNatives)
            systemProperty("java.library.path", file("${layout.buildDirectory.get()}/novelua").absolutePath)
        }
    }
}

dependencies {
    implementation("org.jetbrains:annotations:24.0.0")
    testImplementation(kotlin("test"))
}

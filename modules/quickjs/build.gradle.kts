plugins {
    alias(libs.plugins.android.library)
    alias(libs.plugins.kotlin.android)
}

android {
    namespace = "io.github.novelua.js"
    compileSdk = libs.versions.compileSdk.get().toInt()
    ndkVersion = libs.versions.ndk.get()

    defaultConfig {
        minSdk = libs.versions.minSdk.get().toInt()

        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++20"
                cFlags += "-std=c17"
                arguments += "-DANDROID_STL=c++_static"
                arguments += "-DCMAKE_EXE_LINKER_FLAGS=-nostdlib++ -lc++_static -lc++abi"
                arguments += "-DCMAKE_SHARED_LINKER_FLAGS=-nostdlib++ -lc++_static -lc++abi"
                abiFilters += "arm64-v8a"
            }
        }
    }

    externalNativeBuild {
        cmake {
            path = file("../../native/quickjs/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    
    kotlinOptions {
        jvmTarget = "17"
    }
}

dependencies {
    testImplementation(libs.junit)
}

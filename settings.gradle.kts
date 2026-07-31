pluginManagement {
    repositories {
        google()
        mavenCentral()
        gradlePluginPortal()
    }
}

dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
    }
}

rootProject.name = "Novelua"

include(":modules:regex")
include(":modules:lexsoup")
include(":modules:quickjs")
include(":modules:luau")
include(":modules:benchmark")
include(":modules:interop")
include(":app")

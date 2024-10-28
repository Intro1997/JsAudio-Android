import com.android.build.gradle.internal.cxx.configure.gradleLocalProperties

val server_address: String = gradleLocalProperties(rootDir).getProperty("server_address")

plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

val printBuildFinishedMessage by tasks.creating(Task::class) {
    doLast {
        val cmdWorkingPath = rootDir.path
        var terminalType = "bash"
        var subCmd = "-c"
        val os = System.getProperty("os.name")
        if (os.contains("windows")) {
            terminalType = "cmd"
            subCmd = "/c"
        }
        exec {
            commandLine(terminalType, subCmd, "npm run combine").apply {
                this.workingDir = File(cmdWorkingPath)
            }
        }
    }
}

tasks.whenTaskAdded {
    if (this.name == "assembleDebug") {
        this.finalizedBy(printBuildFinishedMessage)
    }
}

android {
    namespace = "com.example.jsaudio"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.example.jsaudio"
        minSdk = 23
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            buildConfigField("String", "SERVER_ADDRESS", server_address)
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
        debug {
            buildConfigField("String", "SERVER_ADDRESS", server_address)
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }
    buildFeatures {
        viewBinding = true
        buildConfig = true
    }
}

dependencies {
    implementation("androidx.core:core-ktx:1.13.1")
    implementation("androidx.appcompat:appcompat:1.7.0")
    implementation("com.google.android.material:material:1.12.0")
    implementation("androidx.constraintlayout:constraintlayout:2.1.4")
    implementation(project(":app:node_env"))
    implementation(project(":app:audio"))
    implementation("com.jakewharton:process-phoenix:3.0.0")
    testImplementation("junit:junit:4.13.2")
    androidTestImplementation("androidx.test.ext:junit:1.1.5")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")
}
package com.example.node_env

interface NodeModuleHandler {
    fun create() {}
    fun pause() {}
    fun resume() {}
    fun destroy() {}
    fun getPreloadScript(): String {
        return ""
    }
}
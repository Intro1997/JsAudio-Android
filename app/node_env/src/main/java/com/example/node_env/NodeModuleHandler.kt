package com.example.node_env

interface NodeModuleHandler {
    fun create() {}
    fun start() {}
    fun pause() {}
    fun stop() {}
    fun resume() {}
    fun destroy() {}
    fun getPreloadScript(): String {
        return ""
    }
}
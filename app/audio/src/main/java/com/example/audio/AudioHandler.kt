package com.example.audio

import android.util.Log
import com.example.node_env.NodeModuleHandler

class AudioHandler : NodeModuleHandler {
    companion object {
        private const val TAG = "AudioHandler"

        init {
            Log.d(TAG, "load audio library")
            System.loadLibrary("audio")
        }
    }

    override fun getPreloadScript(): String {
        return getPreloadScriptFromNative()
    }

    private external fun getPreloadScriptFromNative(): String
}
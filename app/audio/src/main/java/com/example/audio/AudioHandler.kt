package com.example.audio

import android.util.Log

class AudioHandler {
    companion object {
        private const val TAG = "AudioHandler"

        init {
            Log.d(TAG, "load audio library")
            System.loadLibrary("audio")
        }
    }
}
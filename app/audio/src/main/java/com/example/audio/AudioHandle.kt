package com.example.audio

import android.util.Log

class AudioHandle {
    companion object {
        init {
            Log.d("AudioHandle", "load audio library")
            System.loadLibrary("audio")
        }
    }
}
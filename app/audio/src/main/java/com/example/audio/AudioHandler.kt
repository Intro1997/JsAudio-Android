package com.example.audio

import android.content.Context
import android.media.AudioManager
import android.util.Log
import com.example.node_env.NodeModuleHandler

class AudioHandler(context: Context) : NodeModuleHandler {
    init {
        val audioManager = context.getSystemService(Context.AUDIO_SERVICE) as AudioManager
        outputSampleRate =
            audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE).toInt()

        setAudioEngineInitParam(outputSampleRate)
    }

    companion object {
        private const val TAG = "AudioHandler"
        var outputSampleRate = 0

        init {
            Log.d(TAG, "load audio library")
            System.loadLibrary("audio")
        }
    }

    override fun getPreloadScript(): String {
        return getPreloadScriptFromNative()
    }

    override fun start() {
        super.start()
        Log.d(TAG, "start Audio Engine Handler")
        startNativeAudioEngine()
    }


    override fun pause() {
        super.pause()
        Log.d(TAG, "pause Audio Engine Handler")
        pauseNativeAudioEngine()
    }

    override fun stop() {
        super.stop()
        Log.d(TAG, "stop Audio Engine Handler")
        stopNativeAudioEngine()
    }

    override fun resume() {
        super.resume()
        Log.d(TAG, "resume Audio Engine Handler")
        resumeNativeAudioEngine()
    }

    override fun destroy() {
        Log.d(TAG, "destroy Audio Engine Handler")
        destroyNativeAudioEngine()
    }

    private external fun getPreloadScriptFromNative(): String

    private external fun setAudioEngineInitParam(
        outputSampleRate: Int
    )

    private external fun startNativeAudioEngine()
    private external fun pauseNativeAudioEngine()
    private external fun resumeNativeAudioEngine()
    private external fun stopNativeAudioEngine()
    private external fun destroyNativeAudioEngine()
}
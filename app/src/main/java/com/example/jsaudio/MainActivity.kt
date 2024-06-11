package com.example.jsaudio

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.example.audio.AudioHandler
import com.example.jsaudio.databinding.ActivityMainBinding
import com.example.node_env.NodeEnvHandler

class MainActivity : AppCompatActivity() {
    companion object {
        private const val TAG = "MainActivity"
    }

    private lateinit var binding: ActivityMainBinding
    private var nodeEnvHandler: NodeEnvHandler? = null
    private var audioHandler: AudioHandler? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d(TAG, "Create application")

        nodeEnvHandler = NodeEnvHandler.create()
        audioHandler = AudioHandler()

        val ret = "create node env " + if (nodeEnvHandler == null) "failed" else "success"

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        binding.sampleText.text = ret
    }

    override fun onStart() {
        super.onStart()
        Log.d(TAG, "Start application")
        nodeEnvHandler?.evalCode(getJsEntry())
    }

    override fun onResume() {
        super.onResume()
        Log.d(TAG, "Resume application")
        nodeEnvHandler?.resume()
    }


    override fun onPause() {
        super.onPause()
        Log.d(TAG, "Pause application")
        nodeEnvHandler?.pause()
    }


    override fun onStop() {
        super.onStop()
        Log.d(TAG, "Stop application")

    }

    override fun onDestroy() {
        super.onDestroy()
        Log.d(TAG, "Destroy application")
        nodeEnvHandler?.destroy()
    }

    private fun getJsEntry(): String {
        return BuildConfig.JS_ENTRY
    }
}
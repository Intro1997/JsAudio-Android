package com.example.jsaudio

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.example.audio.AudioHandle
import com.example.jsaudio.databinding.ActivityMainBinding
import com.example.node_env.NodeEnvHandle

class MainActivity : AppCompatActivity() {
    companion object {
        private const val TAG = "MainActivity"
    }

    private lateinit var binding: ActivityMainBinding
    private var nodeEnvHandle: NodeEnvHandle? = null
    private var audioHandle: AudioHandle? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d(TAG, "Create application")

        nodeEnvHandle = NodeEnvHandle.create()
        audioHandle = AudioHandle()

        val ret = "create node env " + if (nodeEnvHandle == null) "failed" else "success"

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        binding.sampleText.text = ret
    }

    override fun onStart() {
        super.onStart()
        Log.d(TAG, "Start application")
        nodeEnvHandle?.evalCode(getJsEntry())
    }

    override fun onResume() {
        super.onResume()
        Log.d(TAG, "Resume application")
        nodeEnvHandle?.resume()
    }


    override fun onPause() {
        super.onPause()
        Log.d(TAG, "Pause application")
        nodeEnvHandle?.pause()
    }


    override fun onStop() {
        super.onStop()
        Log.d(TAG, "Stop application")

    }

    override fun onDestroy() {
        super.onDestroy()
        Log.d(TAG, "Destroy application")
        nodeEnvHandle?.destroy()
    }

    private fun getJsEntry(): String {
        return BuildConfig.JS_ENTRY
    }
}
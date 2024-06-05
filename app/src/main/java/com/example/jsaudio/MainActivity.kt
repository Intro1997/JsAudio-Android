package com.example.jsaudio

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.example.jsaudio.databinding.ActivityMainBinding
import com.example.nodeenvlib.NodeEnvHandle

class MainActivity : AppCompatActivity() {
    companion object {
        private const val TAG = "MainActivity"
    }

    private lateinit var binding: ActivityMainBinding
    private var nodeEnvHandle: NodeEnvHandle? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d(TAG, "Create application")

        nodeEnvHandle = NodeEnvHandle.Create()

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
        nodeEnvHandle?.Resume()
    }


    override fun onPause() {
        super.onPause()
        Log.d(TAG, "Pause application")
        nodeEnvHandle?.Pause()
    }


    override fun onStop() {
        super.onStop()
        Log.d(TAG, "Stop application")

    }

    override fun onDestroy() {
        super.onDestroy()
        Log.d(TAG, "Destroy application")
        nodeEnvHandle?.Destroy()
    }

    private fun getJsEntry(): String {
        return BuildConfig.JS_ENTRY
    }
}
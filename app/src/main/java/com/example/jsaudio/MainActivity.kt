package com.example.jsaudio

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import com.example.jsaudio.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    companion object {
        private val TAG = "MainActivity"

        // Used to load the 'jsaudio' library on application startup.
        init {
            System.loadLibrary("jsaudio")
        }
    }

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d(TAG, "Create application")
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = getNativeNodeInitState()
    }

    override fun onStart() {
        super.onStart()
        Log.d(TAG, "Start application")
    }

    override fun onResume() {
        super.onResume()
        Log.d(TAG, "Resume application")
    }


    override fun onPause() {
        super.onPause()
        Log.d(TAG, "Pause application")
    }


    override fun onStop() {
        super.onStop()
        Log.d(TAG, "Stop application")

    }

    override fun onDestroy() {
        super.onDestroy()
        Log.d(TAG, "Destroy application")
        destroyNativeNode();
    }

    /**
     * A native method that is implemented by the 'jsaudio' native library,
     * which is packaged with this application.
     */
    private external fun getNativeNodeInitState(): String

    /**
     * native node manager api
     */
    private external fun destroyNativeNode(): Unit
}
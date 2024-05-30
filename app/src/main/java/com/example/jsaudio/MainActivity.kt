package com.example.jsaudio

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import com.example.jsaudio.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d("MainActivity", "Create application")
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()
    }

    override fun onStart() {
        super.onStart()
        Log.d("MainActivity", "Start application")
    }

    override fun onResume() {
        super.onResume()
        Log.d("MainActivity", "Resume application")
    }


    override fun onPause() {
        super.onPause()
        Log.d("MainActivity", "Pause application")
    }


    override fun onStop() {
        super.onStop()
        Log.d("MainActivity", "Stop application")
    }

    override fun onDestroy() {
        super.onDestroy()
        Log.d("MainActivity", "Destroy application")
    }

    /**
     * A native method that is implemented by the 'jsaudio' native library,
     * which is packaged with this application.
     */
    private external fun stringFromJNI(): String

    companion object {
        // Used to load the 'jsaudio' library on application startup.
        init {
            System.loadLibrary("jsaudio")
        }
    }
}
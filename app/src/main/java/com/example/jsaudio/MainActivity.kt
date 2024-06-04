package com.example.jsaudio

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.example.jsaudio.databinding.ActivityMainBinding
import java.io.BufferedInputStream
import java.io.BufferedReader
import java.lang.Exception
import java.net.HttpURLConnection
import java.net.URL


class MainActivity : AppCompatActivity() {
    companion object {
        private const val TAG = "MainActivity"

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

        val jsThread = Thread {
            run {
                if (createNativeNode()) {
                    runOnUiThread { run { binding.sampleText.text = getNativeNodeInitState() } }
                    val fileContent = loadFileFromHttpUrl(getJsEntry())
                    if (fileContent.isNotEmpty()) {
                        Log.d(TAG, "load file success, file content: $fileContent")
                        evalCode(fileContent)
                    }
                }

            }
        }
        jsThread.name = "JS Thread"
        jsThread.start()
    }

    override fun onStart() {
        super.onStart()
        Log.d(TAG, "Start application")
    }

    override fun onResume() {
        super.onResume()
        resumeNativeNode()
        Log.d(TAG, "Resume application")
    }


    override fun onPause() {
        super.onPause()
        pauseNativeNode()
        Log.d(TAG, "Pause application")
    }


    override fun onStop() {
        super.onStop()
        Log.d(TAG, "Stop application")

    }

    override fun onDestroy() {
        super.onDestroy()
        Log.d(TAG, "Destroy application")
        destroyNativeNode()
    }


    private fun loadFileFromHttpUrl(url: String): String {
        var ret = ""
        try {
            val urlObj = URL(url)
            val urlConnection = urlObj.openConnection() as HttpURLConnection

            try {
                val inputStream = BufferedInputStream(urlConnection.inputStream)
                ret = inputStream.bufferedReader().use(BufferedReader::readText)
            } finally {
                urlConnection.disconnect()
            }
        } catch (e: Exception) {
            Log.e(TAG, "Load file from url <$url> failed! msg: ${e.message}")
        }
        return ret
    }

    private fun getJsEntry(): String {
        return BuildConfig.JS_ENTRY
    }

    /**
     * A native method that is implemented by the 'jsaudio' native library,
     * which is packaged with this application.
     */
    private external fun getNativeNodeInitState(): String

    /**
     * native node manager api
     */
    private external fun createNativeNode(): Boolean

    private external fun pauseNativeNode()

    private external fun resumeNativeNode()

    private external fun destroyNativeNode()

    private external fun evalCode(codeStr: String)
}
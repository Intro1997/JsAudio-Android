package com.example.jsaudio

import com.jakewharton.processphoenix.ProcessPhoenix
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import com.example.audio.AudioHandler
import com.example.jsaudio.databinding.ActivityMainBinding
import com.example.node_env.NodeEnvHandler


class MainActivity : AppCompatActivity() {
    companion object {
        private const val TAG = "MainActivity"
        private const val JS_ENTRY = "index.js"
    }

    enum class JsThreadState(val state: Int) {
        Running(0x1),
        Stop(0x2),
        Pause(0x3)
    }

    private lateinit var binding: ActivityMainBinding
    private var nodeEnvHandler: NodeEnvHandler? = null
    private var stopButton: Button? = null
    private var startButton: Button? = null
    private var pauseButton: Button? = null
    private var resumeButton: Button? = null
    private var buttonTestJsState: JsThreadState = JsThreadState.Stop

    private fun getJsLifetimeControlButton(): Boolean {
        stopButton = findViewById(R.id.stop_button)
        startButton = findViewById(R.id.start_button)
        pauseButton = findViewById(R.id.pause_button)
        resumeButton = findViewById(R.id.resume_button)
        return stopButton != null &&
                startButton != null &&
                pauseButton != null &&
                resumeButton != null

    }

    private fun updateJsLifetimeControlButtonState(
        state: NodeEnvHandler.NodeEnvState?
    ) {
        var aimState = NodeEnvHandler.NodeEnvState.Stop
        state?.let { aimState = it }
        when (aimState) {
            NodeEnvHandler.NodeEnvState.Pause -> {
                stopButton?.isEnabled = true
                startButton?.isEnabled = false
                pauseButton?.isEnabled = false
                resumeButton?.isEnabled = true
            }

            NodeEnvHandler.NodeEnvState.Running -> {
                stopButton?.isEnabled = true
                startButton?.isEnabled = false
                pauseButton?.isEnabled = true
                resumeButton?.isEnabled = false
            }

            NodeEnvHandler.NodeEnvState.Stop -> {
                stopButton?.isEnabled = false
                startButton?.isEnabled = false
                pauseButton?.isEnabled = false
                resumeButton?.isEnabled = false
            }

            NodeEnvHandler.NodeEnvState.Init -> {
                stopButton?.isEnabled = false
                startButton?.isEnabled = true
                pauseButton?.isEnabled = false
                resumeButton?.isEnabled = false
            }

        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d(TAG, "Create application")


        NodeEnvHandler.registerNodeModuleHandler(AudioHandler(this))
        nodeEnvHandler = NodeEnvHandler.create(getServerAddress())

        val ret = "create node env " + if (nodeEnvHandler == null) "failed" else "success"

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        binding.sampleText.text = ret

        nodeEnvHandler?.setJsEntry(JS_ENTRY)

        getJsLifetimeControlButton()
        updateJsLifetimeControlButtonState(
            nodeEnvHandler?.getNodeEnvState()
        )
    }

    fun onStartClick(view: View) {
        Log.d(TAG, "Click Start Button")
        nodeEnvHandler?.start()
        buttonTestJsState = JsThreadState.Running
        updateJsLifetimeControlButtonState(
            nodeEnvHandler?.getNodeEnvState()
        )
    }

    fun onRestartClick(view: View) {
        ProcessPhoenix.triggerRebirth(view.context)
    }

    fun onStopClick(view: View) {
        Log.d(TAG, "Click Stop Button")
        nodeEnvHandler?.stop()
        buttonTestJsState = JsThreadState.Stop
        updateJsLifetimeControlButtonState(
            nodeEnvHandler?.getNodeEnvState()
        )
    }

    fun onPauseClick(view: View) {
        Log.d(TAG, "Click Pause Button")
        nodeEnvHandler?.pause()
        buttonTestJsState = JsThreadState.Pause
        updateJsLifetimeControlButtonState(
            nodeEnvHandler?.getNodeEnvState()
        )
    }

    fun onResumeClick(view: View) {
        Log.d(TAG, "Click Resume Button")
        nodeEnvHandler?.resume()
        buttonTestJsState = JsThreadState.Running
        updateJsLifetimeControlButtonState(
            nodeEnvHandler?.getNodeEnvState()
        )
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
        nodeEnvHandler?.destroy()
    }

    private fun getServerAddress(): String {
        return BuildConfig.SERVER_ADDRESS
    }
}
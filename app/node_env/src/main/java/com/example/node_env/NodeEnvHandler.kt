package com.example.node_env

import java.net.HttpURLConnection
import android.util.Log
import java.net.URL
import java.io.BufferedInputStream
import java.io.BufferedReader

class NodeEnvHandler private constructor() {
    companion object {
        init {
            System.loadLibrary("node_env")
        }

        private const val TAG = "NodeEnvHandle"

        /**
         * innerNodeEnvHandle is relative to isNativeNodeEnvCreated
         * when isNativeNodeEnvCreated is false, innerNodeEnvHandle MUST be null
         * when isNativeNodeEnvCreated is true, innerNodeEnvHandle MUST valid
         */
        private var innerNodeEnvHandler: NodeEnvHandler? = null
        private var isNativeNodeEnvCreated: Boolean = false
        private var isEvaluatingCode = ThreadSafeBool(false)

        fun create(): NodeEnvHandler? {
            if (innerNodeEnvHandler == null) {
                innerNodeEnvHandler = NodeEnvHandler()
                innerNodeEnvHandler?.let { nodeEnvHandle ->
                    isNativeNodeEnvCreated = nodeEnvHandle.createNativeNode()
                }
            }

            if (!isNativeNodeEnvCreated) {
                innerNodeEnvHandler = null
            }

            return innerNodeEnvHandler
        }
    }


    fun evalCode(jsEntry: String) {
        if (isEvaluatingCode.getValue()) {
            Log.w(TAG, "Cannot run code, node env is busy!")
        } else {
            innerNodeEnvHandler?.let { nodeEnvHandle ->
                val jsThread = Thread {
                    run {
                        val fileContent = nodeEnvHandle.loadFileFromHttpUrl(jsEntry)
                        if (fileContent.isNotEmpty()) {
                            Log.d(TAG, "load file success, file content: $fileContent")
                            isEvaluatingCode.setValue(true)
                            nodeEnvHandle.nativeEvalCode(fileContent)
                            isEvaluatingCode.setValue(false)
                        }
                    }
                }
                jsThread.name = "JS Thread"
                jsThread.start()
            }
        }
    }

    fun pause() {
        innerNodeEnvHandler?.pauseNativeNode()
    }

    fun resume() {
        innerNodeEnvHandler?.resumeNativeNode()
    }

    fun destroy() {
        innerNodeEnvHandler?.let { nodeEnvHandle ->
            nodeEnvHandle.destroyNativeNode()
            isNativeNodeEnvCreated = false
            isEvaluatingCode.setValue(false)
        }
        innerNodeEnvHandler = null
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

    private external fun createNativeNode(): Boolean

    private external fun pauseNativeNode()

    private external fun resumeNativeNode()

    private external fun destroyNativeNode()

    private external fun nativeEvalCode(codeStr: String)

}
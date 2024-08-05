package com.example.node_env

import java.net.HttpURLConnection
import android.util.Log
import java.net.URL
import java.io.BufferedInputStream
import java.io.BufferedReader


class NodeEnvHandler private constructor() : NodeModuleHandler {
    enum class NodeEnvState(val state: Int) {
        Running(0x1),
        Stop(0x2),
        Pause(0x3)
    }

    companion object {
        init {
            System.loadLibrary("node_env")
        }

        private const val TAG = "NodeEnvHandle"
        private var jsEntry = ""
        private var jsEntryFile = ""
        private var nodeEnvState = NodeEnvState.Stop

        /**
         * innerNodeEnvHandle is relative to isNativeNodeEnvCreated
         * when isNativeNodeEnvCreated is false, innerNodeEnvHandle MUST be null
         * when isNativeNodeEnvCreated is true, innerNodeEnvHandle MUST valid
         */
        private var innerNodeEnvHandler: NodeEnvHandler? = null
        private var registeredModuleHandler: MutableList<NodeModuleHandler> = mutableListOf()
        private var isNativeNodeEnvCreated: Boolean = false
        private var isEvaluatingCode = ThreadSafeBool(false)

        fun registerNodeModuleHandler(handler: NodeModuleHandler) {
            registeredModuleHandler.add(handler)
        }

        fun create(jsEntry: String): NodeEnvHandler? {
            this.jsEntry = jsEntry
            if (innerNodeEnvHandler == null) {
                innerNodeEnvHandler = NodeEnvHandler()
                innerNodeEnvHandler?.let { nodeEnvHandle ->
                    isNativeNodeEnvCreated =
                        nodeEnvHandle.createNativeNode(getAllModulePreloadScript())
                }
            }

            if (!isNativeNodeEnvCreated) {
                innerNodeEnvHandler = null
            }

            return innerNodeEnvHandler
        }

        private fun getAllModulePreloadScript(): String {
            var allPreloadScript = ""
            registeredModuleHandler.forEach { handler ->
                allPreloadScript += handler.getPreloadScript()
            }
            return allPreloadScript
        }

        @JvmStatic
        fun loadFileFromJsEntry(entryFile: String): String {
            val url = jsEntry + entryFile
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
    }

    fun setJsEntryFile(filePath: String) {
        jsEntryFile = filePath
    }

    fun getNodeEnvState(): NodeEnvState {
        return nodeEnvState
    }

    private fun evalCodeFromEntryFile(jsEntryFile: String) {
        if (isEvaluatingCode.getValue()) {
            Log.w(TAG, "Cannot run code, node env is busy!")
        } else {
            Thread.currentThread()
            innerNodeEnvHandler?.let { nodeEnvHandle ->
                val jsThread = Thread {
                    run {
                        val fileContent = loadFileFromJsEntry(jsEntryFile)
                        if (fileContent.isNotEmpty()) {
//                            Log.d(TAG, "load file success, file content: $fileContent")
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

    override fun start() {
        if (jsEntryFile != "") {
            evalCodeFromEntryFile(jsEntryFile)
            nodeEnvState = NodeEnvState.Running
        }
    }

    override fun stop() {
        innerNodeEnvHandler?.pauseNativeNode()
        registeredModuleHandler.forEach { handler ->
            handler.stop()
        }
        nodeEnvState = NodeEnvState.Stop
    }

    override fun pause() {
        innerNodeEnvHandler?.pauseNativeNode()
        registeredModuleHandler.forEach { handler ->
            handler.pause()
        }
        nodeEnvState = NodeEnvState.Pause
    }

    override fun resume() {
        innerNodeEnvHandler?.resumeNativeNode()
        registeredModuleHandler.forEach { handler ->
            handler.resume()
        }
        nodeEnvState = NodeEnvState.Running
    }

    override fun destroy() {
        registeredModuleHandler.forEach { handler ->
            handler.destroy()
        }
        innerNodeEnvHandler?.let { nodeEnvHandle ->
            nodeEnvHandle.destroyNativeNode()
            isNativeNodeEnvCreated = false
            isEvaluatingCode.setValue(false)
        }
        innerNodeEnvHandler = null
    }


    private external fun createNativeNode(preloadScript: String): Boolean

    private external fun pauseNativeNode()

    private external fun resumeNativeNode()

    private external fun destroyNativeNode()

    private external fun nativeEvalCode(codeStr: String)

}
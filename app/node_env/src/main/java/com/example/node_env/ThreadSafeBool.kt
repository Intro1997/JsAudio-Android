package com.example.node_env

import java.util.concurrent.locks.ReentrantLock

class ThreadSafeBool(var innerValue: Boolean = false) {
    private val innerReentrantLock = ReentrantLock()

    fun getValue(): Boolean {
        innerReentrantLock.lock()
        val retVal = innerValue
        innerReentrantLock.unlock()
        return retVal
    }

    fun setValue(newValue: Boolean) {
        innerReentrantLock.lock()
        innerValue = newValue
        innerReentrantLock.unlock()
    }
}
from scipy.signal import sawtooth, square
import numpy as np
import math
import os
import sys


def CreateSineWave(frequency, sampleRate, sign, sampleSize):
    compute_freq = frequency * 2 * math.pi
    xn = np.arange(0, compute_freq * sampleSize /
                   sampleRate, compute_freq / sampleRate)
    yn = sign * np.sin(xn)
    return yn


def CreateSawToothWave(frequency, sampleRate, sign, sampleSize):
    compute_freq = frequency * 2 * math.pi
    xn = np.arange(0, compute_freq * sampleSize /
                   sampleRate, compute_freq / sampleRate)
    yn = sign * sawtooth(xn + math.pi, 1.0)
    return yn


def CreateTriangleWave(frequency, sampleRate, sign, sampleSize):
    compute_freq = frequency * 2 * math.pi
    xn = np.arange(0, compute_freq * sampleSize /
                   sampleRate, compute_freq / sampleRate)
    yn = sign * sawtooth(xn + math.pi / 2, 0.5)
    return yn


def CreateSquareWave(frequency, sampleRate, sign, sampleSize):
    compute_freq = frequency * 2 * math.pi
    xn = np.arange(0, compute_freq * sampleSize /
                   sampleRate, compute_freq / sampleRate)
    yn = sign * square(xn, 0.5)
    return yn


def MakeFilePathValid(filePath):
    folderPath = os.path.dirname(filePath)
    if not os.path.exists(folderPath):
        os.makedirs(folderPath)


def WriteDataToFile(data, filePath):
    MakeFilePathValid(filePath)
    with open(filePath, 'w') as file:
        np.savetxt(filePath, np.array(data), fmt='%lf')


def ProduceWaveDataToFile(SineWavePath, SawToothWavePath, TriangleWavePath, SquareWavePath, frequency, sampleRate, sign, sampleSize):
    WriteDataToFile(CreateSineWave(frequency, sampleRate,
                    sign, sampleSize), SineWavePath)
    WriteDataToFile(CreateSawToothWave(
        frequency, sampleRate, sign, sampleSize), SawToothWavePath)
    WriteDataToFile(CreateTriangleWave(
        frequency, sampleRate, sign, sampleSize), TriangleWavePath)
    WriteDataToFile(CreateSquareWave(
        frequency, sampleRate, sign, sampleSize), SquareWavePath)


args = sys.argv[1:]
if len(args) < 4:
    print("Need 4 parameter<frequency, sampleRate, sign, sampleSize> at least, but get %r paramters" % (args))
    exit(1)

try:
    frequency, sampleRate, sign, sampleSize = [
        float(arg) for arg in sys.argv[1:]]
    sign = 1.0 if sign > 0.0 else -1.0
    ProduceWaveDataToFile("data/sine.d", "data/sawtooth.d",
                          "data/triangle.d", "data/square.d", frequency, sampleRate, sign, sampleSize)
except ValueError:
    print("Error: frequency, sampleRate, sign need to be number!\n")

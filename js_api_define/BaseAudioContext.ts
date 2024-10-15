interface BaseAudioContext {
  readonly destination: AudioDestinationNode
  /**
   * inner type: float
   * data unit: hz
   */
  readonly sampleRate: number
  /**
   * inner type: double
   * precision: microsecond
   * description: 
   * - AudioContext currentTime use real time 
   * - OfflineAudioContext currentTime use:
   *    (bufferLength / 128 + (bufferLength%128 == 0) ? 0 : 1)
   */
  readonly currentTime: number

  createBuffer(
    numberOfChannels: number /* unsigned long  */,
    length: number /* unsigned long  */,
    sampleRate: number /* float */
  ): AudioBuffer

  createBufferSource(): AudioBufferSourceNode
  createDelay(maxDelayTime?: number /* default double 1.0 */): DelayNode
  createGain(): GainNode
  createOscillator(): OscillatorNode
}

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

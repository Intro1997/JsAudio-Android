interface AudioBuffer {
  // constructor: (options: AudioBufferOptions)
  readonly sampleRate: number; // float
  readonly length: number; // unsigned long
  readonly duration: number; // double
  readonly numberOfChannels: number; // unsigned long

  getChannelData: (channel: number /* unsigned long */) => Float32Array;
  copyFromChannel(
    destination: Float32Array,
    channelNumber: number /* unsigned long  */,
    bufferOffset?: number /* optional unsigned long  */
  ): void;
  copyToChannel(
    source: Float32Array,
    channelNumber: number /* unsigned long  */,
    bufferOffset?: number /* optional unsigned long  */
  ): void;
}

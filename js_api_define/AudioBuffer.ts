interface AudioBuffer {
  new(options: AudioBufferOptions): AudioBuffer;
  readonly sampleRate: number; // float
  readonly length: number; // uint32_t
  readonly duration: number; // double
  readonly numberOfChannels: number; // uint32_t

  getChannelData: (channel: number /* uint32_t */) => Float32Array;
  copyFromChannel(
    destination: Float32Array,
    channelNumber: number /* uint32_t  */,
    bufferOffset?: number /* optional uint32_t  */
  ): void;
  copyToChannel(
    source: Float32Array,
    channelNumber: number /* uint32_t  */,
    bufferOffset?: number /* optional uint32_t  */
  ): void;
}

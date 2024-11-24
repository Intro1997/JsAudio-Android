interface AudioBufferSourceNode extends AudioScheduledSourceNode {
  new(
    context: BaseAudioContext,
    options?: AudioBufferSourceOptions
  ): AudioBufferSourceNode;

  buffer: AudioBuffer | null;
  readonly detune: AudioParam;
  loop: boolean;
  loopEnd: number;
  loopStart: number;
  readonly playbackRate: AudioParam;
  /**
   *
   * @param when decide when to start
   * @param offset decide begin time of buffer;
   * if offset less than loop_start, start from offset
   * if offset large than loop_end, start from loop_start
   * @param duration decide playing time
   * if (duration + offset > loop_start) and (offset < loop_end), start playing from offset time, playing duration seconds
   * if (duration + offset > loop_start) and (offset >= loop_end), start playing from loop_start time, playing duration seconds
   * if (duration + offset <= loop_start), playing empty sound
   */
  start(when?: number, offset?: number, duration?: number): void;
}

interface GainOptions extends AudioNodeOptions {
  gain?: number;
}

interface GainNode extends AudioNode {
  constructor(context: BaseAudioContext, options?: GainOptions);
  readonly gain: AudioParam;
};
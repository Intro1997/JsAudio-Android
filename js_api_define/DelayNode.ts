interface DelayOptions extends AudioNodeOptions {
  delayTime?: /* float */ number;
  maxDelayTime?: /* float */ number;
}

interface DelayNode extends AudioNode {
  new(context: BaseAudioContext, options?: DelayOptions): DelayNode;
  readonly delayTime: AudioParam;
}


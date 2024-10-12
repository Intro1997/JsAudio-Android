interface OscillatorNode extends AudioScheduledSourceNode {
  // @ts-ignore
  type: "sawtooth" | "sine" | "square" | "triangle"; // not support custom now
  readonly frequency: AudioParam
  /**
   * max value and min value refers to following link
   * https://source.chromium.org/chromium/chromium/src/+/main:third_party/blink/renderer/modules/webaudio/oscillator_node.cc;l=77-79;drc=f522344e45882da4c7f7cb1b3a0a7bd747d654bb;bpv=0;bpt=1#:~:text=73-,74,-75
   */
  readonly detune: AudioParam
};
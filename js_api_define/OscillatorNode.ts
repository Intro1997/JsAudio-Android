// NOTE: You should not expect the output of oscillator node 
// to change based on the channelCount property.
// "An oscillator is always mono." refers to 
// https://source.chromium.org/chromium/chromium/src/+/main:third_party/blink/renderer/modules/webaudio/oscillator_handler.cc;drc=0fba000db58b7966ef2b2e9202dabba44f13d62a;l=34

// wed do not support custom now
// @ts-ignore
type OscillatorType = /* "custom" | */ "sawtooth" | "sine" | "square" | "triangle";

interface OscillatorOptions extends AudioNodeOptions {
  detune?: number;
  frequency?: number;

  /* we do not support periodicWave */
  // periodicWave?: PeriodicWave;

  // @ts-ignore
  type?: OscillatorType;
}

interface OscillatorNode extends AudioScheduledSourceNode {
  new(context: BaseAudioContext, options?: OscillatorOptions): OscillatorNode;

  // @ts-ignore
  type: OscillatorType;
  readonly frequency: AudioParam
  /**
   * max value and min value refers to following link
   * https://source.chromium.org/chromium/chromium/src/+/main:third_party/blink/renderer/modules/webaudio/oscillator_node.cc;l=77-79;drc=f522344e45882da4c7f7cb1b3a0a7bd747d654bb;bpv=0;bpt=1#:~:text=73-,74,-75
   */
  readonly detune: AudioParam
};
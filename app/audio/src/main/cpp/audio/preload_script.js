globalThis.AudioContext = process._linkedBinding("audio").AudioContext;
globalThis.OfflineAudioContext =
  process._linkedBinding("audio").OfflineAudioContext;
globalThis.OscillatorNode = process._linkedBinding("audio").OscillatorNode;
globalThis.AudioParam = process._linkedBinding("audio").AudioParam;
globalThis.AudioBuffer = process._linkedBinding("audio").AudioBuffer;
globalThis.GainNode = process._linkedBinding("audio").GainNode;
globalThis.DelayNode = process._linkedBinding("audio").DelayNode;

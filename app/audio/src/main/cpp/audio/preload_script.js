globalThis.AudioContext = process._linkedBinding("audio").AudioContext;
globalThis.OfflineAudioContext =
  process._linkedBinding("audio").OfflineAudioContext;

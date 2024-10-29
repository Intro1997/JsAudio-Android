interface AudioContext extends BaseAudioContext {
  new(): AudioContext
  resume(): Promise<undefined>
  suspend(): Promise<undefined>
  close(): Promise<undefined>
};
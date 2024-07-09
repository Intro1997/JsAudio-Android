interface AudioContext extends BaseAudioContext {
  constructor()
  resume(): Promise<undefined>
  suspend(): Promise<undefined>
  close(): Promise<undefined>
};
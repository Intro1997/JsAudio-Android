// we do not implement an actual Event, 
// we just try our best to minimize the 
// discrepancies between implementation and standards
interface OfflineAudioCompletionEvent {
  readonly renderedBuffer: AudioBuffer;
}

interface AddEventListenerOptions {
  once?: boolean;
  passive?: boolean; // we do not implement this
  signal?: AbortSignal; // we do not implement this
}

interface OfflineAudioContextOptions {
  length: number;
  numberOfChannels?: number;
  sampleRate: number;
}

interface OfflineAudioContext extends BaseAudioContext {
  new(contextOptions: OfflineAudioContextOptions): OfflineAudioContext
  new(
    numberOfChannels: number /* unsigned long */,
    length: number /* unsigned long */,
    sampleRate: number /* number */
  ): OfflineAudioContext

  startRendering(): Promise<AudioBuffer>;

  readonly length: number /* unsigned long */;
}

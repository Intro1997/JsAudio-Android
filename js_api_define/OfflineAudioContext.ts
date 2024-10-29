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

  // @ts-ignore
  oncomplete(ev: OfflineAudioCompletionEvent)

  startRendering(): Promise<AudioBuffer>;
  suspend(suspendTime: number /* double  */): Promise<undefined>;
  resume(): Promise<undefined>;


  addEventListener<K extends keyof OfflineAudioContextEventMap>(
    type: K,
    listener: (
      ev: OfflineAudioContextEventMap[K] /* OfflineAudioCompletionEvent */
    ) => any,
    options?: boolean | AddEventListenerOptions
  ): void;

  addEventListener(
    type: string,
    listener: EventListenerOrEventListenerObject,
    options?: boolean | AddEventListenerOptions
  ): void;

  removeEventListener<K extends keyof OfflineAudioContextEventMap>(
    type: K,
    listener: (
      ev: OfflineAudioContextEventMap[K] /* OfflineAudioCompletionEvent */
    ) => any,
    options?: boolean | EventListenerOptions
  ): void;

  removeEventListener(
    type: string,
    listener: EventListenerOrEventListenerObject,
    options?: boolean | EventListenerOptions
  ): void;

  readonly length: number /* unsigned long */;
}

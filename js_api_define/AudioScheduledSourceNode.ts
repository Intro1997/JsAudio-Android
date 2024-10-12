interface AudioScheduledSourceNode extends AudioNode {
  start(when?: number /* double, default 0 */): void
  stop(when?: number /* double, default 0 */): void
};
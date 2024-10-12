interface AudioParam {
  value: number /* float */
  // @ts-ignore
  readonly automationRate: AutomationRate // currently only support a-rate 
  readonly defaultValue: number /* float */
  readonly minValue: number /* float */
  readonly maxValue: number /* float */

  // TODO: Currently not support following api
  // AudioParam setValueAtTime (float value, double startTime);
  // AudioParam linearRampToValueAtTime (float value, double endTime);
  // AudioParam exponentialRampToValueAtTime (float value, double endTime);
  // AudioParam setTargetAtTime (float target, double startTime, float timeConstant);
  // AudioParam setValueCurveAtTime (sequence<float> values,
  //                                 double startTime,
  //                                 double duration);
  // AudioParam cancelScheduledValues (double cancelTime);
  // AudioParam cancelAndHoldAtTime (double cancelTime);
};
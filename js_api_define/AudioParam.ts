interface AudioParam {
  value: number /* float */
  // @ts-ignore
  readonly automationRate: AutomationRate // currently only support a-rate 
  readonly defaultValue: number /* float */
  readonly minValue: number /* float */
  readonly maxValue: number /* float */
};
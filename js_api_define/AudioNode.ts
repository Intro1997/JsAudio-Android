interface AudioNode {
  /**
   * 
   * @param destinationNode 
   * @returns AudioNode
   * @description: connect current node to destinationNode.
   *               If connection failed, function will throw 
   *               exception.
   */
  connect(destinationNode: AudioNode): AudioNode

  disconnect()
  readonly context: BaseAudioContext

  /**
   * inner type: unsigned long
   */
  readonly numberOfInputs: number

  /**
   * inner type: unsigned long
   */
  readonly numberOfOutputs: number

  /**
   * inner type: unsigned long
   */
  // @ts-ignore
  readonly channelCount: number

  // @ts-ignore
  readonly channelCountMode: ChannelCountMode
};
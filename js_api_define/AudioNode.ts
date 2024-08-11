interface AudioNode {
  // in chrome, audioNode.context return AudioContext object
  // so we use AudioContext here
  // @ts-ignore
  readonly context: AudioContext

  /**
   * inner type: uint32_t
   */
  readonly numberOfInputs: number

  /**
   * inner type: uint32_t
   */
  readonly numberOfOutputs: number

  /**
   * inner type: uint32_t
   * default value: 2
   */
  // @ts-ignore
  readonly channelCount: number

  /**
   * inner type: string
   * default value: "explicit"
   */
  // @ts-ignore
  readonly channelCountMode: ChannelCountMode

  /**
   * 
   * @param destinationNode 
   * @returns AudioNode
   * @description: connect current node to destinationNode.
   *               If connection failed, function will throw 
   *               exception.
   */
  connect(destinationNode: AudioNode): AudioNode

  disconnect(): void

};
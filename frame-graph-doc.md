# FrameGraph
## Architecture
1. FrameGraph
   * FrameGraphRenderPassCache
   * FrameGraphFrameBufferCache
   * FrameGraphResourceManager
2. FrameGraphResource
   * FrameGraphResourceHandle
3. FrameGraphTask
   * ResourceTransitionTask
   * RenderPassTask

+ AttachmentDesc
## Details
1. FrameGraphTaskType
   * ResourceTransition
   * RenderPass
   * ComputePass
2. FrameGraphResourceType
   * Attachment
   * Buffer
3. FrameGraphResourceFlags
   * None
   * Present
   * idk, now

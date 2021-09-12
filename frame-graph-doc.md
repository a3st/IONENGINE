# FrameGraph
## Architecture
1. FrameGraph
   * FrameGraphRenderPassCache
   * FrameGraphFrameBufferCache
   * FrameGraphResourceManager
2. FrameGraphResource
   * FrameGraphResourceHandle
3. FrameGraphRenderPass
4. FrameGraphTask
   * ResourceTransitionTask
   * RenderPassExecutorTask

+ AttachmentDesc
## Details
1. FrameGraphTaskType
   * ResourceTransition
   * RenderPassExecutor
2. FrameGraphResourceType
   * Attachment
   * Buffer
3. FrameGraphResourceFlags
   * None
   * Present
   * idk, now

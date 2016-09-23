
#include "RenderAPI.h"
#include "PlatformBase.h"


// Metal implementation of RenderAPI.


#if SUPPORT_METAL

#include "Unity/IUnityGraphicsMetal.h"
#import <Metal/Metal.h>
#import <Metal/MTLDevice.h>


class RenderAPI_Metal : public RenderAPI
{
public:
    RenderAPI_Metal();
    virtual ~RenderAPI_Metal() { }
    
    virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);
    
    virtual void DrawPrimitiveImmediate(int triangleCount, int vertexSize, const void* data);
    
    virtual void* BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch);
    virtual void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr);
    
private:
    void CreateResources();
    
private:
    IUnityGraphicsMetal*	m_MetalGraphics;
    id<MTLBuffer>			m_VertexBuffer;
    id<MTLBuffer>			m_ConstantBuffer;
    
    id<MTLDepthStencilState> m_DepthStencil;
    id<MTLRenderPipelineState>	m_Pipeline;
};


RenderAPI* CreateRenderAPI_Metal()
{
    return new RenderAPI_Metal();
}

static id<MTLDevice> metalDevice;


void RenderAPI_Metal::CreateResources()
{
    //id<MTLDevice> metalDevice = device;
    NSError* error = nil;
    
    // Vertex / Constant buffers
#	if UNITY_OSX
    MTLResourceOptions bufferOptions = MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeManaged;
#	else
    MTLResourceOptions bufferOptions = MTLResourceOptionCPUCacheModeDefault;
#	endif
    m_VertexBuffer = [metalDevice newBufferWithLength:1024 options:bufferOptions];
    m_VertexBuffer.label = @"PluginVB";
}


RenderAPI_Metal::RenderAPI_Metal()
{
}


void RenderAPI_Metal::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
    if (type == kUnityGfxDeviceEventInitialize)
    {
#if UNITY_OSX
        m_MetalGraphics = interfaces->Get<IUnityGraphicsMetal>();
        NSBundle* metalBundle = m_MetalGraphics->MetalBundle();
#else
        NSBundle* metalBundle = UnityGetMetalBundle();
#endif
        
#if UNITY_OSX
        metalDevice = m_MetalGraphics->MetalDevice();
#else
        metalDevice = (__bridge id<MTLDevice>)((void*)interfaces);
#endif
        
        CreateResources();
        
    }
    else if (type == kUnityGfxDeviceEventShutdown)
    {
        //@TODO: release resources
    }
}


void RenderAPI_Metal::DrawPrimitiveImmediate(int triangleCount, int vertexSize, const void* data)
{
    // Update vertex and constant buffers
    //@TODO: we don't do any synchronization here :)
    
    const int vbSize = triangleCount * 3 * vertexSize;
    
    ::memcpy(m_VertexBuffer.contents, data, vbSize);
    
#if UNITY_OSX
    [m_VertexBuffer didModifyRange:NSMakeRange(0, vbSize)];
#endif

#if UNITY_OSX
    id<MTLRenderCommandEncoder> cmd = (id<MTLRenderCommandEncoder>)m_MetalGraphics->CurrentCommandEncoder();
#else
    id<MTLRenderCommandEncoder> cmd = UnityCurrentMTLCommandEncoder();//(id<MTLRenderCommandEncoder>)m_MetalGraphics->CurrentCommandEncoder();
#endif
 
    // Bind buffers
    [cmd setVertexBuffer:m_VertexBuffer offset:0 atIndex:1];
    
    // Draw
    [cmd drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:triangleCount*3];
}


void* RenderAPI_Metal::BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch)
{
    const int rowPitch = textureWidth * 4;
    // Just allocate a system memory buffer here for simplicity
    unsigned char* data = new unsigned char[rowPitch * textureHeight];
    *outRowPitch = rowPitch;
    return data;
}


void RenderAPI_Metal::EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr)
{
    id<MTLTexture> tex = (__bridge id<MTLTexture>)textureHandle;
    // Update texture data, and free the memory buffer
    [tex replaceRegion:MTLRegionMake3D(0,0,0, textureWidth,textureHeight,1) mipmapLevel:0 withBytes:dataPtr bytesPerRow:rowPitch];
    delete[](unsigned char*)dataPtr;
}


#endif // #if SUPPORT_METAL

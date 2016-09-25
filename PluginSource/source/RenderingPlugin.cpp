// Example low level rendering Unity plugin

#include "PlatformBase.h"
#include "RenderAPI.h"

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <vector>

// --------------------------------------------------------------------------
// UnitySetInterfaces

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;

extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    s_UnityInterfaces = unityInterfaces;
    s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
    s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
    
    // Run OnGraphicsDeviceEvent(initialize) manually on plugin load
    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
    s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}



// --------------------------------------------------------------------------
// GraphicsDeviceEvent


static RenderAPI* s_CurrentAPI = NULL;
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;


static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    // Create graphics API implementation upon initialization
    if (eventType == kUnityGfxDeviceEventInitialize)
    {
        assert(s_CurrentAPI == NULL);
        s_DeviceType = s_Graphics->GetRenderer();
        s_CurrentAPI = CreateRenderAPI(s_DeviceType);
    }
    
    // Let the implementation process the device related events
    if (s_CurrentAPI)
    {
        s_CurrentAPI->ProcessDeviceEvent(eventType, s_UnityInterfaces);
    }
    
    // Cleanup graphics API implementation upon shutdown
    if (eventType == kUnityGfxDeviceEventShutdown)
    {
        delete s_CurrentAPI;
        s_CurrentAPI = NULL;
        s_DeviceType = kUnityGfxRendererNull;
    }
}


struct MyVertex2
{
	float x, y, z;
	float u, v;
};


MyVertex2 verts2[4] =
{
	{ -0.5f, -0.5f,  0, 0, 0 },
	{ -0.5f, 0.5f ,  0, 0, 1 },
	{ 0.5f, -0.5f,  0, 1, 0 },
	{ 0.5f, 0.5f,   0, 1, 1 },
};

unsigned short indice2[6] =
{
	0,1,2,1,3,2,
};

std::vector<IMesh*> garbageReq;
void GarbageBufferRequest(IMesh* name)
{
	garbageReq.push_back(name);
}

IMesh* mesh = NULL;
extern "C" void UNITY_INTERFACE_API OnRenderEvent(int eventID)
{
    // Unknown / unsupported graphics device type? Do nothing
    if (s_CurrentAPI == NULL)
        return;

	switch (eventID)
	{
	case 0:
	{
		for (unsigned int i = 0; i < garbageReq.size(); ++i)
		{
			//s_CurrentAPI->GarbageBufferRequest(garbageReq[i]);
			SAFE_DELETE(garbageReq[i]);
		}
		garbageReq.clear();
	}
	case 999:
	{
		// Testing rendering a quad
		if (mesh == NULL)
		{
			mesh = CreateMesh(4 * sizeof(MyVertex2), RenderAPI::eVDE_Position | RenderAPI::eVDE_Texcoord);
			IMeshPart* meshpart = mesh->addPart(RenderAPI::TRIANGLES, RenderAPI::INDEX16, 6);

			mesh->setVertexData(verts2, 0, sizeof(MyVertex2) * 4);
			meshpart->setIndexData(indice2, 0, 6);
		}

		if (mesh)
		{
			int tri_stats = 0;
			int vert_stats = 0;
			mesh->render(tri_stats, vert_stats);
		}
	}
		
		break;
	default:
		break;
	}
}

typedef	void	(*UnityPluginSetGraphicsDeviceFunc)(void* device, int deviceType, int eventType);
typedef	void	(*UnityPluginRenderMarkerFunc)(int marker);

extern "C" void UnitySetGraphicsDevice_iOS(void* device, int deviceType, int eventType)
{
    // ios专属的回调，对于MTL方式。device是MTLDevice
    
    // Create graphics API implementation upon initialization
    if (eventType == kUnityGfxDeviceEventInitialize)
    {
        assert(s_CurrentAPI == NULL);
        s_DeviceType = (UnityGfxRenderer)deviceType;
        s_CurrentAPI = CreateRenderAPI(s_DeviceType);
    }
    
    // Let the implementation process the device related events
    if (s_CurrentAPI)
    {
        s_CurrentAPI->ProcessDeviceEvent((UnityGfxDeviceEventType)eventType, (IUnityInterfaces*)device);
    }
    
    // Cleanup graphics API implementation upon shutdown
    if (eventType == kUnityGfxDeviceEventShutdown)
    {
        delete s_CurrentAPI;
        s_CurrentAPI = NULL;
        s_DeviceType = kUnityGfxRendererNull;
    }
}

extern "C" void UnityRenderMarker_iOS(int marker)
{
    printf("%d", marker);
}

// --------------------------------------------------------------------------
// GetRenderEventFunc, an example function we export which is used to get a rendering event callback function.

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
    return OnRenderEvent;
}


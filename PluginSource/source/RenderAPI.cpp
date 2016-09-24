#include "RenderAPI.h"
#include "PlatformBase.h"
#include "Unity/IUnityGraphics.h"
#include "Impl/CMeshGLCoreES.h"

UnityGfxRenderer g_api = kUnityGfxRendererNull;

UnityGfxRenderer GetRenderAPIType()
{
	return g_api;
}

IMesh* CreateMesh(unsigned int vertexCount, unsigned int vertexLayout)
{
	switch (g_api)
	{
		case kUnityGfxRendererOpenGLCore:
		case kUnityGfxRendererOpenGLES20:
		case kUnityGfxRendererOpenGLES30:
			return CMeshGLCoreES::createMesh(vertexCount, vertexLayout);
		break;
#if SUPPORT_METAL
        case kUnityGfxRendererMetal:
            extern IMesh* CreateMetalMesh(unsigned int vertexBufferSize, unsigned int vertexLayout);
            return CreateMetalMesh(vertexCount, vertexLayout);
#endif
		default:
			return NULL;
		break;
	}	
}

RenderAPI* CreateRenderAPI(UnityGfxRenderer apiType)
{
	g_api = apiType;

#	if SUPPORT_D3D11
	if (apiType == kUnityGfxRendererD3D11)
	{
		extern RenderAPI* CreateRenderAPI_D3D11();
		return CreateRenderAPI_D3D11();
	}
#	endif // if SUPPORT_D3D11

#	if SUPPORT_D3D9
	if (apiType == kUnityGfxRendererD3D9)
	{
		extern RenderAPI* CreateRenderAPI_D3D9();
		return CreateRenderAPI_D3D9();
	}
#	endif // if SUPPORT_D3D9

#	if SUPPORT_D3D12
	if (apiType == kUnityGfxRendererD3D12)
	{
		extern RenderAPI* CreateRenderAPI_D3D12();
		return CreateRenderAPI_D3D12();
	}
#	endif // if SUPPORT_D3D9


#	if SUPPORT_OPENGL_UNIFIED
	if (apiType == kUnityGfxRendererOpenGLCore || apiType == kUnityGfxRendererOpenGLES20 || apiType == kUnityGfxRendererOpenGLES30)
	{
		extern RenderAPI* CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType);
		return CreateRenderAPI_OpenGLCoreES(apiType);
	}
#	endif // if SUPPORT_OPENGL_UNIFIED

#	if SUPPORT_OPENGL_LEGACY
	if (apiType == kUnityGfxRendererOpenGL)
	{
		extern RenderAPI* CreateRenderAPI_OpenGL2();
		return CreateRenderAPI_OpenGL2();
	}
#	endif // if SUPPORT_OPENGL_LEGACY

#	if SUPPORT_METAL
	if (apiType == kUnityGfxRendererMetal)
	{
		extern RenderAPI* CreateRenderAPI_Metal();
		return CreateRenderAPI_Metal();
	}
#	endif // if SUPPORT_METAL


	// Unknown or unsupported graphics API
	return NULL;
}

#include "RenderAPI.h"
#include "PlatformBase.h"

// Direct3D 9 implementation of RenderAPI.

#if SUPPORT_D3D9

#include <assert.h>
#include <d3d9.h>
#include "Unity/IUnityGraphicsD3D9.h"

D3DVERTEXELEMENT9 Decl[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0 },
	{ 0,  12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0,  16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1 },
	{ 0,  28, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 2 },
	{ 0,  40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 3 },
	D3DDECL_END()
};

class RenderAPI_D3D9 : public RenderAPI
{
public:
	RenderAPI_D3D9();
	virtual ~RenderAPI_D3D9() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);

	virtual void DrawSimpleTriangles(int triangleCount, int vertexSize, const void* data);

	virtual void* BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch);
	virtual void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr);

private:
	IDirect3DDevice9* m_Device;
	// A dynamic vertex buffer just to demonstrate how to handle D3D9 device resets.
	IDirect3DVertexBuffer9* m_DynamicVB;
	IDirect3DVertexDeclaration9* m_VD;
};


RenderAPI* CreateRenderAPI_D3D9()
{
	return new RenderAPI_D3D9();
}


RenderAPI_D3D9::RenderAPI_D3D9()
	: m_Device(NULL)
	, m_DynamicVB(NULL)
	, m_VD(NULL)
{
}


void RenderAPI_D3D9::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	switch (type)
	{
	case kUnityGfxDeviceEventInitialize:
	{
		IUnityGraphicsD3D9* d3d = interfaces->Get<IUnityGraphicsD3D9>();
		m_Device = d3d->GetDevice();
	}
	// fall-through!
	case kUnityGfxDeviceEventAfterReset:
		// After device is initialized or was just reset, create the VB.
		if (m_DynamicVB == NULL)
		{
			m_Device->CreateVertexBuffer(1024, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_DynamicVB, NULL);
		}
		if (m_VD == NULL)
		{
			m_Device->CreateVertexDeclaration(Decl, &m_VD);
		}

		break;
	case kUnityGfxDeviceEventBeforeReset:
	case kUnityGfxDeviceEventShutdown:
		// Before device is reset or being shut down, release the VB.
		SAFE_RELEASE(m_DynamicVB);
		SAFE_RELEASE(m_VD);
		break;
	}
}


void RenderAPI_D3D9::DrawSimpleTriangles(int triangleCount, int vertexSize, const void* data)
{

	// Vertex layout
	//const int kVertexSize = 12 + 4; // position + color
	//m_Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	void* vbPtr;
	m_DynamicVB->Lock(0, 0, &vbPtr, D3DLOCK_DISCARD);
	memcpy(vbPtr, data, triangleCount * 3 * vertexSize);
	m_DynamicVB->Unlock();

	// set layout
	m_Device->SetVertexDeclaration(m_VD);

	// set vb
	m_Device->SetStreamSource(0, m_DynamicVB, 0, vertexSize);

	// Draw
	m_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, triangleCount);
}


void* RenderAPI_D3D9::BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch)
{
	IDirect3DTexture9* d3dtex = (IDirect3DTexture9*)textureHandle;
	assert(d3dtex);

	// Lock the texture and return pointer
	D3DLOCKED_RECT lr;
	HRESULT hr = d3dtex->LockRect(0, &lr, NULL, 0);
	if (FAILED(hr))
		return NULL;

	*outRowPitch = lr.Pitch;
	return lr.pBits;
}


void RenderAPI_D3D9::EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr)
{
	IDirect3DTexture9* d3dtex = (IDirect3DTexture9*)textureHandle;
	assert(d3dtex);

	// Unlock the texture after modification
	d3dtex->UnlockRect(0);
}


#endif // #if SUPPORT_D3D9

#pragma once

#include "Unity/IUnityGraphics.h"

struct IUnityInterfaces;


// Super-simple "graphics abstraction". This is nothing like how a proper platform abstraction layer would look like;
// all this does is a base interface for whatever our plugin sample needs. Which is only "draw some triangles"
// and "modify a texture" at this point.
//
// There are implementations of this base class for D3D9, D3D11, OpenGL etc.; see individual RenderAPI_* files.
class RenderAPI
{
public:
	enum IndexFormat
	{
		INDEX8,
		INDEX16,
		INDEX32,
	};

	enum PrimitiveType
	{
		TRIANGLES,
		TRIANGLE_STRIP,
		LINES,
		LINE_STRIP,
		POINTS,
	};
	// IMPORTANT!!! THAT YOUR MESH SHOULD APPLY ELEMENT LIKE THE FOLLOWING ORDER!!! -kyleyi
	enum VertecDeclElement
	{
		eVDE_Position = 1 << 0,			// float3
		eVDE_Normal = 1 << 1,			// float3
		eVDE_Texcoord = 1 << 2,			// floot2
		eVDE_Color = 1 << 3,			// dword
		eVDE_TexcoordFull = 1 << 4,			// floot4
											// etc
	};

	virtual ~RenderAPI() { }


	// Process general event like initialization, shutdown, device loss/reset etc.
	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces) = 0;


	// Draw some triangle geometry, using some simple rendering state.
	// Upon call into our plug-in the render state can be almost completely arbitrary depending
	// on what was rendered in Unity before. Here, we turn off culling, blending, depth writes etc.
	// and draw the triangles with a given world matrix. The triangle data is
	// float3 (position) and byte4 (color) per vertex.
	virtual void DrawPrimitiveImmediate(int triangleCount, int vertexSize, const void* data) = 0;


	// Begin modifying texture data. You need to pass texture width/height too, since some graphics APIs
	// (e.g. OpenGL ES) do not have a good way to query that from the texture itself...
	//
	// Returns pointer into the data buffer to write into (or NULL on failure), and pitch in bytes of a single texture row.
	virtual void* BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch) = 0;

	// End modifying texture data.
	virtual void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr) = 0;
};

struct CullBox
{
	float min[3];
	float max[3];
};

struct IMeshPart
{
	virtual void setIndexData(const void* indexData, unsigned int indexStart, unsigned int indexCount) = 0;
};

struct IMesh
{
	virtual void setVertexData(const void* vertexData, unsigned int vertexStart, unsigned int vertexBufferSize) = 0;
	virtual IMeshPart* addPart(RenderAPI::PrimitiveType primitiveType, RenderAPI::IndexFormat indexFormat, unsigned int indexCount) = 0;
	virtual unsigned int getPartCount() const = 0;
	virtual IMeshPart* getPart(unsigned int index) = 0;
	virtual const CullBox& getBoundingBox() const = 0;
	virtual void setBoundingBox(const CullBox& box) = 0;
	virtual void render(int& rendered_tri, int& rendered_vert) =0;
};

// Create a graphics API implementation instance for the given API type.
RenderAPI* CreateRenderAPI(UnityGfxRenderer apiType);
IMesh* CreateMesh(unsigned int vertexCount, unsigned int vertexLayout);
UnityGfxRenderer GetRenderAPIType();
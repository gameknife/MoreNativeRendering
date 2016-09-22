#include "RenderAPI.h"
#include "PlatformBase.h"

// OpenGL Core profile (desktop) or OpenGL ES (mobile) implementation of RenderAPI.
// Supports several flavors: Core, ES2, ES3


#if SUPPORT_OPENGL_UNIFIED


#include <assert.h>
#if UNITY_IPHONE
#	include <OpenGLES/ES2/gl.h>
#elif UNITY_ANDROID
#	include <GLES2/gl2.h>
#else
#	include "GLEW/glew.h"
#endif


class RenderAPI_OpenGLCoreES : public RenderAPI
{
public:
	RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType);
	virtual ~RenderAPI_OpenGLCoreES() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);

	virtual void DrawSimpleTriangles(int triangleCount, int vertexSize, const void* data);

	virtual void* BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch);
	virtual void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr);

private:
	void CreateResources();

private:
	UnityGfxRenderer m_APIType;
	GLuint m_VertexShader;
	GLuint m_FragmentShader;
	GLuint m_Program;
	GLuint m_VertexArray;
	GLuint m_VertexBuffer;
	int m_UniformWorldMatrix;
	int m_UniformProjMatrix;
};


RenderAPI* CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
{
	return new RenderAPI_OpenGLCoreES(apiType);
}


enum VertexInputs
{
	kVertexInputPosition = 0,
	kVertexInputColor = 1,
    kVertexInputUV = 3,
};

void RenderAPI_OpenGLCoreES::CreateResources()
{

#	if SUPPORT_OPENGL_CORE
    if (m_APIType == kUnityGfxRendererOpenGLCore)
	{
		glewExperimental = GL_TRUE;
		glewInit();
		glGetError(); // Clean up error generated by glewInit
	}
#	endif // if SUPPORT_OPENGL_CORE

	// Create vertex buffer
	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STREAM_DRAW);

	assert(glGetError() == GL_NO_ERROR);
}


RenderAPI_OpenGLCoreES::RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
	: m_APIType(apiType)
{
}


void RenderAPI_OpenGLCoreES::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	if (type == kUnityGfxDeviceEventInitialize)
	{
		CreateResources();
	}
	else if (type == kUnityGfxDeviceEventShutdown)
	{
		//@TODO: release resources
	}
}


void RenderAPI_OpenGLCoreES::DrawSimpleTriangles(int triangleCount, int vertexSize, const void* data)
{
	// Core profile needs VAOs, setup one
#	if SUPPORT_OPENGL_CORE
	if (m_APIType == kUnityGfxRendererOpenGLCore)
	{
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);
	}
#	endif // if SUPPORT_OPENGL_CORE

	// Bind a vertex buffer, and update data in it
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize * triangleCount * 3, data);

	// Setup vertex layout
	glEnableVertexAttribArray(kVertexInputPosition);
	glVertexAttribPointer(kVertexInputPosition, 3, GL_FLOAT, GL_FALSE, vertexSize, (char*)NULL + 0);
     
	glEnableVertexAttribArray(kVertexInputColor);
	glVertexAttribPointer(kVertexInputColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, vertexSize, (char*)NULL + 12);
     
    glEnableVertexAttribArray(kVertexInputUV);
    glVertexAttribPointer(kVertexInputUV, 2, GL_FLOAT, GL_FALSE, vertexSize, (char*)NULL + 40);
	// Draw
	glDrawArrays(GL_TRIANGLES, 0, triangleCount * 3);

	// Cleanup VAO
#	if SUPPORT_OPENGL_CORE
	if (m_APIType == kUnityGfxRendererOpenGLCore)
	{
		glDeleteVertexArrays(1, &m_VertexArray);
	}
#	endif
}


void* RenderAPI_OpenGLCoreES::BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch)
{
	const int rowPitch = textureWidth * 4;
	// Just allocate a system memory buffer here for simplicity
	unsigned char* data = new unsigned char[rowPitch * textureHeight];
	*outRowPitch = rowPitch;
	return data;
}


void RenderAPI_OpenGLCoreES::EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr)
{
	GLuint gltex = (GLuint)(size_t)(textureHandle);
	// Update texture data, and free the memory buffer
	glBindTexture(GL_TEXTURE_2D, gltex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, dataPtr);
	delete[](unsigned char*)dataPtr;
}


#endif // #if SUPPORT_OPENGL_UNIFIED

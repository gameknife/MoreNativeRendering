#include "CMeshPartGLCoreES.h"

CMeshPartGLCoreES::CMeshPartGLCoreES() :
	_mesh(NULL), _meshIndex(0), _primitiveType(RenderAPI::TRIANGLES), _indexCount(0), _indexBuffer(0), _indexFormat(RenderAPI::INDEX16)
{
}

CMeshPartGLCoreES::~CMeshPartGLCoreES()
{
    if (_indexBuffer)
    {
#ifndef MUTE_RENDER
        glDeleteBuffers(1, &_indexBuffer);
#endif
    }

}

CMeshPartGLCoreES* CMeshPartGLCoreES::create(CMeshGLCoreES* mesh, unsigned int meshIndex, RenderAPI::PrimitiveType primitiveType,
	RenderAPI::IndexFormat indexFormat, unsigned int indexCount)
{
    // Create a VBO for our index buffer.
    GLuint vbo;
#ifndef MUTE_RENDER
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
	
    unsigned int indexSize = 0;
    switch (indexFormat)
    {
    case RenderAPI::INDEX8:
        indexSize = 1;
        break;
    case RenderAPI::INDEX16:
        indexSize = 2;
        break;
    case RenderAPI::INDEX32:
        indexSize = 4;
        break;
    default:
        //GP_ERROR("Unsupported index format (%d).", indexFormat);
        glDeleteBuffers(1, &vbo);
        return NULL;
    }


    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * indexCount, NULL, GL_STREAM_DRAW);
#endif
    CMeshPartGLCoreES* part = new CMeshPartGLCoreES();
    part->_mesh = mesh;
    part->_meshIndex = meshIndex;
    part->_primitiveType = primitiveType;
    part->_indexFormat = indexFormat;
    part->_indexCount = indexCount;
    part->_indexBuffer = vbo;

    return part;
}

unsigned int CMeshPartGLCoreES::getMeshIndex() const
{
    return _meshIndex;
}

RenderAPI::PrimitiveType CMeshPartGLCoreES::getPrimitiveType() const
{
    return _primitiveType;
}

unsigned int CMeshPartGLCoreES::getIndexCount() const
{
    return _indexCount;
}

RenderAPI::IndexFormat CMeshPartGLCoreES::getIndexFormat() const
{
    return _indexFormat;
}

IndexBufferHandle CMeshPartGLCoreES::getIndexBuffer() const
{
    return _indexBuffer;
}

void CMeshPartGLCoreES::setIndexData(const void* indexData, unsigned int indexStart, unsigned int indexCount)
{
#ifndef MUTE_RENDER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);

    unsigned int indexSize = 0;
    switch (_indexFormat)
    {
    case RenderAPI::INDEX8:
        indexSize = 1;
        break;
    case RenderAPI::INDEX16:
        indexSize = 2;
        break;
    case RenderAPI::INDEX32:
        indexSize = 4;
        break;
    default:
        return;
    }

    if (indexStart == 0 && indexCount == 0)
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * _indexCount, indexData, GL_STREAM_DRAW);
    }
    else
    {
        if (indexCount == 0)
        {
            indexCount = _indexCount - indexStart;
        }

		if (indexCount > _indexCount)
		{
			_indexCount = indexCount;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * indexCount, NULL, GL_STREAM_DRAW);
		}

        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexStart * indexSize, indexCount * indexSize, indexData);
    }
#endif
}

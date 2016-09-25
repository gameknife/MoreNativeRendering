#include "CMeshPartGLCoreES.h"

CMeshPartGLCoreES::CMeshPartGLCoreES() :
	_mesh(NULL), _meshIndex(0), _primitiveType(RenderAPI::TRIANGLES), _indexCount(0), _indexBuffer(0), _indexFormat(RenderAPI::INDEX16), indexDataWaitForFlush(NULL)
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
        return NULL;
    }
    CMeshPartGLCoreES* part = new CMeshPartGLCoreES();
    part->_mesh = mesh;
    part->_meshIndex = meshIndex;
    part->_primitiveType = primitiveType;
    part->_indexFormat = indexFormat;
    part->_indexCount = indexCount;

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
	if (indexDataWaitForFlush != NULL)
	{
		SAFE_DELETE(indexDataWaitForFlush);
	}

	indexDataWaitForFlush = malloc(indexCount * getIndexSize());
	indexDataOffset = indexStart;
	indexDataCount = indexCount;
	::memcpy(indexDataWaitForFlush, indexData, indexCount * getIndexSize());
}

unsigned int CMeshPartGLCoreES::getIndexSize()
{
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
	}
	return indexSize;
}

void CMeshPartGLCoreES::render()
{
	if (_indexBuffer == NULL)
	{
		glGenBuffers(1, &_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndexSize() * _indexCount, NULL, GL_STREAM_DRAW);
	}

#ifndef MUTE_RENDER
	if (indexDataWaitForFlush != NULL)
	{
		if (indexDataCount > _indexCount)
		{
			_indexCount = indexDataCount;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndexSize() * indexDataCount, NULL, GL_STREAM_DRAW);
		}

		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexDataOffset * getIndexSize(), indexDataCount * getIndexSize(), indexDataWaitForFlush);
		SAFE_DELETE(indexDataWaitForFlush);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
#endif

}

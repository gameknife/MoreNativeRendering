#include "GLdefine.h"
#include "CMeshPart.h"
//#include "AGOC/New/ObjectPool.h"

CMeshPart::CMeshPart() :
	_mesh(NULL), _meshIndex(0), _primitiveType(CMesh::TRIANGLES), _indexCount(0), _indexBuffer(0), _indexFormat(CMesh::INDEX16)
{
}

CMeshPart::~CMeshPart()
{
    if (_indexBuffer)
    {
#ifndef MUTE_RENDER
        glDeleteBuffers(1, &_indexBuffer);
#endif
    }

}

CMeshPart* CMeshPart::create(CMesh* mesh, unsigned int meshIndex, CMesh::PrimitiveType primitiveType,
    CMesh::IndexFormat indexFormat, unsigned int indexCount)
{
    // Create a VBO for our index buffer.
    GLuint vbo;
#ifndef MUTE_RENDER
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
	
    unsigned int indexSize = 0;
    switch (indexFormat)
    {
    case CMesh::INDEX8:
        indexSize = 1;
        break;
    case CMesh::INDEX16:
        indexSize = 2;
        break;
    case CMesh::INDEX32:
        indexSize = 4;
        break;
    default:
        //GP_ERROR("Unsupported index format (%d).", indexFormat);
        glDeleteBuffers(1, &vbo);
        return NULL;
    }


    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * indexCount, NULL, GL_STREAM_DRAW);
#endif
    CMeshPart* part = new CMeshPart();
    part->_mesh = mesh;
    part->_meshIndex = meshIndex;
    part->_primitiveType = primitiveType;
    part->_indexFormat = indexFormat;
    part->_indexCount = indexCount;
    part->_indexBuffer = vbo;

    return part;
}

unsigned int CMeshPart::getMeshIndex() const
{
    return _meshIndex;
}

CMesh::PrimitiveType CMeshPart::getPrimitiveType() const
{
    return _primitiveType;
}

unsigned int CMeshPart::getIndexCount() const
{
    return _indexCount;
}

CMesh::IndexFormat CMeshPart::getIndexFormat() const
{
    return _indexFormat;
}

IndexBufferHandle CMeshPart::getIndexBuffer() const
{
    return _indexBuffer;
}

void CMeshPart::setIndexData(const void* indexData, unsigned int indexStart, unsigned int indexCount)
{
#ifndef MUTE_RENDER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);

    unsigned int indexSize = 0;
    switch (_indexFormat)
    {
    case CMesh::INDEX8:
        indexSize = 1;
        break;
    case CMesh::INDEX16:
        indexSize = 2;
        break;
    case CMesh::INDEX32:
        indexSize = 4;
        break;
    default:
        //GP_ERROR("Unsupported index format (%d).", _indexFormat);
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

		// resize the buffer
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

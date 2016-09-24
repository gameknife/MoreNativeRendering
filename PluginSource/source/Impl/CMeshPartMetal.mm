#include "CMeshPartMetal.h"

CMeshPartMetal::CMeshPartMetal() :
	_mesh(NULL), _meshIndex(0), _primitiveType(RenderAPI::TRIANGLES), _indexCount(0), _indexBuffer(0), _indexFormat(RenderAPI::INDEX16)
{
}

CMeshPartMetal::~CMeshPartMetal()
{
    if (_indexBuffer)
    {
#ifndef MUTE_RENDER
        //glDeleteBuffers(1, &_indexBuffer);
#endif
    }

}

CMeshPartMetal* CMeshPartMetal::create(CMeshMetal* mesh, unsigned int meshIndex, RenderAPI::PrimitiveType primitiveType,
	RenderAPI::IndexFormat indexFormat, unsigned int indexCount)
{
    // Create a VBO for our index buffer.
    IndexBufferHandle vbo;
#ifndef MUTE_RENDER
    
	
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
#	if UNITY_OSX
    MTLResourceOptions bufferOptions = MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeManaged;
#	else
    MTLResourceOptions bufferOptions = MTLResourceOptionCPUCacheModeDefault;
#	endif
    vbo = [GloablGetMTLDevice() newBufferWithLength:indexSize * indexCount options:bufferOptions];
#endif
    CMeshPartMetal* part = new CMeshPartMetal();
    part->_mesh = mesh;
    part->_meshIndex = meshIndex;
    part->_primitiveType = primitiveType;
    part->_indexFormat = indexFormat;
    part->_indexCount = indexCount;
    part->_indexBuffer = vbo;

    return part;
}

unsigned int CMeshPartMetal::getMeshIndex() const
{
    return _meshIndex;
}

RenderAPI::PrimitiveType CMeshPartMetal::getPrimitiveType() const
{
    return _primitiveType;
}

unsigned int CMeshPartMetal::getIndexCount() const
{
    return _indexCount;
}

RenderAPI::IndexFormat CMeshPartMetal::getIndexFormat() const
{
    return _indexFormat;
}

IndexBufferHandle CMeshPartMetal::getIndexBuffer() const
{
    return _indexBuffer;
}

void CMeshPartMetal::setIndexData(const void* indexData, unsigned int indexStart, unsigned int indexCount)
{
#ifndef MUTE_RENDER
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);

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
    
    void* p = _indexBuffer.contents;
    p = static_cast<char*>(p) + indexStart * indexSize;
    ::memcpy(p, indexData, indexCount * indexSize);
    
    if(indexCount > _indexCount)
    {
        _indexCount = indexCount;
    }
    
#if UNITY_OSX
    [_indexBuffer didModifyRange:NSMakeRange(0, indexCount * indexSize)];
#endif
    
#endif
}

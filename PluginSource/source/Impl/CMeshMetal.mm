#include "CMeshMetal.h"
#include "CMeshPartMetal.h"
#import <Metal/Metal.h>
#import <Metal/MTLDevice.h>
//#include "CVertexAttribBindingGLCoreES.h"

#define METAL_VERTEX_STRIDE 48

#define METAL_DATAOFFSET_POS 0
#define METAL_DATAOFFSET_COL 12
#define METAL_DATAOFFSET_NOR 16
#define METAL_DATAOFFSET_TEX 40
#define METAL_DATAOFFSET_TAN 28

#define METAL_DATASIZE_POS 12
#define METAL_DATASIZE_COL 4
#define METAL_DATASIZE_NOR 12
#define METAL_DATASIZE_TEX 8
#define METAL_DATASIZE_TAN 12

IMesh* CreateMetalMesh(unsigned int vertexBufferSize, unsigned int vertexLayout)
{
    return CMeshMetal::createMesh(vertexBufferSize, vertexLayout);
}

CMeshMetal::CMeshMetal()
: _vertexCount(0), _vertexSize(0), _vertexBuffer(0), _primitiveType(RenderAPI::TRIANGLES),
_partCount(0), _parts(NULL), _vertexLayout((RenderAPI::VertecDeclElement)0)
{
    
}

CMeshMetal::~CMeshMetal()
{
    // delete gl object: VAO
    //SAFE_DELETE(_vertexAttributeBinding);
    
    // delete gl object: Element Buffer
    if (_parts)
    {
        for (unsigned int i = 0; i < _partCount; ++i)
        {
            SAFE_DELETE(_parts[i]);
        }
        SAFE_DELARRAY(_parts);
    }
    
    // delete gl object: VBO
    if (_vertexBuffer)
    {
#ifndef MUTE_RENDER
        //glDeleteBuffers(1, &_vertexBuffer);
        _vertexBuffer = 0;
#endif
    }
}

CMeshMetal* CMeshMetal::createMesh(unsigned int vertexBufferSize, unsigned int vertexLayout)
{
    VertexBufferHandle vbo = NULL;
    
    CMeshMetal* mesh = new CMeshMetal();
    mesh->_vertexLayout = (RenderAPI::VertecDeclElement)vertexLayout;
    mesh->_vertexCount = vertexBufferSize / mesh->getVertexSize();
    mesh->layout2dataoffset();
    
#ifndef MUTE_RENDER
    // Vertex / Constant buffers
#	if UNITY_OSX
    MTLResourceOptions bufferOptions = MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeManaged;
#	else
    MTLResourceOptions bufferOptions = MTLResourceOptionCPUCacheModeDefault;
#	endif
    
    // unity use a lazy vertex desciption, fixed 48 bytes stride, pos3 + color1 + normal3 + tangent3 + tc2
    // so vbo should like 48 x vertSize
    vbo = [GloablGetMTLDevice() newBufferWithLength:(METAL_VERTEX_STRIDE * mesh->_vertexCount) options:bufferOptions];
#endif
    
    mesh->_vertexBuffer = vbo;
    
    
    return mesh;
}

unsigned int CMeshMetal::getVertexCount() const
{
    return _vertexCount;
}

VertexBufferHandle CMeshMetal::getVertexBuffer() const
{
    return _vertexBuffer;
}

RenderAPI::PrimitiveType CMeshMetal::getPrimitiveType() const
{
    return _primitiveType;
}

RenderAPI::VertecDeclElement CMeshMetal::getVertexLayout() const
{
    return _vertexLayout;
}

void CMeshMetal::layout2dataoffset()
{
    int offset = 0;
    if (getVertexLayout() & RenderAPI::eVDE_Position)
    {
        dataoffset.pos = offset;
        offset += 12;
    }
    if (getVertexLayout() & RenderAPI::eVDE_Color)
    {
        dataoffset.color = offset;
        offset += 4;
    }
    if (getVertexLayout() & RenderAPI::eVDE_Normal)
    {
        dataoffset.normal = offset;
        offset += 12;
    }
    if (getVertexLayout() & RenderAPI::eVDE_Texcoord)
    {
        dataoffset.texcoord = offset;
        offset += 8;
    }
    if (getVertexLayout() & RenderAPI::eVDE_TexcoordFull)
    {
        dataoffset.texcoord = offset;
        offset += 16;
    }
}

int CMeshMetal::getVertexSize()
{
    if(_vertexSize == 0)
    {
        int offset = 0;
        if (getVertexLayout() & RenderAPI::eVDE_Position)
        {
            offset += 12;
        }
        if (getVertexLayout() & RenderAPI::eVDE_Color)
        {
            offset += 4;
        }
        if (getVertexLayout() & RenderAPI::eVDE_Normal)
        {
            offset += 12;
        }
        if (getVertexLayout() & RenderAPI::eVDE_Texcoord)
        {
            offset += 8;
        }
        if (getVertexLayout() & RenderAPI::eVDE_TexcoordFull)
        {
            offset += 16;
        }
        _vertexSize = offset;
    }
    
    return _vertexSize;
}

void CMeshMetal::setVertexData(const void* vertexData, unsigned int vertexStart, unsigned int vertexBufferSize)
{
#ifndef MUTE_RENDER
    
    
    // set data by index
    // pos
    if(true)
    {
        for(int i=0; i < _vertexCount; ++i)
        {
            void* p = _vertexBuffer.contents;
            p = static_cast<char*>(p) + METAL_VERTEX_STRIDE * i + METAL_DATAOFFSET_POS;
            
            const void* pSrc = vertexData;
            pSrc = static_cast<const char*>(pSrc) + getVertexSize() * i;
            
            ::memcpy(p, pSrc , METAL_DATASIZE_POS);
        }
    }
    
    if(dataoffset.color)
    {
        for(int i=0; i < _vertexCount; ++i)
        {
            void* p = _vertexBuffer.contents;
            p = static_cast<char*>(p) + METAL_VERTEX_STRIDE * i + METAL_DATAOFFSET_COL;
            
            const void* pSrc = vertexData;
            pSrc = static_cast<const char*>(pSrc) + getVertexSize() * i + dataoffset.color;
            
            ::memcpy(p, pSrc , METAL_DATASIZE_COL);
        }
    }
    
    if(dataoffset.normal)
    {
        for(int i=0; i < _vertexCount; ++i)
        {
            void* p = _vertexBuffer.contents;
            p = static_cast<char*>(p) + METAL_VERTEX_STRIDE * i + METAL_DATAOFFSET_NOR;
            
            const void* pSrc = vertexData;
            pSrc = static_cast<const char*>(pSrc) + getVertexSize() * i + dataoffset.normal;
            
            ::memcpy(p, pSrc, METAL_DATASIZE_NOR);
        }
    }
    
    if(dataoffset.texcoord)
    {
        for(int i=0; i < _vertexCount; ++i)
        {
            void* p = _vertexBuffer.contents;
            p = static_cast<char*>(p) + METAL_VERTEX_STRIDE * i + METAL_DATAOFFSET_TEX;
            
            const void* pSrc = vertexData;
            pSrc = static_cast<const char*>(pSrc) + getVertexSize() * i + dataoffset.texcoord;
            
            ::memcpy(p, pSrc , METAL_DATAOFFSET_TEX);
        }
    }
    
    //::memcpy(_vertexBuffer.contents, vertexData, vertexBufferSize);
    
#if UNITY_OSX
    [_vertexBuffer didModifyRange:NSMakeRange(0, METAL_VERTEX_STRIDE * _vertexCount)];
#endif
#endif
}

IMeshPart* CMeshMetal::addPart(RenderAPI::PrimitiveType primitiveType, RenderAPI::IndexFormat indexFormat, unsigned int indexCount)
{
    CMeshPartMetal* part = CMeshPartMetal::create(this, _partCount, primitiveType, indexFormat, indexCount);
    if (part)
    {
        // Increase size of part array and copy old subets into it.
        CMeshPartMetal** oldParts = _parts;
        _parts = new CMeshPartMetal*[_partCount + 1];
        for (unsigned int i = 0; i < _partCount; ++i)
        {
            _parts[i] = oldParts[i];
        }
        // Add new part to array.
        _parts[_partCount++] = part;
        // Delete old part array.
        SAFE_DELARRAY(oldParts);
    }
    return part;
    
    return NULL;
}

unsigned int CMeshMetal::getPartCount() const
{
    return _partCount;
}

IMeshPart* CMeshMetal::getPart(unsigned int index)
{
    return _parts[index];
}

const CullBoxBase& CMeshMetal::getBoundingBox() const
{
    return _boundingBox;
}

void CMeshMetal::setBoundingBox(const CullBoxBase& box)
{
    _boundingBox = box;
}

MTLPrimitiveType GetMTLPrimitiveType(RenderAPI::PrimitiveType type)
{
    switch (type)
    {
        case RenderAPI::TRIANGLES:
            return MTLPrimitiveTypeTriangle;
        case RenderAPI::TRIANGLE_STRIP:
            return MTLPrimitiveTypeTriangleStrip;
        case RenderAPI::LINES:
            return MTLPrimitiveTypeLine;
        case RenderAPI::LINE_STRIP:
            return MTLPrimitiveTypeLineStrip;
        case RenderAPI::POINTS:
            return MTLPrimitiveTypePoint;
    }
    return MTLPrimitiveTypeTriangle;
}
//
MTLIndexType GetMTLIndexType(RenderAPI::IndexFormat type)
{
    switch (type)
    {
        case RenderAPI::INDEX16:
            return MTLIndexTypeUInt16;
        case RenderAPI::INDEX32:
            return MTLIndexTypeUInt32;
    }
    return MTLIndexTypeUInt16;
}

void CMeshMetal::render( int& rendered_tri, int& rendered_vert )
{
#ifndef MUTE_RENDER
    id<MTLRenderCommandEncoder> cmd = GloablGetMTLCmdEncoder();
    
    // Bind buffers
    [cmd setVertexBuffer:_vertexBuffer offset:0 atIndex:1];
    
    // Draw
    //[cmd drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:1*3];
    
    
    
    //	if (_vertexAttributeBinding == NULL)
    //		_vertexAttributeBinding = CVertexAttribBindingGLCoreES::create(this);
    //
    //	if (_partCount == 0)
    //		return;
    //
    //	_vertexAttributeBinding->bind();
    for (unsigned int i = 0; i < _partCount; ++i)
    {
        CMeshPartMetal* part = (CMeshPartMetal*)getPart(i);
        rendered_tri = part->getIndexCount() / 3;
        rendered_vert = getVertexCount();
        [cmd drawIndexedPrimitives:GetMTLPrimitiveType(_primitiveType) indexCount:part->getIndexCount() indexType:GetMTLIndexType(part->getIndexFormat()) indexBuffer:part->_indexBuffer indexBufferOffset:0];
        
    }
    //	_vertexAttributeBinding->unbind();
#endif
}

#ifndef CMESHMETAL_H_
#define CMESHMETAL_H_

//#include "../gldefine.h"
#include "PlatformBase.h"
#include "../RenderAPI.h"
#import <Metal/Metal.h>

typedef id<MTLBuffer> IndexBufferHandle;
typedef id<MTLBuffer> VertexBufferHandle;

class CMeshPartMetal;

extern id<MTLDevice> GloablGetMTLDevice();
id<MTLRenderCommandEncoder> GloablGetMTLCmdEncoder();

struct MetalDataOffset
{
    int pos;
    int color;
    int normal;
    int tangent;
    int texcoord;
    
    MetalDataOffset()
    {
        ::memset(this, 0, sizeof(MetalDataOffset));
    }
};

class CMeshMetal : public IMesh
{
public:
    static CMeshMetal* createMesh(unsigned int vertexCount, unsigned int vertexLayout);
    
    virtual ~CMeshMetal();
    
    unsigned int getVertexCount() const;
    VertexBufferHandle getVertexBuffer() const;
    RenderAPI::PrimitiveType getPrimitiveType() const;
    RenderAPI::VertecDeclElement getVertexLayout() const;
    int getVertexSize();
    void layout2dataoffset();
    
    void setVertexData(const void* vertexData, unsigned int vertexStart, unsigned int vertexBufferSize);
    IMeshPart* addPart(RenderAPI::PrimitiveType primitiveType, RenderAPI::IndexFormat indexFormat, unsigned int indexCount);
    unsigned int getPartCount() const;
    IMeshPart* getPart(unsigned int index);
    const CullBox& getBoundingBox() const;
    void setBoundingBox(const CullBox& box);
    void render(int& rendered_tri, int& rendered_vert);
    
private:
    CMeshMetal();
    CMeshMetal(const CMeshMetal& copy);
    
    //CVertexAttribBindingGLCoreES* _vertexAttributeBinding;
    unsigned int _vertexCount;
    VertexBufferHandle _vertexBuffer;
    RenderAPI::PrimitiveType _primitiveType;
    unsigned int _partCount;
    CMeshPartMetal** _parts;
    CullBox _boundingBox;
    RenderAPI::VertecDeclElement _vertexLayout;
    
    MetalDataOffset dataoffset;
    int _vertexSize;
};

#endif

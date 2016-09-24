#ifndef CMESHPARTMETAL_H_
#define CMESHPARTMETAL_H_

#include "CMeshMetal.h"

class CMeshPartMetal : public IMeshPart
{
    friend class CMeshMetal;

public:
    ~CMeshPartMetal();
    unsigned int getMeshIndex() const;
	RenderAPI::PrimitiveType getPrimitiveType() const;

    unsigned int getIndexCount() const;
	RenderAPI::IndexFormat getIndexFormat() const;
    IndexBufferHandle getIndexBuffer() const;

    void setIndexData(const void* indexData, unsigned int indexStart, unsigned int indexCount);

private:
    CMeshPartMetal();
    CMeshPartMetal(const CMeshPartMetal& copy);

    static CMeshPartMetal* create(CMeshMetal* mesh, unsigned int meshIndex, RenderAPI::PrimitiveType primitiveType, RenderAPI::IndexFormat indexFormat, unsigned int indexCount);

    CMeshMetal* _mesh;
    unsigned int _meshIndex;
	RenderAPI::PrimitiveType _primitiveType;
	RenderAPI::IndexFormat _indexFormat;
    unsigned int _indexCount;
    IndexBufferHandle _indexBuffer;
};

#endif

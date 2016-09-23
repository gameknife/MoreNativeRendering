#ifndef CMESHPART_H_
#define CMESHPART_H_

#include "CMesh.h"

class CMeshPart
{
    friend class CMesh;

public:
    ~CMeshPart();
    unsigned int getMeshIndex() const;
    CMesh::PrimitiveType getPrimitiveType() const;

    unsigned int getIndexCount() const;
    CMesh::IndexFormat getIndexFormat() const;
    IndexBufferHandle getIndexBuffer() const;

    void setIndexData(const void* indexData, unsigned int indexStart, unsigned int indexCount);

private:
    CMeshPart();
    CMeshPart(const CMeshPart& copy);

    static CMeshPart* create(CMesh* mesh, unsigned int meshIndex, CMesh::PrimitiveType primitiveType, CMesh::IndexFormat indexFormat, unsigned int indexCount);

    CMesh* _mesh;
    unsigned int _meshIndex;
    CMesh::PrimitiveType _primitiveType;
    CMesh::IndexFormat _indexFormat;
    unsigned int _indexCount;
    IndexBufferHandle _indexBuffer;
};

#endif

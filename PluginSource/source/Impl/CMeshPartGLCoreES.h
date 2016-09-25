#ifndef CMESHPARTGLESCORE_H_
#define CMESHPARTGLESCORE_H_

#include "CMeshGLCoreES.h"

class CMeshPartGLCoreES : public IMeshPart
{
    friend class CMeshGLCoreES;

public:
    ~CMeshPartGLCoreES();
    unsigned int getMeshIndex() const;
	RenderAPI::PrimitiveType getPrimitiveType() const;

    unsigned int getIndexCount() const;
	RenderAPI::IndexFormat getIndexFormat() const;
    IndexBufferHandle getIndexBuffer() const;

    void setIndexData(const void* indexData, unsigned int indexStart, unsigned int indexCount);

	unsigned int getIndexSize();

	void render();

private:
    CMeshPartGLCoreES();
    CMeshPartGLCoreES(const CMeshPartGLCoreES& copy);

    static CMeshPartGLCoreES* create(CMeshGLCoreES* mesh, unsigned int meshIndex, RenderAPI::PrimitiveType primitiveType, RenderAPI::IndexFormat indexFormat, unsigned int indexCount);

    CMeshGLCoreES* _mesh;
    unsigned int _meshIndex;
	RenderAPI::PrimitiveType _primitiveType;
	RenderAPI::IndexFormat _indexFormat;
    unsigned int _indexCount;
    IndexBufferHandle _indexBuffer;

	void* indexDataWaitForFlush;
	unsigned int indexDataOffset;
	unsigned int indexDataCount;
};

#endif

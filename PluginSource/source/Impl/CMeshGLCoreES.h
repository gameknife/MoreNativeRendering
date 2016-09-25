#ifndef CMESHGLESCORE_H_
#define CMESHGLESCORE_H_

#include "../gldefine.h"
#include "../RenderAPI.h"

typedef GLuint IndexBufferHandle;
typedef GLuint VertexBufferHandle;

class CMeshPartGLCoreES;
class CVertexAttribBindingGLCoreES;

// enum IndexFormat
// {
// 	INDEX8 = GL_UNSIGNED_BYTE,
// 	INDEX16 = GL_UNSIGNED_SHORT,
// 	INDEX32 = GL_UNSIGNED_INT
// };
// 
// enum PrimitiveType
// {
// 	TRIANGLES = GL_TRIANGLES,
// 	TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
// 	LINES = GL_LINES,
// 	LINE_STRIP = GL_LINE_STRIP,
// 	POINTS = GL_POINTS
// };

class CMeshGLCoreES : public IMesh
{
public:
    static CMeshGLCoreES* createMesh(unsigned int vertexCount, unsigned int vertexLayout);

	virtual ~CMeshGLCoreES();

    unsigned int getVertexCount() const;
	VertexBufferHandle getVertexBuffer() const;
    RenderAPI::PrimitiveType getPrimitiveType() const;
	RenderAPI::VertecDeclElement getVertexLayout() const;
	int getVertexSize() const;

	void setVertexData(const void* vertexData, unsigned int vertexStart, unsigned int vertexBufferSize);
	IMeshPart* addPart(RenderAPI::PrimitiveType primitiveType, RenderAPI::IndexFormat indexFormat, unsigned int indexCount);
    unsigned int getPartCount() const;
	IMeshPart* getPart(unsigned int index);
    const CullBoxBase& getBoundingBox() const;
    void setBoundingBox(const CullBoxBase& box);
	void render(int& rendered_tri, int& rendered_vert);

private:
    CMeshGLCoreES();
    CMeshGLCoreES(const CMeshGLCoreES& copy);

	CVertexAttribBindingGLCoreES* _vertexAttributeBinding;
    unsigned int _vertexCount;
	VertexBufferHandle _vertexBuffer;
	RenderAPI::PrimitiveType _primitiveType;
    unsigned int _partCount;
    CMeshPartGLCoreES** _parts;
	CullBoxBase _boundingBox;
	RenderAPI::VertecDeclElement _vertexLayout;

	void* vertexDataWaitForFlush;
};

#endif

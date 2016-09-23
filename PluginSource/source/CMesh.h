#ifndef CMESH_H_
#define CMESH_H_

#include "gldefine.h"

typedef GLuint IndexBufferHandle;
typedef GLuint VertexBufferHandle;

class CMeshPart;
class CVertexAttributeBinding;

struct CullBox
{
	float min[3];
	float max[3];
};

class CMesh
{
public:
    enum IndexFormat
    {
        INDEX8 = GL_UNSIGNED_BYTE,
        INDEX16 = GL_UNSIGNED_SHORT,
        INDEX32 = GL_UNSIGNED_INT
    };

    enum PrimitiveType
    {
        TRIANGLES = GL_TRIANGLES,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
        LINES = GL_LINES,
        LINE_STRIP = GL_LINE_STRIP,
        POINTS = GL_POINTS
    };

	// IMPORTANT!!! THAT YOUR MESH SHOULD APPLY ELEMENT LIKE THE FOLLOWING ORDER!!! -kyleyi
	enum VertecDeclElement
	{
		eVDE_Position			= 1 << 0,			// float3
		eVDE_Normal				= 1 << 1,			// float3
		eVDE_Texcoord			= 1 << 2,			// floot2
		eVDE_Color				= 1 << 3,			// dword
		eVDE_TexcoordFull		= 1 << 4,			// floot4
		// etc
	};

    static CMesh* createMesh(unsigned int vertexCount, unsigned int vertexLayout);

    unsigned int getVertexCount() const;
	VertexBufferHandle getVertexBuffer() const;
    PrimitiveType getPrimitiveType() const;
	VertecDeclElement getVertexLayout() const;
	int getVertexSize() const;
    void setPrimitiveType(CMesh::PrimitiveType type);
	void setVertexData(const void* vertexData, unsigned int vertexStart, unsigned int vertexBufferSize);
    CMeshPart* addPart(PrimitiveType primitiveType, CMesh::IndexFormat indexFormat, unsigned int indexCount);
    unsigned int getPartCount() const;
    CMeshPart* getPart(unsigned int index);
    const CullBox& getBoundingBox() const;
    void setBoundingBox(const CullBox& box);


    virtual ~CMesh();

	void render(int& rendered_tri, int& rendered_vert);

private:
    CMesh();
    CMesh(const CMesh& copy);

	CVertexAttributeBinding* _vertexAttributeBinding;
    unsigned int _vertexCount;
	VertexBufferHandle _vertexBuffer;
    PrimitiveType _primitiveType;
    unsigned int _partCount;
    CMeshPart** _parts;
	CullBox _boundingBox;
	VertecDeclElement _vertexLayout;
};

#endif

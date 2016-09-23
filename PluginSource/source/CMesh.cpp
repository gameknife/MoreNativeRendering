#include "GLdefine.h"
#include "CMesh.h"
#include "CMeshPart.h"
#include "CVertexAttributeBinding.h"

CMesh::CMesh() 
: _vertexAttributeBinding(NULL), _vertexCount(0), _vertexBuffer(0), _primitiveType(TRIANGLES),
  _partCount(0), _parts(NULL), _vertexLayout((VertecDeclElement)0)
{

}

CMesh::~CMesh()
{
	// delete gl object: VAO
	SAFE_DELETE(_vertexAttributeBinding);

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
        glDeleteBuffers(1, &_vertexBuffer);
		_vertexBuffer = 0;
#endif
    }
}

CMesh* CMesh::createMesh(unsigned int vertexBufferSize, unsigned int vertexLayout)
{
    GLuint vbo;
#ifndef MUTE_RENDER
    glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STREAM_DRAW);
#endif
    CMesh* mesh = new CMesh();
    mesh->_vertexBuffer = vbo;
	mesh->_vertexLayout = (VertecDeclElement)vertexLayout;
	mesh->_vertexCount = vertexBufferSize / mesh->getVertexSize();
    return mesh;
}

unsigned int CMesh::getVertexCount() const
{
    return _vertexCount;
}

VertexBufferHandle CMesh::getVertexBuffer() const
{
    return _vertexBuffer;
}

CMesh::PrimitiveType CMesh::getPrimitiveType() const
{
    return _primitiveType;
}

CMesh::VertecDeclElement CMesh::getVertexLayout() const
{
	return _vertexLayout;
}

int CMesh::getVertexSize() const
{
	int offset = 0;
	if (getVertexLayout() & CMesh::eVDE_Position)
	{
		offset += 12;
	}
	if (getVertexLayout() & CMesh::eVDE_Normal)
	{
		offset += 12;
	}
	if (getVertexLayout() & CMesh::eVDE_Texcoord)
	{
		offset += 8;
	}
	if (getVertexLayout() & CMesh::eVDE_TexcoordFull)
	{
		offset += 16;
	}
	if (getVertexLayout() & CMesh::eVDE_Color)
	{
		offset += 4;
	}
	return offset;
}

void CMesh::setPrimitiveType(PrimitiveType type)
{
    _primitiveType = type;
}

void CMesh::setVertexData(const void* vertexData, unsigned int vertexStart, unsigned int vertexBufferSize)
{
#ifndef MUTE_RENDER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBufferSize, vertexData);
#endif
}

CMeshPart* CMesh::addPart(PrimitiveType primitiveType, IndexFormat indexFormat, unsigned int indexCount)
{
    CMeshPart* part = CMeshPart::create(this, _partCount, primitiveType, indexFormat, indexCount);
    if (part)
    {
        // Increase size of part array and copy old subets into it.
        CMeshPart** oldParts = _parts;
        _parts = new CMeshPart*[_partCount + 1];
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
}

unsigned int CMesh::getPartCount() const
{
    return _partCount;
}

CMeshPart* CMesh::getPart(unsigned int index)
{
    return _parts[index];
}
 
const CullBox& CMesh::getBoundingBox() const
{
    return _boundingBox;
}

void CMesh::setBoundingBox(const CullBox& box)
{
    _boundingBox = box;
}

void CMesh::render( int& rendered_tri, int& rendered_vert )
{
#ifndef MUTE_RENDER
	if (_vertexAttributeBinding == NULL)
		_vertexAttributeBinding = CVertexAttributeBinding::create(this);

	if (_partCount == 0)
		return;

	_vertexAttributeBinding->bind();
	for (unsigned int i = 0; i < _partCount; ++i)
	{
		CMeshPart* part = getPart(i);	
		rendered_tri = part->getIndexCount() / 3;
		rendered_vert = getVertexCount();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, part->_indexBuffer);
		glDrawElements(part->getPrimitiveType(), part->getIndexCount(), part->getIndexFormat(), 0);
	}
	_vertexAttributeBinding->unbind();
#endif
}

#include "CMeshGLCoreES.h"
#include "CMeshPartGLCoreES.h"
#include "CVertexAttribBindingGLCoreES.h"

CMeshGLCoreES::CMeshGLCoreES() 
: _vertexAttributeBinding(NULL), _vertexCount(0), _vertexBuffer(0), _primitiveType(RenderAPI::TRIANGLES),
  _partCount(0), _parts(NULL), _vertexLayout((RenderAPI::VertecDeclElement)0)
{

}

CMeshGLCoreES::~CMeshGLCoreES()
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

CMeshGLCoreES* CMeshGLCoreES::createMesh(unsigned int vertexBufferSize, unsigned int vertexLayout)
{
    GLuint vbo;
#ifndef MUTE_RENDER
    glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STREAM_DRAW);
#endif
    CMeshGLCoreES* mesh = new CMeshGLCoreES();
    mesh->_vertexBuffer = vbo;
	mesh->_vertexLayout = (RenderAPI::VertecDeclElement)vertexLayout;
	mesh->_vertexCount = vertexBufferSize / mesh->getVertexSize();
    return mesh;
}

unsigned int CMeshGLCoreES::getVertexCount() const
{
    return _vertexCount;
}

VertexBufferHandle CMeshGLCoreES::getVertexBuffer() const
{
    return _vertexBuffer;
}

RenderAPI::PrimitiveType CMeshGLCoreES::getPrimitiveType() const
{
    return _primitiveType;
}

RenderAPI::VertecDeclElement CMeshGLCoreES::getVertexLayout() const
{
	return _vertexLayout;
}

int CMeshGLCoreES::getVertexSize() const
{
	int offset = 0;
	if (getVertexLayout() & RenderAPI::eVDE_Position)
	{
		offset += 12;
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
	if (getVertexLayout() & RenderAPI::eVDE_Color)
	{
		offset += 4;
	}
	return offset;
}

void CMeshGLCoreES::setVertexData(const void* vertexData, unsigned int vertexStart, unsigned int vertexBufferSize)
{
#ifndef MUTE_RENDER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBufferSize, vertexData);
#endif
}

IMeshPart* CMeshGLCoreES::addPart(RenderAPI::PrimitiveType primitiveType, RenderAPI::IndexFormat indexFormat, unsigned int indexCount)
{
    CMeshPartGLCoreES* part = CMeshPartGLCoreES::create(this, _partCount, primitiveType, indexFormat, indexCount);
    if (part)
    {
        // Increase size of part array and copy old subets into it.
        CMeshPartGLCoreES** oldParts = _parts;
        _parts = new CMeshPartGLCoreES*[_partCount + 1];
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

unsigned int CMeshGLCoreES::getPartCount() const
{
    return _partCount;
}

IMeshPart* CMeshGLCoreES::getPart(unsigned int index)
{
    return _parts[index];
}
 
const CullBox& CMeshGLCoreES::getBoundingBox() const
{
    return _boundingBox;
}

void CMeshGLCoreES::setBoundingBox(const CullBox& box)
{
    _boundingBox = box;
}

GLenum GetGLPrimitiveType(RenderAPI::PrimitiveType type)
{
	switch (type)
	{
	case RenderAPI::TRIANGLES:
		return GL_TRIANGLES;
	case RenderAPI::TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	case RenderAPI::LINES:
		return GL_LINES;
	case RenderAPI::LINE_STRIP:
		return GL_LINE_STRIP;
	case RenderAPI::POINTS:
		return GL_POINTS;
	}
	return GL_TRIANGLES;
}

GLenum GetGLIndexType(RenderAPI::IndexFormat type)
{
	switch (type)
	{
	case RenderAPI::INDEX8:
		return GL_UNSIGNED_BYTE;
	case RenderAPI::INDEX16:
		return GL_UNSIGNED_SHORT;
	case RenderAPI::INDEX32:
		return GL_UNSIGNED_INT;
	}
	return GL_UNSIGNED_SHORT;
}

void CMeshGLCoreES::render( int& rendered_tri, int& rendered_vert )
{
#ifndef MUTE_RENDER
	if (_vertexAttributeBinding == NULL)
		_vertexAttributeBinding = CVertexAttribBindingGLCoreES::create(this);

	if (_partCount == 0)
		return;

	_vertexAttributeBinding->bind();
	for (unsigned int i = 0; i < _partCount; ++i)
	{
		CMeshPartGLCoreES* part = (CMeshPartGLCoreES*)getPart(i);
		rendered_tri = part->getIndexCount() / 3;
		rendered_vert = getVertexCount();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, part->_indexBuffer);
		glDrawElements(GetGLPrimitiveType(part->getPrimitiveType()), part->getIndexCount(), GetGLIndexType(part->getIndexFormat()), 0);
	}
	_vertexAttributeBinding->unbind();
#endif
}

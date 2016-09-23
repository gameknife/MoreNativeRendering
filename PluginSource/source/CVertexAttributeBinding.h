#ifndef CVERTEXATTRIBUTEBINDING_H_
#define CVERTEXATTRIBUTEBINDING_H_

#include "gldefine.h"

class CMesh;
class CVertexAttributeBinding
{
public:
    static CVertexAttributeBinding* create(CMesh* mesh);

    void bind();
    void unbind();

	CVertexAttributeBinding();
	~CVertexAttributeBinding();

private:
    class VertexAttribute
    {
    public:
        bool enabled;
        int size;
        GLenum type;
        bool normalized;
        unsigned int stride;
        void* pointer;
    };



    CVertexAttributeBinding& operator=(const CVertexAttributeBinding&);
    static CVertexAttributeBinding* create(CMesh* mesh, void* vertexPointer);

    void setVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalize, GLsizei stride, void* pointer);

    GLuint _handle;
    VertexAttribute* _attributes;
    CMesh* _mesh;
};

#endif

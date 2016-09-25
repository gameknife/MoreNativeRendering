#ifndef CVERTEXATTRIBUTEBINDINGGLESCORE_H_
#define CVERTEXATTRIBUTEBINDINGGLESCORE_H_

#include "../gldefine.h"

class CMeshGLCoreES;
class CVertexAttribBindingGLCoreES
{
public:
    static CVertexAttribBindingGLCoreES* create(CMeshGLCoreES* mesh);

    void bind();
    void unbind();

	CVertexAttribBindingGLCoreES();
	~CVertexAttribBindingGLCoreES();

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



    CVertexAttribBindingGLCoreES& operator=(const CVertexAttribBindingGLCoreES&);
    static CVertexAttribBindingGLCoreES* create(CMeshGLCoreES* mesh, void* vertexPointer);

    void setVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalize, GLsizei stride, void* pointer);

    GLuint _handle;
    VertexAttribute* _attributes;
    CMeshGLCoreES* _mesh;
};

#endif

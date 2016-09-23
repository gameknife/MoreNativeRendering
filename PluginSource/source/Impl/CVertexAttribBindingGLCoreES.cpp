#include "CVertexAttribBindingGLCoreES.h"
#include "CMeshGLCoreES.h"

static GLuint __maxVertexAttribs = 0;
CVertexAttribBindingGLCoreES::CVertexAttribBindingGLCoreES() :
    _handle(0), _attributes(NULL), _mesh(NULL)
{
}

CVertexAttribBindingGLCoreES::~CVertexAttribBindingGLCoreES()
{
#ifndef MUTE_RENDER
    SAFE_DELARRAY(_attributes);

    if (_handle)
    {
#if SUPPORT_OPENGL_CORE
        glDeleteVertexArrays(1, &_handle);
#endif
        _handle = 0;
    }
#endif
}

CVertexAttribBindingGLCoreES* CVertexAttribBindingGLCoreES::create(CMeshGLCoreES* mesh)
{
    CVertexAttribBindingGLCoreES* b;
    b = create(mesh, 0);

    return b;
}

// what a magic hack... holy unity3d! fix to another hacking one  -kyleyi
#define IDX_POS 0
#define IDX_NOR 2
#define IDX_TEC 3
#define IDX_COL 1
#define IDX_TEC1 4

CVertexAttribBindingGLCoreES* CVertexAttribBindingGLCoreES::create(CMeshGLCoreES* mesh, void* vertexPointer)
{
#ifndef MUTE_RENDER
    if (__maxVertexAttribs == 0)
    {
        GLint temp;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &temp);

        __maxVertexAttribs = temp;
        if (__maxVertexAttribs <= 0)
        {
            return NULL;
        }
    }

    // Create a new VertexAttributeBinding.
	CVertexAttribBindingGLCoreES* b = new CVertexAttribBindingGLCoreES();

#if SUPPORT_OPENGL_CORE
    if (mesh && glGenVertexArrays)
    {
        // Use hardware VAOs.
        glGenVertexArrays(1, &b->_handle);
        if (b->_handle == 0)
        {
            SAFE_DELETE(b);
            return NULL;
        }
        // Bind the new VAO.
        glBindVertexArray(b->_handle);
        // Bind the Mesh VBO so our glVertexAttribPointer calls use it.
        glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexBuffer());
    }
    else
#endif
    {
        // Construct a software representation of a VAO.
        VertexAttribute* attribs = new VertexAttribute[__maxVertexAttribs];
        for (unsigned int i = 0; i < __maxVertexAttribs; ++i)
        {
            // Set GL defaults
            attribs[i].enabled = GL_FALSE;
            attribs[i].size = 4;
            attribs[i].stride = 0;
            attribs[i].type = GL_FLOAT;
            attribs[i].normalized = GL_FALSE;
            attribs[i].pointer = 0;
        }
        b->_attributes = attribs;
    }

	b->_mesh = mesh;

    if (mesh)
    {
		int offset = 0;
		if (mesh->getVertexLayout() & RenderAPI::eVDE_Position)
		{
			b->setVertexAttribPointer(IDX_POS, 3, GL_FLOAT, false, mesh->getVertexSize(), (void*)(offset));
			offset += 12;
		}
		if (mesh->getVertexLayout() & RenderAPI::eVDE_Normal)
		{
			b->setVertexAttribPointer(IDX_NOR, 3, GL_FLOAT, false, mesh->getVertexSize(), (void*)(offset));
			offset += 12;
		}
		if (mesh->getVertexLayout() & RenderAPI::eVDE_Texcoord)
		{
			b->setVertexAttribPointer(IDX_TEC, 2, GL_FLOAT, false, mesh->getVertexSize(), (void*)(offset));
			offset += 8;
		}
		if (mesh->getVertexLayout() & RenderAPI::eVDE_TexcoordFull)
		{
			b->setVertexAttribPointer(IDX_TEC, 4, GL_FLOAT, false, mesh->getVertexSize(), (void*)(offset));
			offset += 16;
		}
		if (mesh->getVertexLayout() & RenderAPI::eVDE_Color)
		{
			b->setVertexAttribPointer(IDX_COL, 4, GL_UNSIGNED_BYTE, true, mesh->getVertexSize(), (void*)(offset));
			offset += 4;
		}
	}

    if (b->_handle)
    {
#if SUPPORT_OPENGL_CORE
        glBindVertexArray(0);
#endif
    }

    return b;
#else
	return NULL;
#endif
}

void CVertexAttribBindingGLCoreES::setVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalize, GLsizei stride, void* pointer)
{
#ifndef MUTE_RENDER
    if (_handle)
    {
#if SUPPORT_OPENGL_CORE
        // Hardware mode.
		glEnableVertexAttribArray(indx);
        glVertexAttribPointer(indx, size, type, normalize, stride, pointer);
#endif
    }
    else
    {
        // Software mode.   
        _attributes[indx].enabled = true;
        _attributes[indx].size = size;
        _attributes[indx].type = type;
        _attributes[indx].normalized = (normalize == GL_TRUE);
        _attributes[indx].stride = stride;
        _attributes[indx].pointer = pointer;
    }
#endif
}

void CVertexAttribBindingGLCoreES::bind()
{
#ifndef MUTE_RENDER
    if (_handle)
    {
#if SUPPORT_OPENGL_CORE
        // Hardware mode
        glBindVertexArray(_handle);
#endif
    }
    else
    {
        // Software mode
        if (_mesh)
        {
            glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_mesh->getVertexBuffer());
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        for (unsigned int i = 0; i < __maxVertexAttribs; ++i)
        {
            VertexAttribute& a = _attributes[i];
            if (a.enabled)
            {
				{
					glEnableVertexAttribArray(i);
					glVertexAttribPointer(i, a.size, a.type, a.normalized, a.stride, a.pointer);
				}
  
            }
        }
    }
#endif
}

void CVertexAttribBindingGLCoreES::unbind()
{
#ifndef MUTE_RENDER
	if (_handle)
	{
#if SUPPORT_OPENGL_CORE
		// Hardware mode
		glBindVertexArray(0);
#endif
	}
	else
	{
		if (_mesh)
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			for (unsigned int i = 1; i < __maxVertexAttribs; ++i)
			{
				VertexAttribute& a = _attributes[i];
				if (a.enabled)
				{
					glDisableVertexAttribArray(i);
				}
			}
		}
	}
#endif
}

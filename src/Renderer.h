#pragma once

#include <GL/glew.h>
#include <signal.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

# define ASSERT(x) if (!(x))  raise(SIGTRAP);
# define GLCall(x) GLClearError();\
                    x;\
                    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

//export MESA_GL_VERSION_OVERRIDE=3.3

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    void Clear();
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
};
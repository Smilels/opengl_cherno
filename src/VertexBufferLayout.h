#pragma once

#include <GL/glew.h>
#include <vector>
#include "Renderer.h"

struct  VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
            case GL_FLOAT:
                return 4;
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:
                return 1;
        }
        ASSERT(false);
    }
};

struct  VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
public:
    VertexBufferLayout(): m_Stride(0) {}

    void Push(unsigned int type, unsigned int count, unsigned char normalized)
    {
        m_Elements.push_back({type, count, normalized});
        m_Stride += count * VertexBufferElement::GetSizeOfType(type);
    }

    inline unsigned int GetStride() const { return m_Stride;}
    inline std::vector<VertexBufferElement> GetElements() const {return m_Elements;}
};
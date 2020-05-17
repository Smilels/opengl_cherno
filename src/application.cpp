#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream> //file stream
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// on my laptop, i need the following command line
// export MESA_GL_VERSION_OVERRIDE=3.3

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

//    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3));
    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3));
    GLCall((GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE));

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        GLCall(glfwTerminate());
        return -1;
    }

    /* Make the window's context current */
    GLCall(glfwMakeContextCurrent(window));

    GLCall(glfwSwapInterval(1));

    if (glewInit() != GLEW_OK)
        std::cout << "error" << std::endl;

    std::cout<< glGetString(GL_VERSION) << std::endl;
    {
        float positions[] = {
                -0.5f, -0.5f, 0.0f, 0.0f, //0
                0.5f, -0.5f, 1.0f, 0.0f, //1
                0.5f, 0.5f, 1.0f, 1.0f, //2
                -0.5f, 0.5f, 0.0f, 1.0f//3
        };

        // index buffer
        unsigned int indices[] = {
                0, 1, 2,
                2, 3, 0
        };

        // these 6 values are x,y, z left most and rightmost boundaries
        glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -1.5f, 1.5f, -1.0f, 1.0f);

        // blending determines how we combine our output color with what is already in our target buffer
        // out =  the color we output from our fragment shader (know as source)
        // target buffer =  the buffer our fragment shader is drawing to (known as destination)
//        GLCall(glEnable(GL_BLEND));
        // glBlendFunc(src, dest)
//        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        // glBendEquation(mode) mode = how we combine the src and dest; default value is GL_FUNC_ADD
        // if the pixel we're rendering from our texture is transparent, then src alpha =0, dest = 1

        // bind a vao
        // Vertex buffer is used to store vertex arrays (used with DrawArrays, DrawElements functions)
        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push(GL_FLOAT, 2, GL_FALSE);
        layout.Push(GL_FLOAT, 2, GL_FALSE);
        va.AddBuffer(vb,layout);

        // index buffer is used to store indices that index vertices when rendering via DrawElements function
        IndexBuffer ib(indices, 6);
        ib.Bind();

        Shader shader("/homeL/demo/ros_workspace/opengl_ws/src/opengl_cherno/res/shaders/uniform_texture.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 1.0f, 0.5f, 0.5f, 1.0f);
//        shader.SetUniformMat4f("u_MVP", proj);

        Texture texture("/homeL/demo/ros_workspace/opengl_ws/src/opengl_cherno/res/textures/icon.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        // unbound everything
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window)) {
            /* Render here */
            renderer.Clear();

            // bind up shader
            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.5, 0.5, 1.0);
            renderer.Draw(va, ib, shader);

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;
            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        shader.Unbind();
    }
    glfwTerminate();
    return 0;
}
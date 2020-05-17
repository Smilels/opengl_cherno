//#define GL_GLEXT_PROTOTYPES
//#include <EGL/egl.h>
//#include <GL/gl.h>
//#include <GL/glext.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <signal.h>
#include <iostream>
#include <fstream> //file stream
#include <string> // getline
#include <sstream>

# define ASSERT(x) if (!(x))  raise(SIGTRAP);
# define GLCall(x) GLClearError();\
                    x;\
                    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

//export MESA_GL_VERSION_OVERRIDE=3.3

// to ensure there is no error before the line call this function
static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

// print the error code, because the default printing is decimal but opengl error code is hex,
// so we convert that error code from decimal to hex
static void GLCheckError()
{
    while(GLenum error =  glGetError())
    {
        std::cout << "[openGL Error] = (" << std::hex << error << std:: dec << ")" << std::endl;
    }
}

// the one which is more powerful and use this function together with ASSEERT one
static bool GLLogCall(const char* function, const char* file, int line)
{
    while(GLenum error =  glGetError())
    {
        std::cout << "[openGL Error] = (" << std::hex << error << std:: dec << ")" << function <<
        " " << file << std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE=-1, VERTEX=0, FRAGMENT=1
    };
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type =ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type =ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile shader" << (type == GL_VERTEX_SHADER ? "vertex" :  "fragment")<< std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "error" << std::endl;

    std::cout<< glGetString(GL_VERSION) << std::endl;

    float positions[12] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f, 0.5f,

//            0.5f, 0.5f,
            -0.5f, 0.5f,
//            -0.5f, -0.5f,
    };

    // index buffer
    unsigned int indices[] = {
            0,1,2,
            2,3,0
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 *  sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int),indices, GL_STATIC_DRAW);

//    std::string vertexShader =
//            "#version 330 core\n"
//            "\n"
////            "attribute vec4 position;\n"
//            "layout(location=0) in vec4 position;"
//            "void main()\n"
//            "{\n"
//            "gl_Position = position;\n"
//            "}\n";
//  note: can write semicolon after main function but in the separate shader file, we cannot write this semicolon after main function
//    std::string fragmentShader =
//            "#version 330 core\n"
//            "\n"
////            "varying vec4 color;\n"
////            "layout(location=0) out vec4 color;"
//            "void main()\n"
//            "{\n"
////            "color = vec4(1.0, 0.5, 0.5, 1.0);\n"
//            "gl_FragColor = vec4(1.0, 0.5, 0.5, 1.0);\n"
//            "}\n";

    ShaderProgramSource source = ParseShader("/home/demo/ros_workspace/opengl_ws/src/opengl_cherno/res/shaders/basic.shader");
    std::cout << source.VertexSource << std::endl;
    std::cout << source.FragmentSource << std::endl;
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
//        glBegin(GL_TRIANGLES);
//        glVertex2f(-0.5f, -0.5f);
//        glVertex2f(-0.0f, 0.5f);
//        glVertex2f(0.5f, -0.5f);
//        glEnd();
//          GLClearError();
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
//          GLCheckError();
//          ASSERT(GLLogCall());
//        glDrawArrays(GL_TRIANGLES, 0, 6);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}
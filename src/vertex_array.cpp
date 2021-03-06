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

//    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3));
    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3));
    GLCall((GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE));
    // the compatibility opengl profile makes vao object 0 a default object
    // the core opengl prodile makes vao object 0 not an object at all
    // so if we use GLFW_OPENGL_CORE_PROFILE, we need to bind a vao by ourselves

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

    // bind a vao
    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 *  sizeof(float), positions, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0));

    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    ShaderProgramSource source = ParseShader("/homeL/demo/ros_workspace/opengl_ws/src/opengl_cherno/res/shaders/uniform.shader");
//    std::cout << source.VertexSource << std::endl;
//    std::cout << source.FragmentSource << std::endl;
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    GLCall(int location =glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 1.0, 0.5, 0.5, 1.0));

    // unbound everything
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // bind up shader
        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, r, 0.5, 0.5, 1.0));

        GLCall(glBindVertexArray(vao));

        // if glBindVertexArray then don't need to bind buffer
        // set up the layout of that vertex buffer
//       GLCall( glBindBuffer(GL_ARRAY_BUFFER, buffer));
//        GLCall(glEnableVertexAttribArray(0));
//        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0));

        // bind our index buffer
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (r>1.0f)
            increment = -0.05f;
        else if(r<0.0f)
            increment = 0.05f;

        r +=increment;
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}
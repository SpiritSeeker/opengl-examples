#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

// -------------------------- GL Error Logging --------------------------
#include <signal.h>

#define ASSERT(x) if (!(x)) raise(SIGTRAP);
#define GLCall(x) GLClearError();\
  x;\
  ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
  while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
  while (GLenum error = glGetError())
  {
    std::cout << "[OpenGL Error] (" << std::hex << error << std::dec << "): " << function << " " << file << ": " << line << std::endl;
    return false;
  }
  return true;
}
// ----------------------------------------------------------------------

// ------------------- Shader Compilation and Loading -------------------
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
    NONE = -1, VERTEX = 0, FRAGMENT = 1
  };

  std::string line;
  std::stringstream ss[2];
  ShaderType type = ShaderType::NONE;
  while (getline(stream, line))
  {
    if (line.find("#shader") != std::string::npos)
    {
      if (line.find("vertex") != std::string::npos)
        type = ShaderType::VERTEX;
      else if (line.find("fragment") != std::string::npos)
        type = ShaderType::FRAGMENT;
    }
    else
    {
      ss[(int)type] << line << "\n";
    }
  }

  return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
  unsigned int id;
  GLCall(id = glCreateShader(type));
  const char* src = source.c_str();
  GLCall(glShaderSource(id, 1, &src, nullptr));
  GLCall(glCompileShader(id));

  int result;
  GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
  if (result == GL_FALSE)
  {
    int length;
    GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
    char* message = (char*)alloca(length * sizeof(char));
    GLCall(glGetShaderInfoLog(id, length, &length, message));
    std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
    std::cout << message << std::endl;
    GLCall(glDeleteShader(id));
    return 0;
  }

  return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
  unsigned int program;
  GLCall(program = glCreateProgram());
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  GLCall(glAttachShader(program, vs));
  GLCall(glAttachShader(program, fs));
  GLCall(glLinkProgram(program));
  GLCall(glValidateProgram(program));

  GLCall(glDeleteShader(vs));
  GLCall(glDeleteShader(fs));

  return program;
}
// ----------------------------------------------------------------------

int main(void)
{
  // --------------------- Initialize GLFW and GLEW ---------------------
  GLFWwindow* window;

  if (!glfwInit())
    return -1;

  window = glfwCreateWindow(640, 480, "Hello World!", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  // Set glfwSwapBuffers to run once per frame render.
  // Implies, set FPS = max supported FPS
  glfwSwapInterval(1);

  if (glewInit() != GLEW_OK)
    std::cout << "Error!" << std::endl;

  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  // --------------------------------------------------------------------

  // -------------------- Data to be sent to the GPU --------------------
  float positions[] = {
    -0.5f, -0.5f,  // 0
     0.5f, -0.5f,  // 1
     0.5f,  0.5f,  // 2
    -0.5f,  0.5f   // 3
  };

  // Has to be unsigned
  unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
  };
  // --------------------------------------------------------------------

  // ----------- Creating Buffers and storing the above data ------------
  unsigned int buffer;
  GLCall(glGenBuffers(1, &buffer));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
  GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

  GLCall(glEnableVertexAttribArray(0));
  GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

  unsigned int ibo;   // Index Buffer Object
  GLCall(glGenBuffers(1, &ibo));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
  GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 2 * sizeof(unsigned int), indices, GL_STATIC_DRAW));
  // --------------------------------------------------------------------

  // --------------------------- Load Shaders ---------------------------
  ShaderProgramSource source = ParseShader("OpenGL/res/shaders/Basic.shader");
  unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
  GLCall(glUseProgram(shader));
  // --------------------------------------------------------------------

  // --------------------------- Set Uniforms ---------------------------
  int location;
  GLCall(location = glGetUniformLocation(shader, "u_Color"));
  ASSERT(location != -1);
  GLCall(glUniform4f(location, 0.0f, 0.5f, 0.9f, 1.0f));

  // Variables to change Uniforms
  float r = 0.0f;
  float increment = 0.05f;
  while (!glfwWindowShouldClose(window))
  {
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    // Uniforms are set per draw.
    // Cannot draw one triangle in one color and the other in a different color with the current setup.
    GLCall(glUniform4f(location, r, 0.5f, 0.9f, 1.0f));
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    if (r > 1.0f)
      increment = -0.05f;
    if (r < 0.0f)
      increment = 0.05f;

    r += increment;

    glfwSwapBuffers(window);

    glfwPollEvents();
  }
  // Cleanup
  GLCall(glDeleteProgram(shader));
  glfwTerminate();
  return 0;
}

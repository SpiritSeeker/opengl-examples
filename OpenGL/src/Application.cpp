#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

int main(void)
{
  // --------------------- Initialize GLFW and GLEW ---------------------
  GLFWwindow* window;

  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    -0.5f, -0.5f, 0.0f, 0.0f, // 0
     0.5f, -0.5f, 1.0f, 0.0f, // 1
     0.5f,  0.5f, 0.0f, 1.0f, // 2
    -0.5f,  0.5f, 1.0f, 1.0f  // 3
  };

  // Has to be unsigned
  unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
  };
  // --------------------------------------------------------------------

  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  // ---------------------- Creating Vertex Array -----------------------
  VertexArray va;
  // --------------------------------------------------------------------

  // -------- Creating Vertex Buffers and storing the above data --------
  VertexBuffer vb(positions, 4 * 4 * sizeof(float));

  VertexBufferLayout layout;
  layout.Push<float>(2);
  layout.Push<float>(2);
  va.AddBuffer(vb, layout);
  // --------------------------------------------------------------------

  // -------- Creating Index Buffers and storing the above data ---------
  // Tip: Index Buffers can also be bound to vao before doing glVertexAttribPointer
  IndexBuffer ib(indices, 6);
  // --------------------------------------------------------------------

  // --------------------------- Load Shaders ---------------------------
  Shader shader("OpenGL/res/shaders/Basic.shader");
  shader.Bind();
  // --------------------------------------------------------------------

  // --------------------------- Set Uniforms ---------------------------
  shader.SetUniform4f("u_Color", 0.0f, 0.5f, 0.9f, 1.0f);
  // --------------------------------------------------------------------

  Texture texture("res/textures/pic.png");
  texture.Bind();
  shader.SetUniform1i("u_Texture", 0);

  // ------------------------ Unbind everything -------------------------
  va.Unbind();
  vb.Unbind();
  ib.Unbind();
  shader.Unbind();
  // --------------------------------------------------------------------

  Renderer renderer;

  // Variables to change Uniforms
  float r = 0.0f;
  float increment = 0.05f;
  while (!glfwWindowShouldClose(window))
  {
    renderer.Clear();

    // Bind everything before drawing (in case stuff changed)
    shader.Bind();
    shader.SetUniform4f("u_Color", r, 0.5f, 0.9f, 1.0f);

    renderer.Draw(va, ib, shader);

    if (r > 1.0f)
      increment = -0.05f;
    if (r < 0.0f)
      increment = 0.05f;

    r += increment;

    glfwSwapBuffers(window);

    glfwPollEvents();
  }
  // Cleanup
  glfwTerminate();
  return 0;
}

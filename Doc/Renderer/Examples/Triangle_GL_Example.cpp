// Reference-only example: Render a colored triangle using the Gfx interfaces with an OpenGL backend.
// This is NOT compiled; copy into a sample target and replace CreateGLDevice/CreateGLContext with your real factories.

#if 1
#include "../Gfx.h"
#include <GLFW/glfw3.h>
#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <vector>
#include <memory>
#include <cassert>

using namespace SoulEngine::Gfx;

static std::shared_ptr<IDevice> CreateGLDevice();
static std::shared_ptr<IContext> CreateGLContext();

struct Vertex { float pos[3]; float color[3]; };

static const char* VS = R"GLSL(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;
out vec3 vColor;
void main(){ vColor = aColor; gl_Position = vec4(aPos,1.0); }
)GLSL";

static const char* FS = R"GLSL(
#version 330 core
in vec3 vColor; out vec4 FragColor;
void main(){ FragColor = vec4(vColor,1.0); }
)GLSL";

static GLuint Compile(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok=0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok); assert(ok);
    return s;
}

static GLuint CreateProgram()
{
    GLuint vs = Compile(GL_VERTEX_SHADER, VS);
    GLuint fs = Compile(GL_FRAGMENT_SHADER, FS);
    GLuint p = glCreateProgram();
    glAttachShader(p, vs); glAttachShader(p, fs); glLinkProgram(p);
    GLint ok=0; glGetProgramiv(p, GL_LINK_STATUS, &ok); assert(ok);
    glDeleteShader(vs); glDeleteShader(fs);
    return p;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* win = glfwCreateWindow(800, 600, "Triangle", nullptr, nullptr);
    glfwMakeContextCurrent(win);
    gladLoadGL(glfwGetProcAddress);

    auto device = CreateGLDevice();
    auto context = CreateGLContext();

    // Vertex data
    std::vector<Vertex> vertices = {
        {{-0.6f,-0.5f,0.f}, {1.f,0.f,0.f}},
        {{ 0.6f,-0.5f,0.f}, {0.f,1.f,0.f}},
        {{ 0.0f, 0.6f,0.f}, {0.f,0.f,1.f}},
    };

    BufferDesc vbDesc{}; vbDesc.size = vertices.size()*sizeof(Vertex);
    vbDesc.kind = BufferKind::Vertex; vbDesc.usage = BufferUsage::Immutable; vbDesc.bindFlags = BindFlags::VertexBuffer;
    auto vb = device->CreateBuffer(vbDesc, &SubresourceData{vertices.data(), vbDesc.size, 0});

    // Vertex layout
    VertexAttribute attrs[] = {
        { .location=0, .format=DataFormat::R32G32B32_Float, .offset=0,                .bindingSlot=0, .stepRate=1 },
        { .location=1, .format=DataFormat::R32G32B32_Float, .offset=sizeof(float)*3,  .bindingSlot=0, .stepRate=1 },
    };
    auto layout = device->CreateVertexInputLayout(attrs, 2);

    GLuint prog = CreateProgram();

    while (!glfwWindowShouldClose(win))
    {
        glfwPollEvents();
        glViewport(0,0,800,600);
        glClearColor(0.1f,0.1f,0.15f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        context->SetVertexInputLayout(layout.get());
        uint32_t stride = sizeof(Vertex); uint32_t offset = 0;
        IBuffer* bufs[] = { vb.get() };
        context->SetVertexBuffers(0, bufs, &stride, &offset, 1);
        context->Draw(3, 0);

        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
#endif

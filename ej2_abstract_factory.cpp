#include <iostream>
#include <memory>

// productos abstractos
class Window {
public:
    virtual void draw() = 0;
    virtual ~Window() {}
};

class Button {
public:
    virtual void draw() = 0;
    virtual ~Button() {}
};

class Shader {
public:
    virtual void compile() = 0;
    virtual ~Shader() {}
};

// productos opengl
class OpenGLWindow : public Window {
public:
    void draw() override {
        std::cout << "OpenGL: dibujando ventana con glClear y glViewport" << std::endl;
    }
};

class OpenGLButton : public Button {
public:
    void draw() override {
        std::cout << "OpenGL: dibujando boton con glDrawArrays" << std::endl;
    }
};

class OpenGLShader : public Shader {
public:
    void compile() override {
        std::cout << "OpenGL: compilando shader GLSL con glCompileShader" << std::endl;
    }
};

// productos vulkan
class VulkanWindow : public Window {
public:
    void draw() override {
        std::cout << "Vulkan: dibujando ventana con swapchain y render pass" << std::endl;
    }
};

class VulkanButton : public Button {
public:
    void draw() override {
        std::cout << "Vulkan: dibujando boton con vkCmdDraw" << std::endl;
    }
};

class VulkanShader : public Shader {
public:
    void compile() override {
        std::cout << "Vulkan: compilando shader a SPIR-V con vkCreateShaderModule" << std::endl;
    }
};

// fabrica abstracta, es lo unico que conoce el cliente
class RenderFactory {
public:
    virtual std::unique_ptr<Window> createWindow() = 0;
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<Shader> createShader() = 0;
    virtual ~RenderFactory() {}
};

class OpenGLFactory : public RenderFactory {
public:
    std::unique_ptr<Window> createWindow() override {
        return std::make_unique<OpenGLWindow>();
    }
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<OpenGLButton>();
    }
    std::unique_ptr<Shader> createShader() override {
        return std::make_unique<OpenGLShader>();
    }
};

class VulkanFactory : public RenderFactory {
public:
    std::unique_ptr<Window> createWindow() override {
        return std::make_unique<VulkanWindow>();
    }
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<VulkanButton>();
    }
    std::unique_ptr<Shader> createShader() override {
        return std::make_unique<VulkanShader>();
    }
};

int main() {
    std::unique_ptr<RenderFactory> factory =
        std::make_unique<OpenGLFactory>();

    auto window = factory->createWindow();
    auto button = factory->createButton();

    window->draw();
    button->draw();

    // se cambia la fabrica en tiempo de ejecucion
    factory = std::make_unique<VulkanFactory>();

    auto shader = factory->createShader();
    shader->compile();
}

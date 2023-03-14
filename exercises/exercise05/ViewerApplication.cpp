#include "ViewerApplication.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>
#include <ituGL/asset/Texture2DLoader.h>
#include <ituGL/shader/Material.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui.h>

ViewerApplication::ViewerApplication()
    : Application(1024, 1024, "Viewer demo")
    , m_cameraPosition(0, 30, 30)
    , m_cameraTranslationSpeed(20.0f)
    , m_cameraRotationSpeed(0.5f)
    , m_cameraEnabled(false)
    , m_cameraEnablePressed(false)
    , m_mousePosition(GetMainWindow().GetMousePosition(true))
{
}

void ViewerApplication::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeModel();
    InitializeCamera();
    InitializeLights();

    DeviceGL& device = GetDevice();
    device.EnableFeature(GL_DEPTH_TEST);
    device.SetVSyncEnabled(true);
}

void ViewerApplication::Update()
{
    Application::Update();

    // Update camera controller
    UpdateCamera();
}

void ViewerApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    m_model.Draw();
    RenderGUI();
}

void ViewerApplication::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void ViewerApplication::InitializeModel()
{
    // Load and build shader
    Shader vertexShader = ShaderLoader::Load(Shader::VertexShader, "shaders/blinn-phong.vert");
    Shader fragmentShader = ShaderLoader::Load(Shader::FragmentShader, "shaders/blinn-phong.frag");
    std::shared_ptr<ShaderProgram> shaderProgram = std::make_shared<ShaderProgram>();
    shaderProgram->Build(vertexShader, fragmentShader);

    // Filter out uniforms that are not material properties
    ShaderUniformCollection::NameSet filteredUniforms;
    filteredUniforms.insert("WorldMatrix");
    filteredUniforms.insert("ViewProjMatrix");
    filteredUniforms.insert("AmbientColor"); 
    filteredUniforms.insert("DiffuseReflection");
    filteredUniforms.insert("LightColor");
    filteredUniforms.insert("LightPostion");
    // 2d loader
    Texture2DLoader loader2d(Texture2DObject::Format::FormatRGBA,Texture2DObject::InternalFormat::InternalFormatRGBA8);
    loader2d.SetFlipVertical(true);
    std::vector<std::string> paths{ "models/mill/Ground_shadow.jpg", "models/mill/Ground_color.jpg", "models/mill/MillCat_color.jpg" };
    std::shared_ptr<Texture2DObject> groundShadowTexture = loader2d.LoadShared(paths[0].c_str());
    std::shared_ptr<Texture2DObject> groundColorTexture = loader2d.LoadShared(paths[1].c_str());
    std::shared_ptr<Texture2DObject> millCatTexture = loader2d.LoadShared(paths[2].c_str());

    // Create reference material
    std::shared_ptr<Material> material = std::make_shared<Material>(shaderProgram, filteredUniforms);
    material->SetUniformValue("Color", glm::vec4(1.0f));
    material -> SetUniformValue("Texture2DLoader", groundShadowTexture);

    
    // Setup function
    ShaderProgram::Location worldMatrixLocation = shaderProgram->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location viewProjMatrixLocation = shaderProgram->GetUniformLocation("ViewProjMatrix");
    ShaderProgram::Location AmbientColorLocation = shaderProgram->GetUniformLocation("AmbientColor");
    ShaderProgram::Location AmbientReflectionLocation = shaderProgram->GetUniformLocation("AmbientReflection"); 
    ShaderProgram::Location DiffuseReflectionLocation = shaderProgram->GetUniformLocation("DiffuseReflection");
    ShaderProgram::Location LightColorLocation = shaderProgram->GetUniformLocation("LightColor"); 
    ShaderProgram::Location LightPositionLocation = shaderProgram->GetUniformLocation("LightPosition");
    material->SetShaderSetupFunction([=](ShaderProgram& shaderProgram)
        {
            shaderProgram.SetUniform(worldMatrixLocation, glm::scale(glm::vec3(0.1f)));
            shaderProgram.SetUniform(viewProjMatrixLocation, m_camera.GetViewProjectionMatrix());
            shaderProgram.SetUniform(AmbientColorLocation, m_ambientColor);
            shaderProgram.SetUniform(AmbientReflectionLocation, m_ambientRefl);
            shaderProgram.SetUniform(DiffuseReflectionLocation, m_difffuseRefl);
            shaderProgram.SetUniform(LightColorLocation, m_lightColor);
            shaderProgram.SetUniform(LightPositionLocation, m_lightPosition);
            // (todo) 05.X: Set camera and light uniforms


        });

    std::shared_ptr<Material> material1 = std::make_shared<Material>(*material);
    material1->SetUniformValue("Texture2DLoader", groundColorTexture);

    std::shared_ptr<Material> material2 = std::make_shared<Material>(*material);
    material2->SetUniformValue("Texture2DLoader", millCatTexture);

    // Configure loader
    ModelLoader loader(material);
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Position, "VertexPosition");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Normal, "VertexNormal");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::TexCoord0, "VertexTexCoord");

    // Load model
    m_model = loader.Load("models/mill/Mill.obj");

    // (todo) 05.1: Load and set textures
    m_model.SetMaterial(0, material);
    m_model.SetMaterial(1, material1);
    m_model.SetMaterial(2, material2);
}

void ViewerApplication::InitializeCamera()
{
    // Set view matrix, from the camera position looking to the origin
    m_camera.SetViewMatrix(m_cameraPosition, glm::vec3(0.0f));

    // Set perspective matrix
    float aspectRatio = GetMainWindow().GetAspectRatio();
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 1000.0f);
}

void ViewerApplication::InitializeLights()
{
    // (todo) 05.X: Initialize light variables


}

void ViewerApplication::RenderGUI()
{
    m_imGui.BeginFrame();

    // (todo) 05.4: Add debug controls for light properties
    //ImGui::DragFloat("m_ambientRefl", &m_ambientRefl);


    m_imGui.EndFrame();
}

void ViewerApplication::UpdateCamera()
{
    Window& window = GetMainWindow();

    // Update if camera is enabled (controlled by SPACE key)
    {
        bool enablePressed = window.IsKeyPressed(GLFW_KEY_SPACE);
        if (enablePressed && !m_cameraEnablePressed)
        {
            m_cameraEnabled = !m_cameraEnabled;

            window.SetMouseVisible(!m_cameraEnabled);
            m_mousePosition = window.GetMousePosition(true);
        }
        m_cameraEnablePressed = enablePressed;
    }

    if (!m_cameraEnabled)
        return;

    glm::mat4 viewTransposedMatrix = glm::transpose(m_camera.GetViewMatrix());
    glm::vec3 viewRight = viewTransposedMatrix[0];
    glm::vec3 viewForward = -viewTransposedMatrix[2];

    // Update camera translation
    {
        glm::vec2 inputTranslation(0.0f);

        if (window.IsKeyPressed(GLFW_KEY_A))
            inputTranslation.x = -1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_D))
            inputTranslation.x = 1.0f;

        if (window.IsKeyPressed(GLFW_KEY_W))
            inputTranslation.y = 1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_S))
            inputTranslation.y = -1.0f;

        inputTranslation *= m_cameraTranslationSpeed;
        inputTranslation *= GetDeltaTime();

        // Double speed if SHIFT is pressed
        if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
            inputTranslation *= 2.0f;

        m_cameraPosition += inputTranslation.x * viewRight + inputTranslation.y * viewForward;
    }

    // Update camera rotation
   {
        glm::vec2 mousePosition = window.GetMousePosition(true);
        glm::vec2 deltaMousePosition = mousePosition - m_mousePosition;
        m_mousePosition = mousePosition;

        glm::vec3 inputRotation(-deltaMousePosition.x, deltaMousePosition.y, 0.0f);

        inputRotation *= m_cameraRotationSpeed;

        viewForward = glm::rotate(inputRotation.x, glm::vec3(0,1,0)) * glm::rotate(inputRotation.y, glm::vec3(viewRight)) * glm::vec4(viewForward, 0);
    }

   // Update view matrix
   m_camera.SetViewMatrix(m_cameraPosition, m_cameraPosition + viewForward);
}

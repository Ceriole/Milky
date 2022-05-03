#include <Milky.h>
#include <Milky/Core/EntryPoint.h>

#include <Platform/OpenGL/OpenGLShader.h>

#include <imgui/imgui.h>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class ExampleLayer : public Milky::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f, true)
	{
		m_VertexArray = Milky::VertexArray::Create();

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
		};

		Milky::Ref<Milky::VertexBuffer> vertexBuffer;
		vertexBuffer = Milky::VertexBuffer::Create(vertices, sizeof(vertices));
		vertexBuffer->SetLayout({
				{ Milky::ShaderDataType::Float3, "a_Position" },
				{ Milky::ShaderDataType::Float4, "a_Color" }
			});
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };
		Milky::Ref<Milky::IndexBuffer> indexBuffer;
		indexBuffer = Milky::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA = Milky::VertexArray::Create();
		float squareVertices[4 * 5] = {
			-0.5f, -0.5f, 0.0f, 0, 0,
			 0.5f, -0.5f, 0.0f, 1, 0,
			 0.5f,  0.5f, 0.0f, 1, 1,
			-0.5f,  0.5f, 0.0f, 0, 1
		};

		Milky::Ref<Milky::VertexBuffer> squareVB;
		squareVB = Milky::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
				{ Milky::ShaderDataType::Float3, "a_Position" },
				{ Milky::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Milky::Ref<Milky::IndexBuffer> squareIB;
		squareIB = Milky::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
				#version 330 core
				
				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec4 a_Color;

				uniform mat4 u_ViewProjection;
				uniform mat4 u_Transform;

				out vec3 v_Position;
				out vec4 v_Color;
				
				void main()
				{
					v_Position = a_Position;
					v_Color = a_Color;
					gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
				}
			)";

		std::string fragmentSrc = R"(
				#version 330 core
				
				layout(location = 0) out vec4 o_Color;
		
				in vec3 v_Position;
				in vec4 v_Color;

				void main()
				{
					o_Color = v_Color;
				}
			)";

		m_Shader = Milky::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

		std::string flatColorShaderVertexSrc = R"(
				#version 330 core
				
				layout(location = 0) in vec3 a_Position;

				uniform mat4 u_ViewProjection;
				uniform mat4 u_Transform;

				out vec3 v_Position;
				
				void main()
				{
					v_Position = a_Position;
					gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
				}
			)";

		std::string flatColorShaderFragmentSrc = R"(
				#version 330 core
				
				layout(location = 0) out vec4 o_Color;
		
				in vec3 v_Position;

				uniform vec3 u_Color;

				void main()
				{
					o_Color = vec4(u_Color, 1);
				}
			)";

		m_FlatColorShader = Milky::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Milky::Texture2D::Create("assets/textures/checkerboard.png");

		textureShader->Bind();
		textureShader->Set("u_Texture", 0);
		textureShader->Set("u_Color", {1,1,1,1});
	}

	void OnUpdate(Milky::Timestep ts) override
	{
		m_CameraController.OnUpdate(ts);

		Milky::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Milky::RenderCommand::Clear();

		Milky::Renderer::BeginScene(m_CameraController.GetCamera()); // Lights, cameras, action!

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		glm::vec3 redColor(0.8f, 0.2f, 0.3f);
		glm::vec3 blueColor(0.2f, 0.3f, 0.8f);

		m_FlatColorShader->Bind();
		m_FlatColorShader->Set("u_Color", m_SquareColor);
		// Square grid
		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Milky::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		// Textured squares
		m_Texture->Bind();
		Milky::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		
		// Triangle
		// Milky::Renderer::Submit(m_Shader, m_VertexArray);

		Milky::Renderer::EndScene(); // And cut!
	}

	void OnEvent(Milky::Event& event) override
	{
		m_CameraController.OnEvent(event);
		
		Milky::EventDispatcher dispatcher = Milky::EventDispatcher(event);

		dispatcher.Dispatch<Milky::KeyPressedEvent>([](Milky::KeyPressedEvent& e) {
			if (e.GetKeyCode() == ML_KEY_ESCAPE)
			{
				Milky::Application::Get().Close();
				return true;
			}
			return false;
		});
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

private:
	Milky::ShaderLibrary m_ShaderLibrary;

	Milky::Ref<Milky::Shader> m_Shader;
	Milky::Ref<Milky::VertexArray> m_VertexArray;

	Milky::Ref<Milky::Shader> m_FlatColorShader;
	Milky::Ref<Milky::VertexArray> m_SquareVA;

	Milky::Ref<Milky::Texture2D> m_Texture;

	Milky::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
};

class SandboxApp : public Milky::Application
{
public:
	SandboxApp()
	{
		// PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~SandboxApp()
	{

	}
};

Milky::Application* Milky::CreateApplication()
{
	return new SandboxApp();
}

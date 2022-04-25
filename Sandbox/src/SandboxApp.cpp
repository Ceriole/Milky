#include <Milky.h>

#include <imgui/imgui.h>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Milky::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		m_VertexArray.reset(Milky::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
		};

		std::shared_ptr<Milky::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Milky::VertexBuffer::Create(vertices, sizeof(vertices)));
		vertexBuffer->SetLayout({
				{ Milky::ShaderDataType::Float3, "a_Position" },
				{ Milky::ShaderDataType::Float4, "a_Color" }
			});
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };
		std::shared_ptr<Milky::IndexBuffer> indexBuffer;
		indexBuffer.reset(Milky::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Milky::VertexArray::Create());
		float squareVertices[3 * 7] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<Milky::VertexBuffer> squareVB;
		squareVB.reset(Milky::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
				{ Milky::ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Milky::IndexBuffer> squareIB;
		squareIB.reset(Milky::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
				#version 330 core
				
				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec4 a_Color;

				uniform mat4 u_ViewProjection;

				out vec3 v_Position;
				out vec4 v_Color;
				
				void main()
				{
					v_Position = a_Position;
					v_Color = a_Color;
					gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
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

		m_Shader.reset(new Milky::Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
				#version 330 core
				
				layout(location = 0) in vec3 a_Position;

				uniform mat4 u_ViewProjection;

				out vec3 v_Position;
				
				void main()
				{
					v_Position = a_Position;
					gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
				}
			)";

		std::string blueShaderFragmentSrc = R"(
				#version 330 core
				
				layout(location = 0) out vec4 o_Color;
		
				in vec3 v_Position;

				void main()
				{
					o_Color = vec4(0.2, 0.2, 0.8, 1);
				}
			)";

		m_BlueShader.reset(new Milky::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	void OnUpdate() override
	{
		Milky::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.2f, 1 });
		Milky::RenderCommand::Clear();

		Milky::Renderer::BeginScene(m_Camera); // Lights, cameras, action!

		Milky::Renderer::Submit(m_BlueShader, m_SquareVA);
		Milky::Renderer::Submit(m_Shader, m_VertexArray);

		Milky::Renderer::EndScene(); // And cut!
	}

	void OnEvent(Milky::Event& event) override
	{
		
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Camera Control");

		glm::vec3 camPos = m_Camera.GetPosition();
		float camRot = m_Camera.GetRotation();

		if (ImGui::BeginPopupContextItem("PosResetPopup"))
		{
			if (ImGui::Selectable("Reset")) camPos = {0,0,0};
			if (ImGui::Selectable("Reset X")) camPos.x = 0;
			if (ImGui::Selectable("Reset Y")) camPos.y = 0;
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupContextItem("RotResetPopup"))
		{
			if (ImGui::Selectable("Reset")) camRot = 0;

			ImGui::EndPopup();
		}


		ImGui::DragFloat2("Camera Position", glm::value_ptr(camPos), 0.01f, -5.0f, 5.0f);
		ImGui::OpenPopupOnItemClick("PosResetPopup", ImGuiPopupFlags_MouseButtonRight);
		ImGui::DragFloat("Camera Rotation", &camRot, 0.01f, 0.0f, 2.0f * glm::pi<float>());
		ImGui::OpenPopupOnItemClick("RotResetPopup", ImGuiPopupFlags_MouseButtonRight);

		if (ImGui::Button("Reset Camera"))
		{
			camPos = { 0,0,0 };
			camRot = 0;
		}

		m_Camera.SetPosition(camPos);
		m_Camera.SetRotation(camRot);
		
		ImGui::End();
	}
private:
	std::shared_ptr<Milky::Shader> m_Shader;
	std::shared_ptr<Milky::VertexArray> m_VertexArray;

	std::shared_ptr<Milky::Shader> m_BlueShader;
	std::shared_ptr<Milky::VertexArray> m_SquareVA;

	Milky::OrthographicCamera m_Camera;
};

class SandboxApp : public Milky::Application
{
public:
	SandboxApp()
	{
		PushLayer(new ExampleLayer());
	}

	~SandboxApp()
	{

	}
};

Milky::Application* Milky::CreateApplication()
{
	return new SandboxApp();
}

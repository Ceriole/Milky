#pragma once

#include <Milky.h>

#include "ParticleSystem.h"

class Sandbox2D : public Milky::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Milky::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Milky::Event& event) override;
private:

	// v TEMPORARY v
	Milky::Ref<Milky::VertexArray> m_SquareVA;
	Milky::Ref<Milky::Shader> m_FlatColorShader;
	Milky::OrthographicCameraController m_CameraController;
	// ^ ========= ^

	Milky::Ref<Milky::Texture2D> m_CheckerTexture;
	Milky::Ref<Milky::Texture2D> m_SpriteSheet;
	Milky::Ref<Milky::SubTexture2D> m_TextureStairs, m_TextureBarrel, m_TextureTree;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	ParticleProps m_Particle;
	ParticleSystem m_ParticleSystem;
};

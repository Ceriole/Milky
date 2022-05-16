#pragma once

#include "Milky/Renderer/Camera.h"

namespace Milky {

	const struct SceneCameraDefaults
	{
		float PerspectiveFOV = glm::radians(45.0f);
		float PerspectiveNear = 0.01f, PerspectiveFar = 1000.0f;
		float OrthographicSize = 10.0f;
		float OrthographicNear = -1.0f, OrthographicFar = 1.0f;
	} s_CameraDefaults;

	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1, _Count = 2 };
		
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }

		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }
		void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }
	private:
		void RecalculateProjection();
	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_PerspectiveFOV = s_CameraDefaults.PerspectiveFOV;
		float m_PerspectiveNear = s_CameraDefaults.PerspectiveNear, m_PerspectiveFar = s_CameraDefaults.PerspectiveFar;

		float m_OrthographicSize = s_CameraDefaults.OrthographicSize;
		float m_OrthographicNear = s_CameraDefaults.OrthographicNear, m_OrthographicFar = s_CameraDefaults.OrthographicFar;

		float m_AspectRatio = 0.0f;
	};

}

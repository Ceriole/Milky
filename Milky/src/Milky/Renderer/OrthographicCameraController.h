#pragma once

#include "Milky/Core/Timestep.h"
#include "Milky/Renderer/OrthographicCamera.h"

#include "Milky/Events/ApplicationEvent.h"
#include "Milky/Events/MouseEvent.h"

namespace Milky {

#define CAMERA_MIN_ZOOM 0.1f
#define CAMERA_MAX_ZOOM 20.0f

	struct OrhographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool mouse = true, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera() { return m_Camera; };
		const OrthographicCamera& GetCamera() const { return m_Camera; };

		float GetZoomLevel() { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = std::clamp(level, CAMERA_MIN_ZOOM, CAMERA_MAX_ZOOM); CalculateView(); };

		void SetPosition(const glm::vec3& position) { m_CameraPosition = position; };
		void SetRotation(float rotation) { m_CameraRotation = rotation; };

		const OrhographicCameraBounds& GetBounds() const { return m_Bounds; };
	private:
		void CalculateView();

		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrhographicCameraBounds m_Bounds;
		OrthographicCamera m_Camera;

		bool m_Rotation, m_Mouse;
		glm::vec3 m_CameraPosition = { 0, 0, 0 };
		float m_CameraRotation = 0.0f; // In degrees, in the anti-clockwise direction
		float m_CameraTranslationSpeed = 10.0f, m_CameraRotationSpeed = 180.0f;
		glm::vec2 m_LastMousePos = { 0, 0 };
	};

}

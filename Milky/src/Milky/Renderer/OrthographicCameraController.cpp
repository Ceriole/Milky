#include "mlpch.h"
#include "OrthographicCameraController.h"

#include "Milky/Core/Application.h"
#include "Milky/Core/Input.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Milky {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool mouse, bool rotation)
		: m_AspectRatio(aspectRatio), m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel }), m_Camera(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top), m_Rotation(rotation), m_Mouse(mouse)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		ML_PROFILE_FUNCTION();

		if (m_Mouse)
		{
			auto [x, y] = Input::GetMousePosition();

			if (m_LastMousePos.x || m_LastMousePos.y)
			{
				float dx = x - m_LastMousePos.x;
				float dy = y - m_LastMousePos.y;

				m_CameraPosition.x -= ((dx * 2.0f) / Application::Get().GetWindow().GetWidth()) * m_ZoomLevel * m_AspectRatio;
				m_CameraPosition.y += ((dy * 2.0f) / Application::Get().GetWindow().GetHeight()) * m_ZoomLevel;
			}

			if (Input::IsMouseButtonPressed(ML_MOUSE_BUTTON_MIDDLE))
				m_LastMousePos = { x, y };
			else
				m_LastMousePos = { 0, 0 };
		}
		if (Input::IsKeyPressed(ML_KEY_A))
		{
			m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(ML_KEY_D))
		{
			m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		if (Input::IsKeyPressed(ML_KEY_W))
		{
			m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(ML_KEY_S))
		{
			m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}

		m_Camera.SetPosition(m_CameraPosition);

		if (m_Rotation)
		{
			if (Input::IsKeyPressed(ML_KEY_Q))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			else if (Input::IsKeyPressed(ML_KEY_E))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			if (m_CameraRotation > 180.0f)
				m_CameraRotation -= 360.0f;
			else if (m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;

			m_Camera.SetRotation(m_CameraRotation);
		}
		
		m_CameraTranslationSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		ML_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ML_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(ML_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		CalculateView();
	}

	void OrthographicCameraController::CalculateView()
	{
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		ML_PROFILE_FUNCTION();

		const float scrWidth = (float) Application::Get().GetWindow().GetWidth();
		const float scrHeight = (float) Application::Get().GetWindow().GetHeight();
		float mx = Input::GetMouseX() / scrWidth;
		float my = Input::GetMouseY() / scrHeight;
		mx = (mx *  2.0f) - 1.0f;
		my = (my * -2.0f) + 1.0f;

		glm::vec4 preZoom = glm::vec4(mx, my, 0, 0) * glm::inverse(m_Camera.GetViewProjectionMatrix());

		SetZoomLevel(m_ZoomLevel * std::pow(1.2f, -e.GetYOffset() / 2.0f));

		glm::vec4 postZoom = glm::vec4(mx, my, 0, 0) * glm::inverse(m_Camera.GetViewProjectionMatrix());

		m_CameraPosition += glm::vec3(preZoom) - glm::vec3(postZoom);

		return false;
	}

	bool OrthographicCameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		return false;
	}

	bool OrthographicCameraController::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		ML_PROFILE_FUNCTION();

		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}

}

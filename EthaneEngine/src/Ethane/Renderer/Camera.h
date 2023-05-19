#pragma once

#include <glm/glm.hpp>

namespace Ethane {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			:m_Projection(projection) {}
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};

	// class RenderCamera
	// {
	// public:
	// 	RenderCamera() = default;
	// 	RenderCamera(const glm::mat4& projection)
	// 		:m_Projection(projection) {}
	// 	virtual ~RenderCamera() = default;
	// 
	// 	const glm::mat4& GetProjection() const { return m_Projection; }
	// protected:
	// 	float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;
	// 	glm::mat4 m_ViewMatrix;
	// 	glm::mat4 m_Projection = glm::mat4(1.0f);
	// };

}
#pragma once

#include "Milky/Renderer/Shader.h"
#include <glm/glm.hpp>

#include <unordered_map>

namespace Milky {

	class OpenGLShader : public Shader
	{
	private:
		uint32_t m_RendererID;
		mutable std::unordered_map<std::string, int> m_UniformLocationCache;
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		void UploadUniform(const std::string& name, int value);
		void UploadUniform(const std::string& name, float value);
		void UploadUniform(const std::string& name, const glm::vec2& values);
		void UploadUniform(const std::string& name, const glm::vec3& values);
		void UploadUniform(const std::string& name, const glm::vec4& values);

		void UploadUniform(const std::string& name, const glm::mat3& matrix);
		void UploadUniform(const std::string& name, const glm::mat4& matrix);
	
	private:
		int GetUniformLocation(const std::string& name) const;
	};

}

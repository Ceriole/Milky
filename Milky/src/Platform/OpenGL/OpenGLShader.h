#pragma once

#include "Milky/Renderer/Shader.h"

typedef unsigned int GLenum;

namespace Milky {

	class OpenGLShader : public Shader
	{
	private:
		uint32_t m_RendererID;
		mutable std::unordered_map<std::string, int> m_UniformLocationCache;
		std::string m_Name;
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const std::string& GetName() const override { return m_Name; };

		void Set(const std::string& name, int value) override;
		void Set(const std::string& name, int* values, uint32_t count) override;
		void Set(const std::string& name, float value) override;
		void Set(const std::string& name, float* values, uint32_t count) override;
		void Set(const std::string& name, const glm::vec2& values) override;
		void Set(const std::string& name, const glm::vec3& values) override;
		void Set(const std::string& name, const glm::vec4& values) override;
		void Set(const std::string& name, const glm::mat3& matrix) override;
		void Set(const std::string& name, const glm::mat4& matrix) override;

		void UploadUniform(const std::string& name, int value);
		void UploadUniform(const std::string& name, int* values, uint32_t count);
		void UploadUniform(const std::string& name, float value);
		void UploadUniform(const std::string& name, float* values, uint32_t count);
		void UploadUniform(const std::string& name, const glm::vec2& values);
		void UploadUniform(const std::string& name, const glm::vec3& values);
		void UploadUniform(const std::string& name, const glm::vec4& values);

		void UploadUniform(const std::string& name, const glm::mat3& matrix);
		void UploadUniform(const std::string& name, const glm::mat4& matrix);
	
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		int GetUniformLocation(const std::string& name) const;
	};

}

#include "mlpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <regex>

namespace Milky {

	const std::string typeToken = "#type";
	const std::regex typeTokenRegex = std::regex("(?:"+typeToken+")\\s+(\\w+)\\s*\\n", std::regex_constants::icase);
	const size_t typeTokenLength = std::size(typeToken) - 1;

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		ML_CORE_ASSERT(false, "Unkown shader type: '" + type + "'");
		return GL_NONE;
	}
	
	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		ML_PROFILE_FUNCTION();

		std::string shaderSrc = ReadFile(filepath);
		std::unordered_map<GLenum, std::string> sources = PreProcess(shaderSrc);
		Compile(sources);
		
		// Get name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		ML_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		ML_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		ML_PROFILE_FUNCTION();

		std::string result;
		ML_CORE_TRACE("Loading shader source file: '{0}'", filepath);
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			ML_CORE_ERROR("Failed to load shader source file: '{0}'", filepath);
		}
		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		ML_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		std::string toSearch = source;
		std::smatch match;
		while (std::regex_search(toSearch, match, typeTokenRegex))
		{
			GLenum type = ShaderTypeFromString(match[1]);
			ML_CORE_ASSERT(type, "Syntax error in shader file: Invalid '#type' keyword!");
			std::string src = match.suffix();
			src = src.substr(0, src.find(typeToken));
			shaderSources[type] = src;
			toSearch = match.suffix();
		}
		ML_CORE_ASSERT(shaderSources.size(), "Syntax error in shader file: No '#type' directive(s) found.");

		return shaderSources;
	}

	void OpenGLShader::Compile(std::unordered_map<GLenum, std::string>& shaderSources)
	{
		ML_PROFILE_FUNCTION();

		GLuint program = glCreateProgram();
		ML_CORE_ASSERT(shaderSources.size() <= 2, "Shader program asset can only contain a maximum of 2 shaders!");
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;
			
			GLuint shader = glCreateShader(type);

			const GLchar* source_cstr = source.c_str();
			glShaderSource(shader, 1, &source_cstr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				// Use the infoLog as you see fit.
				ML_CORE_ERROR("{0}", infoLog.data());
				ML_CORE_ASSERT(false, "OpenGL shader compile error!");

				return;
			}
			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		m_RendererID = program;

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);

			// Don't leak shaders either.
			for (auto id : glShaderIDs)
				glDeleteShader(id);

			// Use the infoLog as you see fit.
			ML_CORE_ERROR("{0}", infoLog.data());
			ML_CORE_ASSERT(false, "OpenGL shader program link error!");
			
			return;
		}

		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}
	}

	void OpenGLShader::Bind() const
	{
		ML_PROFILE_FUNCTION();

		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		ML_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::Set(const std::string& name, int value)
	{
		ML_PROFILE_FUNCTION();

		UploadUniform(name, value);
	}

	void OpenGLShader::Set(const std::string& name, int* values, uint32_t count)
	{
		ML_PROFILE_FUNCTION();

		UploadUniform(name, values, count);
	}

	void OpenGLShader::Set(const std::string& name, float value)
	{
		ML_PROFILE_FUNCTION();

		UploadUniform(name, value);
	}

	void OpenGLShader::Set(const std::string& name, float* values, uint32_t count)
	{
		ML_PROFILE_FUNCTION();

		UploadUniform(name, values, count);
	}

	void OpenGLShader::Set(const std::string& name, const glm::vec2& values)
	{
		ML_PROFILE_FUNCTION();

		UploadUniform(name, values);
	}

	void OpenGLShader::Set(const std::string& name, const glm::vec3& values)
	{
		ML_PROFILE_FUNCTION();

		UploadUniform(name, values);
	}

	void OpenGLShader::Set(const std::string& name, const glm::vec4& values)
	{
		ML_PROFILE_FUNCTION();

		UploadUniform(name, values);
	}

	void OpenGLShader::Set(const std::string& name, const glm::mat3& matrix)
	{
		ML_PROFILE_FUNCTION();

		UploadUniform(name, matrix);
	}

	void OpenGLShader::Set(const std::string& name, const glm::mat4& matrix)
	{
		ML_PROFILE_FUNCTION();

		UploadUniform(name, matrix);
	}

	int OpenGLShader::GetUniformLocation(const std::string& name) const
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		m_UniformLocationCache[name] = location;
		return location;
	}

	void OpenGLShader::UploadUniform(const std::string& name, int value)
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniform(const std::string& name, int* values, uint32_t count)
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniform(const std::string& name, float value)
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniform(const std::string& name, float* values, uint32_t count)
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform1fv(location, count, values);
	}

	void OpenGLShader::UploadUniform(const std::string& name, const glm::vec2& values)
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform2f(location, values.x, values.y);
	}

	void OpenGLShader::UploadUniform(const std::string& name, const glm::vec3& values)
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform3f(location, values.x, values.y, values.z);
	}

	void OpenGLShader::UploadUniform(const std::string& name, const glm::vec4& values)
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::UploadUniform(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniform(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}

#include "mlpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Milky/Core/Timer.h"

namespace Milky {

	namespace Utils {
		
		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			ML_CORE_ASSERT(false, "Unkown shader type: '" + type + "'");
			return GL_NONE;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			ML_CORE_ASSERT(false, "Unkown shader enum: " + stage);
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			ML_CORE_ASSERT(false, "Unkown shader enum: " + stage);
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			ML_CORE_ASSERT(false, "Unkown shader stage: " + stage);
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			}
			ML_CORE_ASSERT(false, "Unkown shader stage: " + stage);
			return "";
		}

		static const char* GLShaderCachedOpenGLAMDFileExtension()
		{
			return ".cached_opengl.amd";
		}

		static const bool IsAMDGpu()
		{
			const char* vendor = (char*)glGetString(GL_VENDOR);
			return strstr(vendor, "ATI") != nullptr;
		}

	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_FilePath(filepath)
	{
		ML_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(filepath);
		ML_CORE_ASSERT(source.size(), "Could not load shader file, or file is empty!");
		auto shaderSources = PreProcess(source);
		{
			Timer timer;
			CompileOrGetVulkanBinaries(shaderSources);
			if (Utils::IsAMDGpu())
				CreateProgramAMD();
			else
			{
				CompileOrGetOpenGLBinaries();
				CreateProgram();
			}
			ML_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}
		
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

		CompileOrGetVulkanBinaries(sources);
		if (Utils::IsAMDGpu())
			CreateProgramAMD();
		else
		{
			CompileOrGetOpenGLBinaries();
			CreateProgram();
		}
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
		ML_CORE_INFO("Loading shader source file: '{0}'", filepath);
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				ML_CORE_ERROR("Could not read from shader source file: '{0}'", filepath);
			}
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

		const char* typeToken = "#type";
		const size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			ML_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			ML_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			ML_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		ML_PROFILE_FUNCTION();

		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					ML_CORE_ERROR(module.GetErrorMessage());
					ML_CORE_ASSERT(false, "Shader compilation failure!");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		ML_PROFILE_FUNCTION();

		auto& shaderData = m_OpenGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		shaderData.clear();
		m_OpenGLSourceCode.clear();
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_OpenGLSourceCode[stage] = glslCompiler.compile();
				auto& source = m_OpenGLSourceCode[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					ML_CORE_ERROR(module.GetErrorMessage());
					ML_CORE_ASSERT(false, "Shader compilation failure!");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::CompileOpenGLBinariesAMD(GLenum& program, std::array<uint32_t, 2>& shaderIDs)
	{
		int glShaderIDIndex = 0;
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			spirv_cross::CompilerGLSL glslCompiler(spirv);
			auto& source = glslCompiler.compile();

			uint32_t shader;

			shader = glCreateShader(stage);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			int isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				int maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<char> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				ML_CORE_ERROR("{0}", infoLog.data());
				ML_CORE_ASSERT(false, "Shader compilation failure!");
				return;
			}
			glAttachShader(program, shader);
			shaderIDs[glShaderIDIndex++] = shader;
		}
	}

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), (GLsizei)(spirv.size() * sizeof(uint32_t)));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			ML_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_RendererID = program;
	}

	static bool VerifyProgramLink(GLenum& program)
	{
		int isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			int maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<char> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			ML_CORE_ERROR("{0}", infoLog.data());
			ML_CORE_ASSERT(false, "Shader link failure!");
			return false;
		}
		return true;
	}

	void OpenGLShader::CreateProgramAMD()
	{
		GLuint program = glCreateProgram();

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();
		std::filesystem::path shaderFilePath = m_FilePath;
		std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderCachedOpenGLAMDFileExtension());
		std::ifstream in(cachedPath, std::ios::ate | std::ios::binary);

		if (in.is_open())
		{
			auto size = in.tellg();
			in.seekg(0);

			auto& data = std::vector<char>(size);
			uint32_t format = 0;
			in.read((char*)&format, sizeof(uint32_t));
			in.read((char*)data.data(), size);
			glProgramBinary(program, format, data.data(), (GLsizei)data.size());

			bool linked = VerifyProgramLink(program);

			if (!linked)
				return;
		}
		else
		{
			std::array<uint32_t, 2> shaderIDs;
			CompileOpenGLBinariesAMD(program, shaderIDs);
			glLinkProgram(program);

			bool linked = VerifyProgramLink(program);

			if (linked)
			{
				// Save program data
				GLint formats = 0;
				glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
				ML_CORE_ASSERT(formats > 0, "Driver does not support binary format");
				Utils::CreateCacheDirectoryIfNeeded();
				GLint length = 0;
				glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &length);
				auto shaderData = std::vector<char>(length);
				uint32_t format = 0;
				glGetProgramBinary(program, length, nullptr, &format, shaderData.data());
				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					out.write((char*)&format, sizeof(uint32_t));
					out.write(shaderData.data(), shaderData.size());
					out.flush();
					out.close();
				}
			}

			for (auto& id : shaderIDs)
			{
				glDetachShader(program, id);
				glDeleteShader(id);
			}
		}

		m_RendererID = program;
	}

	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		ML_CORE_ASSERT(shaderData.size(), "Shader data is empty!");
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		ML_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
		ML_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		ML_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		ML_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = (uint32_t)compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = (int)bufferType.member_types.size();

			ML_CORE_TRACE("  {0}", resource.name);
			ML_CORE_TRACE("    Size = {0}", bufferSize);
			ML_CORE_TRACE("    Binding = {0}", binding);
			ML_CORE_TRACE("    Members = {0}", memberCount);
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

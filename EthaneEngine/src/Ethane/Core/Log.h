#pragma once

#include "Base.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include <glm/glm.hpp>

namespace Ethane {

#ifdef ETH_DEBUG

	struct log_mat4
	{
		log_mat4(const glm::mat4& _mat) { Mat = _mat; }
		glm::mat4 Mat;
		template<typename OStream>
		friend OStream& operator<<(OStream& os, const log_mat4& c)
		{
			os << std::endl << "[" << std::endl;
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
					os << c.Mat[i][j] << " ";
				os << std::endl;
			}
			os << "]";
			return os;
		}
	};

	struct log_mat3
	{
		log_mat3(const glm::mat3& _mat) { Mat = _mat; }
		glm::mat3 Mat;
		template<typename OStream>
		friend OStream& operator<<(OStream& os, const log_mat3& c)
		{
			os << std::endl << "[" << std::endl;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
					os << c.Mat[i][j] << " ";
				os << std::endl;
			}
			os << "]";
			return os;
		}
	};
#endif

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger()
		{
			return s_CoreLogger;
		}

		inline static std::shared_ptr<spdlog::logger>& GetClientLogger()
		{
			return s_ClientLogger;
		}

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
	
}


//core log macros
#define ETH_CORE_TRACE(...) ::Ethane::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ETH_CORE_INFO(...)  ::Ethane::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ETH_CORE_WARN(...)  ::Ethane::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ETH_CORE_ERROR(...) ::Ethane::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ETH_CORE_FATAL(...) ::Ethane::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//client log macros
#define ETH_TRACE(...) ::Ethane::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ETH_INFO(...)  ::Ethane::Log::GetClientLogger()->info(__VA_ARGS__)
#define ETH_WARN(...)  ::Ethane::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ETH_ERROR(...) ::Ethane::Log::GetClientLogger()->error(__VA_ARGS__)
#define ETH_FATAL(...) ::Ethane::Log::GetClientLogger()->fatal(__VA_ARGS__)

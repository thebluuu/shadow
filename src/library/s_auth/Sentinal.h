#pragma once
#include <string>
#pragma comment(lib, "Sentinal.lib")

#define SENTINAL_API __declspec(dllimport)

extern "C" SENTINAL_API void s_init(std::string version, std::string program_key, std::string api_key, bool anti_tamper);
extern "C" SENTINAL_API bool s_login(std::string username, std::string password);
extern "C" SENTINAL_API bool s_registr(std::string username, std::string password, std::string token);
extern "C" SENTINAL_API bool s_activate(std::string username, std::string token);
extern "C" SENTINAL_API bool s_token(std::string token);
extern "C" SENTINAL_API std::string s_filestream(std::string file_name);
extern "C" SENTINAL_API std::string s_var(std::string var_name);
extern "C" SENTINAL_API void s_log(std::string message);

extern "C" SENTINAL_API std::string s_get_response();
extern "C" SENTINAL_API std::string s_get_expiry();
extern "C" SENTINAL_API std::string s_get_username();
extern "C" SENTINAL_API int s_get_level();

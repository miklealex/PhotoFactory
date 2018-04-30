#pragma once
#include "UsersManager.h"

class EXPORT_CLASS LogInManager
{
	std::unique_ptr<UsersManager> m_usersManager;
	LogInManager();
	LogInManager(const LogInManager&) = delete;
	LogInManager(const LogInManager&&) = delete;
	auto operator=(const LogInManager&) = delete;
	auto operator=(const LogInManager&&) = delete;
	bool checkPassword(std::string& username, std::string& password);
	bool isUserExist(std::string& username) const;
public:
	static LogInManager& getLogInManager();
	bool createNewUser(User&& currentUser);
	bool updateUser(std::string& userName, User&& updatedUser);
	bool removeUser(std::string& username);
	std::unique_ptr<User> LogIn(std::string& username, std::string& password);
};
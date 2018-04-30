#include "LoginManager.h"

EXPORT_CLASS LogInManager& LogInManager::getLogInManager()
{
	static LogInManager m_manager;
	return m_manager;
}

EXPORT_CLASS LogInManager::LogInManager()
{
	m_usersManager = std::make_unique<UsersManager>(std::move(UsersManager::getUsersManager()));
}

EXPORT_CLASS bool LogInManager::checkPassword(std::string& username, std::string& password)
{
	auto curUser = m_usersManager->getUserByUsername(username);
	if (curUser)
	{
		return curUser->getUserPassword() == password;
	}
	return false;
}

EXPORT_CLASS bool LogInManager::isUserExist(std::string& username) const
{
	return m_usersManager->getUserByUsername(username) != nullptr;
}

EXPORT_CLASS bool LogInManager::createNewUser(User&& currentUser)
{
	return m_usersManager->createUser(currentUser);
}

EXPORT_CLASS bool LogInManager::updateUser(std::string& username, User&& currentUser)
{
	return m_usersManager->updateUser(username, currentUser);
}

EXPORT_CLASS bool LogInManager::removeUser(std::string& username)
{
	return m_usersManager->removeUserByUsername(username);
}

EXPORT_CLASS std::unique_ptr<User> LogInManager::LogIn(std::string& username, std::string& password)
{
	if (checkPassword(username, password))
	{
		return m_usersManager->getUserByUsername(username);
	}
	return nullptr;
}
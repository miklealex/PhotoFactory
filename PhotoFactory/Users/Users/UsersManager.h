#pragma once

#include "tinyxml.h"
#include <boost/filesystem.hpp>
#include <unordered_map>
#include <list>
#include <algorithm>
#include "User.h"
#include "XMLParserForUsers.h"
#define XMLFILE_NAME "Users.xml"

class EXPORT_CLASS UsersManager
{
public:
	static UsersManager& getUsersManager()
	{
		static  UsersManager m_usersManager(boost::filesystem::current_path().string() +"\\" + XMLFILE_NAME);
		return m_usersManager;
	}

	std::list<std::string> getAllUsersNames();
	std::list<std::string> getFilteredUsersNames(std::string& startingWith);
	std::unique_ptr<User> getUserByUsername(const std::string& userName);
	bool createUser(User& newUser);
	bool removeUserByUsername(const std::string& username);
	bool updateUser(std::string& username, User& updatedUser);
	UsersManager(const UsersManager&) = default;
	UsersManager& operator = (const UsersManager&) = default;
private:
	void updateUsersNamesList();
	UsersManager(const std::string& filePath);
	UsersManager() = delete;
	uint64_t m_usersAmount = 0;
	std::unordered_map<std::string, User> m_usersMap;
	std::string m_xmlFilePath;
	std::list<std::string> m_usersNamesList;
};
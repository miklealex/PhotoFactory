// Users.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "UsersManager.h"

EXPORT_CLASS UsersManager::UsersManager(const std::string& filePath)
{
	XMLParserForUsers& xmlParser = XMLParserForUsers::getInstance();
	if (boost::filesystem::exists(filePath.c_str()))
	{
		xmlParser.ParseFile(filePath, m_usersMap);
	}
	else
	{
		xmlParser.UpdateFile(filePath, m_usersMap);
	}
	m_xmlFilePath = filePath;
}

EXPORT_CLASS std::unique_ptr<User> UsersManager::getUserByUsername(const std::string& userName)
{
	auto it = m_usersMap.find(userName);
	if (it != m_usersMap.end())
	{
		return std::make_unique<User>(it->second);
	}
	else
	{
		return nullptr;
	}
}

EXPORT_CLASS std::list<std::string> UsersManager::getFilteredUsersNames(std::string& nameStartsWith)
{
	std::list<std::string> filteredList;
	for_each(m_usersNamesList.begin(), m_usersNamesList.end(), 
		[&filteredList, &nameStartsWith](std::string curName)
		{
			if (curName.length() >= nameStartsWith.length() && curName.substr(0, nameStartsWith.length()) == nameStartsWith)
				filteredList.push_back(curName);
	});
	return filteredList;
}

EXPORT_CLASS std::list<std::string> UsersManager::getAllUsersNames()
{
	if (m_usersNamesList.empty())
	{
		updateUsersNamesList();
	}
	return std::move(m_usersNamesList);
}

EXPORT_CLASS void UsersManager::updateUsersNamesList()
{
	for (auto it = m_usersMap.begin(); it != m_usersMap.end(); ++it)
	{
		m_usersNamesList.push_back(it->first);
	}
}

EXPORT_CLASS bool UsersManager::createUser(User& newUser)
{
	auto it = m_usersMap.find(newUser.getUserName());
	if (it == m_usersMap.end())
	{
		m_usersMap.insert(std::make_pair<std::string, User>(std::move(newUser.getUserName()), std::move(newUser)));
		++m_usersAmount;
		return XMLParserForUsers::getInstance().UpdateUserInFile(m_xmlFilePath, newUser);
	}
	else
	{
		return false;
	}
}

EXPORT_CLASS bool UsersManager::removeUserByUsername(const std::string& userName)
{
	auto it = m_usersMap.find(userName);
	if (it != m_usersMap.end())
	{
		m_usersMap.erase(userName);
		return XMLParserForUsers::getInstance().UpdateFile(m_xmlFilePath, m_usersMap);
	}
	else
	{
		return false;
	}
}

EXPORT_CLASS bool UsersManager::updateUser(std::string& userName, User& updatedUser)
{
	auto it = m_usersMap.find(userName);
	if (it != m_usersMap.end())
	{
		if (m_usersMap.find(updatedUser.getUserName()) != m_usersMap.end())
		{
			return false;
		}
		m_usersMap.erase(it->first);
		m_usersMap.insert(std::make_pair<std::string, User>(std::move(userName), std::move(updatedUser)));
		return XMLParserForUsers::getInstance().UpdateFile(m_xmlFilePath, m_usersMap);
	}
	else
	{
		return false;
	}
}
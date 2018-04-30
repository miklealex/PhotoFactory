#pragma once
#include <string>
#include "User.h"
#include <boost/filesystem.hpp>
#include "tinyxml.h"
#include <unordered_map>
#include <vector>

class EXPORT_CLASS XMLParserForUsers
{
public:
	static XMLParserForUsers& getInstance();
	XMLParserForUsers() = default;
	XMLParserForUsers(const XMLParserForUsers&) = delete;
	auto operator = (const XMLParserForUsers&) = delete;
	XMLParserForUsers(const XMLParserForUsers&&) = delete;
	auto operator = (const XMLParserForUsers&&) = delete;
	bool ParseFile(const std::string& filePath, std::unordered_map<std::string, User>& outUsersMap);
	bool UpdateFile(const std::string& filePath, std::unordered_map<std::string, User>& usersMap) const;
	bool UpdateUserInFile(const std::string& filePath, User& currentUser) const;
private:
	bool ParseUserInfo(TiXmlElement* currentElement,UserInfo& currentUserInfo);
	bool WriteUserToFile(TiXmlElement* currentElement, User& currentUser) const;
	bool UpdateUserInFile(TiXmlElement* currentElement, User& currentUser) const;
};
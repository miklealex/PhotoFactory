#include "User.h"

EXPORT_CLASS User::User(std::string& username, std::string& password, std::string& registerDate, UserInfo& info, std::vector<std::string>* galleryItemsPath)
{
	m_username = username;
	m_password = password;
	m_registerDate = registerDate;
	m_userInfo = info;
	m_uniqueId = boost::uuids::random_generator()();

	if (galleryItemsPath)
	{
		for (auto it = galleryItemsPath->begin(); it != galleryItemsPath->end(); ++it)
		{
			m_galleryItemsPath.push_back(*it);
		}
	}
	else
	{
		boost::filesystem::path userPath = "D:/Gallery/" + username + "/" + boost::gregorian::to_simple_string(boost::gregorian::day_clock::local_day());
		boost::filesystem::create_directories(userPath);
		m_galleryItemsPath.push_back(userPath.string());
	}
}

EXPORT_CLASS User::User(std::string&& username, std::string&& password, std::string&& registerDate, UserInfo&& info, std::vector<std::string>* galleryItemsPath)
{
	m_username = std::move(username);
	m_password = std::move(password);
	m_registerDate = std::move(registerDate);
	m_userInfo = std::move(info);
	m_uniqueId = boost::uuids::random_generator()();

	if (galleryItemsPath)
	{
		for (auto it = galleryItemsPath->begin(); it != galleryItemsPath->end(); ++it)
		{
			m_galleryItemsPath.push_back(*it);
		}
	}
	else
	{
		boost::filesystem::path userPath = "D:/Gallery/" + username + "/" + boost::gregorian::to_simple_string(boost::gregorian::day_clock::local_day());
		boost::filesystem::create_directories(userPath);
		m_galleryItemsPath.push_back(userPath.string());
	}
}

EXPORT_CLASS User::User(const char* username, const char* password, const char* registerDate, UserInfo&& info) : m_username(username), m_password(password), m_registerDate(registerDate)
{
	m_userInfo = std::move(info);
	m_uniqueId = boost::uuids::random_generator()();
}

EXPORT_CLASS auto User::operator = (const User& curUser)
{
	if (this != &curUser)
	{
		m_uniqueId = curUser.m_uniqueId;
		m_username = curUser.m_username;
		m_password = curUser.m_password;
		m_registerDate = curUser.m_registerDate;
		m_galleryItemsPath.clear();
		for (auto it = curUser.m_galleryItemsPath.begin(); it != curUser.m_galleryItemsPath.end(); ++it)
		{
			m_galleryItemsPath.push_back(*it);
		}
		m_userInfo = curUser.m_userInfo;
	}
	return *this;
}

EXPORT_CLASS UserInfo User::getUserInfo()
{
	return m_userInfo;
}

EXPORT_CLASS std::string User::getUserName()
{
	return m_username;
}

EXPORT_CLASS std::string User::getUserPassword()
{
	return m_password;
}

EXPORT_CLASS std::string User::getUserRegisterDate()
{
	return m_registerDate;
}
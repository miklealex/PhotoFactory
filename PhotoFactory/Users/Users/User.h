#pragma once
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <vector>

#define EXPORT_CLASS __declspec(dllexport)

enum UserGender 
{
	Male,
	Female
};

struct UserInfo
{
	std::string m_name;
	std::string m_surname;
	std::string m_dateOfBirth;
	std::string m_bornInTown;
	std::string m_bornCountry;
	std::string m_phoneNumber;
	std::string m_emai;
	UserGender m_sex;
};

class EXPORT_CLASS User
{
	boost::uuids::uuid m_uniqueId;
	std::string m_username;
	std::string m_password;
	std::string m_registerDate;
	UserInfo m_userInfo;
	std::vector<std::string> m_galleryItemsPath;
public:

	User() = delete;
	User(const User&) = default;
	User(std::string& username, std::string& password, std::string& registerDate, UserInfo& info, std::vector<std::string>* galleryItemsPath = nullptr);
	User(std::string&& userName, std::string&& password, std::string&&registerDate, UserInfo&& info, std::vector<std::string>* galleryItemsPath = nullptr);
	User(const char* userName, const char* password, const char* registerDate, UserInfo&& info);
	auto operator = (const User& curUser);
	std::string getUserName();
	std::string getUserRegisterDate();
	std::string getUserPassword();
	UserInfo getUserInfo();
};
#include "XMLParserForUsers.h"

EXPORT_CLASS XMLParserForUsers& XMLParserForUsers::getInstance()
{
	static XMLParserForUsers m_instance;
	return m_instance;
}

EXPORT_CLASS bool XMLParserForUsers::ParseFile(const std::string& filePath, std::unordered_map<std::string, User>& outUsersMap)
{
	if (!boost::filesystem::exists(filePath))
	{
		return false;
	}

	TiXmlDocument doc(filePath.c_str());
	if (!doc.LoadFile())
	{
		return false;
	}

	TiXmlElement* firstEl = doc.FirstChildElement("users");
	if (firstEl)
	{
		TiXmlElement* curUserEl = firstEl->FirstChildElement("user");
		while (curUserEl)
		{
			std::string userName, password, registerDate;
			const char* curAt = curUserEl->Attribute("username");
			if (!curAt) return false;
			userName = curAt;
			TiXmlElement* curEl = curUserEl->FirstChildElement("password");
			if (!curEl) return false;
			password = curEl->GetText();
			curEl = curUserEl->FirstChildElement("registerDate");
			if (!curEl) return false;
			registerDate = curEl->GetText();
			curEl = curUserEl->FirstChildElement("UserInfo");
			if (!curEl) return false;
			UserInfo curUserInfo;
			if (!ParseUserInfo(curEl, curUserInfo)) return false;
			User curUser(userName, password, registerDate, curUserInfo);
			outUsersMap.insert(std::make_pair<std::string, User>(std::move(userName), std::move(curUser)));
			curUserEl = curUserEl->NextSiblingElement();
		}
		return true;
	}
	return false;
}

EXPORT_CLASS bool XMLParserForUsers::ParseUserInfo(TiXmlElement* userInfo, UserInfo& currentUserInfo)
{
	TiXmlElement* curEl = userInfo->FirstChildElement("name");
	if (!curEl) return false;
	currentUserInfo.m_name = curEl->GetText();
	curEl = userInfo->FirstChildElement("surname");
	if (!curEl) return false;
	currentUserInfo.m_surname = curEl->GetText();
	curEl = userInfo->FirstChildElement("dateOfBirth");
	if (!curEl) return false;
	currentUserInfo.m_dateOfBirth = curEl->GetText();
	curEl = userInfo->FirstChildElement("bornTown");
	if (!curEl) return false;
	currentUserInfo.m_bornInTown = curEl->GetText();
	curEl = userInfo->FirstChildElement("bornCountry");
	if (!curEl) return false;
	currentUserInfo.m_bornCountry = curEl->GetText();
	curEl = userInfo->FirstChildElement("phoneNumber");
	if (!curEl) return false;
	currentUserInfo.m_phoneNumber = curEl->GetText();
	curEl = userInfo->FirstChildElement("email");
	if (!curEl) return false;
	currentUserInfo.m_emai = curEl->GetText();
	curEl = userInfo->FirstChildElement("sex");
	if (!curEl) return false;
	const char* sex = curEl->GetText();
	if (!sex) return false;
	if (strlen(sex) != 1) return false;
	currentUserInfo.m_sex = UserGender(sex[0] - '0');
	return true;
}

EXPORT_CLASS bool XMLParserForUsers::UpdateUserInFile(TiXmlElement* firstEl, User& currentUser) const
{
	if (!firstEl)
	{
		return false;
	}
	const char* userName = firstEl->Attribute("username");
	const char* currentUserName = currentUser.getUserName().c_str();
	if (!userName || strcmp(userName, currentUserName))
	{
		firstEl->SetAttribute("username", currentUser.getUserName().c_str());
	}
	TiXmlElement* password = firstEl->FirstChildElement("password");
	if (!password)
	{
		TiXmlElement* password = new TiXmlElement("password");
		password->LinkEndChild(new TiXmlText(currentUser.getUserPassword().c_str()));
		firstEl->LinkEndChild(password);
	}
	else
	{
		password->Clear();
		password->LinkEndChild(new TiXmlText(currentUser.getUserPassword().c_str()));
	}
	TiXmlElement* registerDate = firstEl->FirstChildElement("registerDate");
	if (!registerDate)
	{
		registerDate = new TiXmlElement("registerDate");
		registerDate->LinkEndChild(new TiXmlText(currentUser.getUserRegisterDate().c_str()));
		firstEl->LinkEndChild(registerDate);
	}
	else
	{
		registerDate->Clear();
		registerDate->LinkEndChild(new TiXmlText(currentUser.getUserRegisterDate().c_str()));
	}
	UserInfo curUserInfo = currentUser.getUserInfo();
	TiXmlElement* userInfo = firstEl->FirstChildElement("userInfo");
	if (!userInfo)
	{
		userInfo = new TiXmlElement("UserInfo");
		firstEl->LinkEndChild(userInfo);
	}
	TiXmlElement* name = userInfo->FirstChildElement("name");
	if (!name)
	{
		name = new TiXmlElement("name");
		name->LinkEndChild(new TiXmlText(curUserInfo.m_name.c_str()));
		userInfo->LinkEndChild(name);
	}
	else
	{
		name->Clear();
		name->LinkEndChild(new TiXmlText(curUserInfo.m_name.c_str()));
	}
	TiXmlElement* surname = userInfo->FirstChildElement("surname");
	if (!surname)
	{
		surname = new TiXmlElement("surname");
		surname->LinkEndChild(new TiXmlText(curUserInfo.m_surname.c_str()));
		userInfo->LinkEndChild(surname);
	}
	else
	{
		surname->Clear();
		surname->LinkEndChild(new TiXmlText(curUserInfo.m_surname.c_str()));
	}
	TiXmlElement* dateOfBirth = userInfo->FirstChildElement("dateOfBirth");
	if (!dateOfBirth)
	{
		dateOfBirth = new TiXmlElement("dateOfBirth");
		dateOfBirth->LinkEndChild(new TiXmlText(curUserInfo.m_dateOfBirth.c_str()));
		userInfo->LinkEndChild(dateOfBirth);
	}
	else
	{
		dateOfBirth->Clear();
		dateOfBirth->LinkEndChild(new TiXmlText(curUserInfo.m_dateOfBirth.c_str()));
	}
	TiXmlElement* bornTown = userInfo->FirstChildElement("bornTown");
	if (!bornTown)
	{
		bornTown = new TiXmlElement("bornTown");
		bornTown->LinkEndChild(new TiXmlText(curUserInfo.m_bornInTown.c_str()));
		userInfo->LinkEndChild(bornTown);
	}
	else
	{
		bornTown->Clear();
		bornTown->LinkEndChild(new TiXmlText(curUserInfo.m_bornInTown.c_str()));
	}
	TiXmlElement* bornCountry = userInfo->FirstChildElement("bornCountry");
	if (!bornCountry)
	{
		bornCountry = new TiXmlElement("bornCountry");
		bornCountry->LinkEndChild(new TiXmlText(curUserInfo.m_bornCountry.c_str()));
		userInfo->LinkEndChild(bornCountry);
	}
	else
	{
		bornCountry->Clear();
		bornCountry->LinkEndChild(new TiXmlText(curUserInfo.m_bornCountry.c_str()));
	}
	TiXmlElement* phoneNumber = userInfo->FirstChildElement("phoneNumber");
	if (!phoneNumber)
	{
		phoneNumber = new TiXmlElement("phoneNumber");
		phoneNumber->LinkEndChild(new TiXmlText(curUserInfo.m_phoneNumber.c_str()));
		userInfo->LinkEndChild(phoneNumber);
	}
	else
	{
		phoneNumber->Clear();
		phoneNumber->LinkEndChild(new TiXmlText(curUserInfo.m_phoneNumber.c_str()));
	}
	TiXmlElement* email = userInfo->FirstChildElement("email");
	if (!email)
	{
		email = new TiXmlElement("email");
		email->LinkEndChild(new TiXmlText(curUserInfo.m_emai.c_str()));
		userInfo->LinkEndChild(email);
	}
	else
	{
		email->Clear();
		email->LinkEndChild(new TiXmlText(curUserInfo.m_emai.c_str()));
	}
	TiXmlElement* sex = userInfo->FirstChildElement("sex");
	if (!sex)
	{
		sex = new TiXmlElement("sex");
		std::string s = std::to_string(curUserInfo.m_sex);
		sex->LinkEndChild(new TiXmlText(s.c_str()));
		userInfo->LinkEndChild(sex);
	}
	else
	{
		sex->Clear();
		std::string s = std::to_string(curUserInfo.m_sex);
		sex->LinkEndChild(new TiXmlText(s.c_str()));
	}
	return true;
}

EXPORT_CLASS bool XMLParserForUsers::WriteUserToFile(TiXmlElement* firstEl, User& currentUser) const
{
	TiXmlElement* curUserEl = new TiXmlElement("user");
	curUserEl->SetAttribute("username", currentUser.getUserName().c_str());
	firstEl->LinkEndChild(curUserEl);
	TiXmlElement* password = new TiXmlElement("password");
	password->LinkEndChild(new TiXmlText(currentUser.getUserPassword().c_str()));
	curUserEl->LinkEndChild(password);
	TiXmlElement* registerDate = new TiXmlElement("registerDate");
	registerDate->LinkEndChild(new TiXmlText(currentUser.getUserRegisterDate().c_str()));
	curUserEl->LinkEndChild(registerDate);
	UserInfo curUserInfo = currentUser.getUserInfo();
	TiXmlElement* userInfo = new TiXmlElement("UserInfo");
	curUserEl->LinkEndChild(userInfo);
	TiXmlElement* name = new TiXmlElement("name");
	name->LinkEndChild(new TiXmlText(curUserInfo.m_name.c_str()));
	userInfo->LinkEndChild(name);
	TiXmlElement* surname = new TiXmlElement("surname");
	surname->LinkEndChild(new TiXmlText(curUserInfo.m_surname.c_str()));
	userInfo->LinkEndChild(surname);
	TiXmlElement* dateOfBirth = new TiXmlElement("dateOfBirth");
	dateOfBirth->LinkEndChild(new TiXmlText(curUserInfo.m_dateOfBirth.c_str()));
	userInfo->LinkEndChild(dateOfBirth);
	TiXmlElement* bornTown = new TiXmlElement("bornTown");
	bornTown->LinkEndChild(new TiXmlText(curUserInfo.m_bornInTown.c_str()));
	userInfo->LinkEndChild(bornTown);
	TiXmlElement* bornCountry = new TiXmlElement("bornCountry");
	bornCountry->LinkEndChild(new TiXmlText(curUserInfo.m_bornCountry.c_str()));
	userInfo->LinkEndChild(bornCountry);
	TiXmlElement* phoneNumber = new TiXmlElement("phoneNumber");
	phoneNumber->LinkEndChild(new TiXmlText(curUserInfo.m_phoneNumber.c_str()));
	userInfo->LinkEndChild(phoneNumber);
	TiXmlElement* email = new TiXmlElement("email");
	email->LinkEndChild(new TiXmlText(curUserInfo.m_emai.c_str()));
	userInfo->LinkEndChild(email);
	TiXmlElement* sex = new TiXmlElement("sex");
	std::string s = std::to_string(curUserInfo.m_sex);
	sex->LinkEndChild(new TiXmlText(s.c_str()));
	userInfo->LinkEndChild(sex);
	return true;
}

EXPORT_CLASS bool XMLParserForUsers::UpdateFile(const std::string& filePath, std::unordered_map<std::string, User>& usersMap) const
{
	TiXmlDocument doc(filePath.c_str());
	if (!doc.LoadFile())
	{
		TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
		doc.LinkEndChild(declaration);

		TiXmlElement* firstEl = new TiXmlElement("users");
		doc.LinkEndChild(firstEl);

		for (auto it = usersMap.begin(); it != usersMap.end(); ++it)
		{
			if (!WriteUserToFile(firstEl, it->second)) return false;
		}
		
	}
	else
	{
		std::vector<std::string> usernamesVector;
		for (auto it = usersMap.begin(); it != usersMap.end(); ++it)
		{
			usernamesVector.push_back(it->second.getUserName());
		}
		TiXmlElement* root = doc.FirstChildElement("users");
		if (!root)
		{
			root = new TiXmlElement("users");
			doc.LinkEndChild(root);
		}
		TiXmlElement* firstUser = root->FirstChildElement("user");
		int absentUserName = 0;
		if (!firstUser)
		{
			firstUser = new TiXmlElement("user");
			firstUser->SetAttribute("username", usernamesVector[absentUserName].c_str());
			root->LinkEndChild(firstUser);
			++absentUserName;
		}
		for (; firstUser != nullptr;  firstUser = firstUser->NextSiblingElement())
		{
			if (!firstUser)
			{
				firstUser = new TiXmlElement("user");
				firstUser->SetAttribute("username", usernamesVector[absentUserName].c_str());
				root->LinkEndChild(firstUser);
				++absentUserName;
			}
			auto it = usersMap.find(std::string(firstUser->Attribute("username")));
			if (it == usersMap.end())
			{
				firstUser->Clear();
				continue;
			}
			if (!UpdateUserInFile(firstUser, it->second))
			{
				return false;
			}
		}
	}
	doc.SaveFile(filePath.c_str());
	return true;

}

EXPORT_CLASS bool XMLParserForUsers::UpdateUserInFile(const std::string& filePath, User& currentUser) const
{
	TiXmlDocument doc(filePath.c_str());
	if (!doc.LoadFile())
	{
		TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
		doc.LinkEndChild(declaration);

		TiXmlElement* firstEl = new TiXmlElement("users");
		doc.LinkEndChild(firstEl);

		return WriteUserToFile(firstEl, currentUser);
	}
	else
	{
		TiXmlElement* root = doc.FirstChildElement("users");
		if (!root) return false;
		TiXmlElement* curEl = root->FirstChildElement("user");
		while (curEl && strcmp(curEl->Attribute("username"), currentUser.getUserName().c_str()))
		{
			curEl = curEl->NextSiblingElement();
		}
		if (!curEl)
		{
			if (WriteUserToFile(root, currentUser))
			{
				doc.SaveFile(filePath.c_str());
				return true;
			}
			else return false;
		}
		if (UpdateUserInFile(curEl, currentUser))
		{
			doc.SaveFile(filePath.c_str());
			return true;
		}
		else return false;
	}
}
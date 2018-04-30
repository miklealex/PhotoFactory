#include <iostream>
#include "LogInManager.h"

int main()
{
	LogInManager& manager = LogInManager::getLogInManager();
	std::string userName = "miklealex";
	std::string name = "Misha";
	std::string surname = "Oleksyn";
	std::string phoneNumber = "+380936412622";
	std::string emqil = "miklealex73@g,ail.com";
	std::string password = "123Misha098";
	std::string registerDate = boost::gregorian::to_simple_string(boost::gregorian::day_clock::local_day());
	std::string dateOfBirth = "24.11.1996";
	std::string bornTown = "Lviv";
	std::string bornCountry = "Ukraine";
	UserInfo currentInfo = { name, surname, dateOfBirth, bornTown, bornCountry, phoneNumber, emqil, UserGender::Male };
	User me(userName, password, registerDate, currentInfo);
	manager.createNewUser(std::move(me));
	auto user = manager.LogIn(userName, password);
}
#include "UsersManager.h"

int main()
{
	/*boost::gregorian::date date = boost::gregorian::day_clock::local_day();
	std::string one = boost::gregorian::to_iso_extended_string(date);
	std::string two = boost::gregorian::to_iso_string(date);
	std::string three ="";
	std::string four = boost::gregorian::to_sql_string(date);
	std::cout << one << std::endl << two << std::endl << three << std::endl << four;
	boost::gregorian::date date1 = boost::gregorian::from_string(three);
	std::string five = boost::gregorian::to_simple_string(date1);
	if (date > date1)
	{

	}
	std::cout << std::endl << five;*/
	UsersManager& manager = UsersManager::getUsersManager();

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
	manager.createUser(me);

	std::unique_ptr<User> cyrUs = manager.getUserByUsername("miklealex");
	User updatedUser("loh", "newPassword", cyrUs->getUserRegisterDate(), cyrUs->getUserInfo());
	std::string oldUsName = cyrUs->getUserName();
	manager.updateUser(oldUsName, updatedUser);
		return 0;
}
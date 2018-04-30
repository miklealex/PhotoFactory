#include "ConnectionManager.h"
#include "Common/logger.h"

#define ITEMS_TRANSFER_PORT 9567

ConnectionManager::ConnectionManager(boost::asio::ip::tcp::socket& serverSocket, boost::asio::ip::tcp::socket& clientSocket)
	:m_serverSocket{serverSocket}
	,m_clientSocket{clientSocket}
{
}

bool ConnectionManager::handShake()
{
	boost::system::error_code ec;
	std::string message{ SERVER_HANDSHAKE };
	boost::asio::write(m_serverSocket, boost::asio::buffer(message), ec);

	std::array<char, SERVER_HANDSHAKE_LENGTH> response;
	int len = m_serverSocket.read_some(boost::asio::buffer(response), ec);

	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);

	if (len != 2)
	{
		return false;
	}

	std::string responseMessage{ response.begin(), response.end() };
	if (responseMessage != std::string{ CLIENT_HANDSHAKE })
	{
		return false;
	}

	return true;
}

bool ConnectionManager::responseUserSearch()
{
	boost::system::error_code ec;
	std::array<char, MAX_USERNAME_LENGTH> username;
	int len = m_serverSocket.read_some(boost::asio::buffer(username), ec);

	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);

	std::string searchUsername{ username.begin(), username.end() };

	std::list<std::string> usernamesList = UsersManager::getUsersManager().getFilteredUsersNames(searchUsername);

	for (auto it = usernamesList.begin(); it != usernamesList.end(); ++it)
	{
		std::string len = std::to_string(it->length());
		boost::asio::write(m_serverSocket, boost::asio::buffer(len), ec);

		if (ec == boost::asio::error::eof)
			return true; // Connection closed cleanly by peer
		else if (ec)
			throw boost::system::system_error(ec);

		boost::asio::write(m_serverSocket, boost::asio::buffer(*it), ec);

		if (ec == boost::asio::error::eof)
			return true; // Connection closed cleanly by peer
		else if (ec)
			throw boost::system::system_error(ec);
	}

	return true;
}

bool ConnectionManager::responseFileSend()
{
	std::vector<std::string> m_taggedUserNames;
	boost::system::error_code ec;
	std::array<char, MAX_USERNAME_LENGTH> usernameBuffer;
	int len = m_serverSocket.read_some(boost::asio::buffer(usernameBuffer), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	std::string username{ usernameBuffer.begin(), usernameBuffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	
	std::unique_ptr<GalleryItem> curItem = getNewGalleryItemData();
	if (!curItem)
	{
		return false;
	}
	Logger::instance().setOptions("server_%3N.log", 1 * 1024 * 1024, 10 * 1024 * 1024);

	std::string dirName = _T("D:/Gallery/");
	if (!curItem->getTime().empty())
	{
		dirName.append(curItem->getTime());
	}
	else if (!curItem->getTimeFrom().empty() && !curItem->getTimeTo().empty())
	{
		dirName.append(curItem->getTimeFrom());
	}
	try {
		boost::asio::io_service ioService;

		Server server(ioService, ITEMS_TRANSFER_PORT, dirName);

		ioService.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	GalleryManager::getGalleryManager().addNewGalleryItem(*curItem.get(), username);
	return true;
}

bool ConnectionManager::responseLogIn()
{
	boost::system::error_code ec;
	std::array<char, MAX_USERNAME_LENGTH> username;
	int len = m_serverSocket.read_some(boost::asio::buffer(username), ec);

	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);

	std::string searchUsername{ username.begin(), username.end() };

	std::unique_ptr<User> user = UsersManager::getUsersManager().getUserByUsername(searchUsername);
	if (!user)
	{
		std::string noSuchUserMessage{ NO_SUCH_USERNAME };
		boost::asio::write(m_serverSocket, boost::asio::buffer(noSuchUserMessage), ec);
		if (ec == boost::asio::error::eof)
			return true; // Connection closed cleanly by peer
		else if (ec)
			throw boost::system::system_error(ec);
		return false;
	}
	else
	{
		std::string usernameFoundMessage{ USERNAME_FOUND };
		boost::asio::write(m_serverSocket, boost::asio::buffer(usernameFoundMessage), ec);
		if (ec == boost::asio::error::eof)
			return true; // Connection closed cleanly by peer
		else if (ec)
			throw boost::system::system_error(ec);
		std::array<char, MAX_PASSWORD_LENGTH> password;
		int len = m_serverSocket.read_some(boost::asio::buffer(password), ec);

		if (ec == boost::asio::error::eof)
			return true; // Connection closed cleanly by peer
		else if (ec)
			throw boost::system::system_error(ec);

		std::string sentPassword{ password.begin(), password.end() };
		if (sentPassword != user->getUserPassword())
		{
			std::string incorrectPasswordMessage{ INCORRECT_PASSWORD };
			boost::asio::write(m_serverSocket, boost::asio::buffer(incorrectPasswordMessage), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			return false;
		}
		else
		{
			std::string correctPasswordMessage{ CORRECT_PASSWORD };
			boost::asio::write(m_serverSocket, boost::asio::buffer(correctPasswordMessage), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);

			UserInfo currentUserInfo = user->getUserInfo();
			std::array<char, GOT_IT_LENGTH> checker;
			boost::asio::write(m_serverSocket, boost::asio::buffer(currentUserInfo.m_bornCountry), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			m_serverSocket.read_some(boost::asio::buffer(checker), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
			{
				return false;
			}
			boost::asio::write(m_serverSocket, boost::asio::buffer(currentUserInfo.m_bornInTown), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			m_serverSocket.read_some(boost::asio::buffer(checker), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
			{
				return false;
			}
			boost::asio::write(m_serverSocket, boost::asio::buffer(currentUserInfo.m_dateOfBirth), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			m_serverSocket.read_some(boost::asio::buffer(checker), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
			{
				return false;
			}
			boost::asio::write(m_serverSocket, boost::asio::buffer(currentUserInfo.m_emai), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			m_serverSocket.read_some(boost::asio::buffer(checker), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
			{
				return false;
			}
			boost::asio::write(m_serverSocket, boost::asio::buffer(currentUserInfo.m_name), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			m_serverSocket.read_some(boost::asio::buffer(checker), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
			{
				return false;
			}
			boost::asio::write(m_serverSocket, boost::asio::buffer(currentUserInfo.m_phoneNumber), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			m_serverSocket.read_some(boost::asio::buffer(checker), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
			{
				return false;
			}
			boost::asio::write(m_serverSocket, boost::asio::buffer(std::to_string(currentUserInfo.m_sex)), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			m_serverSocket.read_some(boost::asio::buffer(checker), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
			{
				return false;
			}
			boost::asio::write(m_serverSocket, boost::asio::buffer(currentUserInfo.m_surname), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			m_serverSocket.read_some(boost::asio::buffer(checker), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
			{
				return false;
			}
			boost::asio::write(m_serverSocket, boost::asio::buffer(user->getUserRegisterDate()), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			m_serverSocket.read_some(boost::asio::buffer(checker), ec);
			if (ec == boost::asio::error::eof)
				return true; // Connection closed cleanly by peer
			else if (ec)
				throw boost::system::system_error(ec);
			if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
			{
				return false;
			}
		}
		return true;
	}
}

std::unique_ptr<User> ConnectionManager::getNewUserData()
{
	boost::system::error_code ec;
	std::array<char, MAX_USERNAME_LENGTH> username;
	int len = m_serverSocket.read_some(boost::asio::buffer(username), ec);
	std::array<char, MAX_PASSWORD_LENGTH> passwordMes;
	len = m_serverSocket.read_some(boost::asio::buffer(passwordMes), ec);
	std::string usernameStr{ username.begin(), username.end() };
	std::string password{ passwordMes.begin(), passwordMes.end() };

	std::array<char, MAX_USER_FIELD_LENGTH> buffer;
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_name{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_surname{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_dateOfBirth{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_bornInTown{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_bornCountry{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_phoneNumber{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_email{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_sex{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	UserInfo newInfo{ m_name, m_surname, m_dateOfBirth, m_bornInTown, m_bornCountry, m_phoneNumber, m_email, (UserGender)(m_sex[0] - '0') };
	boost::gregorian::date date = boost::gregorian::day_clock::local_day();
	std::string registerDate = boost::gregorian::to_simple_string(date);
	User newUser{ usernameStr, password, registerDate, newInfo };
	return std::make_unique<User>(newUser);
}

std::unique_ptr<GalleryItem> ConnectionManager::getNewGalleryItemData()
{
	std::array<char, MAX_USER_FIELD_LENGTH> buffer;
	std::vector<std::string> m_taggedUserNames;
	boost::system::error_code ec;
	int len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	GalleryItemType m_type{ (GalleryItemType)std::string{ buffer.begin(), buffer.end() }[0] };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_time{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_timeFrom{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);;
	std::string m_timeTo{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_location{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_localPath{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string m_itemName{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	std::array<char, MAX_GALLERYITEM_DESCRIPTION_LENGTH> buffer1;
	len = m_serverSocket.read_some(boost::asio::buffer(buffer1), ec);
	std::string m_description{ buffer1.begin(), buffer1.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
	std::string taggedAmount{ buffer.begin(), buffer.end() };
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	int taggedAm = std::stoi(taggedAmount);
	for (int i = 0; i < taggedAm; ++i)
	{
		len = m_serverSocket.read_some(boost::asio::buffer(buffer), ec);
		m_taggedUserNames.push_back(std::string{ buffer.begin(), buffer.end() });
		boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	}
	return std::make_unique<GalleryItem>(m_type, m_time, m_timeFrom, m_timeTo, m_location, m_itemName, m_description, m_taggedUserNames);
}

bool ConnectionManager::responseSignUp()
{
	std::unique_ptr<User> newUser = getNewUserData();
	if (!newUser)
	{
		return false;
	}
	UsersManager::getUsersManager().createUser(*newUser.get());
	return true;
}

bool ConnectionManager::responseUsersRefresh()
{
	std::unique_ptr<User> updatedUser = getNewUserData();
	if (!updatedUser)
	{
		return false;
	}
	std::string username = updatedUser->getUserName();
	UsersManager::getUsersManager().updateUser(username, *updatedUser.get());
	return true;
}

bool ConnectionManager::responseFileRefresh()
{
	boost::system::error_code ec;
	std::array<char, MAX_USERNAME_LENGTH> username;
	int len = m_serverSocket.read_some(boost::asio::buffer(username), ec);
	std::string usernameS{ username.begin(), username.end() };
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);

	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);

	std::unique_ptr<GalleryItem> newItem = getNewGalleryItemData();
	if (!newItem)
	{
		return false;
	}
	GalleryManager::getGalleryManager().updateGalleryItem(*newItem.get(), usernameS);
	return true;
}

bool ConnectionManager::responseFileReceive(std::string address, std::string filePath)
{
	boost::system::error_code ec;
	std::array<char, MAX_USERNAME_LENGTH> username;
	int len = m_serverSocket.read_some(boost::asio::buffer(username), ec);
	std::string usernameS{ username.begin(), username.end() };
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);

	boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ GOT_IT }), ec);
	std::unordered_map<std::string, GalleryItem> itemsMap = GalleryManager::getGalleryManager().loadGalleryForUser(usernameS);
	auto foundItem = itemsMap.find(filePath);
	if (foundItem == itemsMap.end())
	{
		boost::asio::write(m_serverSocket, boost::asio::buffer(std::string{ NO_SUCH_ITEM }), ec);
		return false;
	}
	std::array<char, GOT_IT_LENGTH> checker;
	boost::asio::write(m_serverSocket, boost::asio::buffer(std::to_string((int)foundItem->second.getItemType())), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	m_serverSocket.read_some(boost::asio::buffer(checker), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
	{
		return false;
	}

	boost::asio::write(m_serverSocket, boost::asio::buffer(foundItem->second.getTime()), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	m_serverSocket.read_some(boost::asio::buffer(checker), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
	{
		return false;
	}

	boost::asio::write(m_serverSocket, boost::asio::buffer(foundItem->second.getTimeFrom()), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	m_serverSocket.read_some(boost::asio::buffer(checker), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
	{
		return false;
	}

	boost::asio::write(m_serverSocket, boost::asio::buffer(foundItem->second.getTimeTo()), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	m_serverSocket.read_some(boost::asio::buffer(checker), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
	{
		return false;
	}
	
	boost::asio::write(m_serverSocket, boost::asio::buffer(foundItem->second.getLocation()), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	m_serverSocket.read_some(boost::asio::buffer(checker), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
	{
		return false;
	}
	
	boost::asio::write(m_serverSocket, boost::asio::buffer(foundItem->second.getLocalPath()), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	m_serverSocket.read_some(boost::asio::buffer(checker), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
	{
		return false;
	}
	
	boost::asio::write(m_serverSocket, boost::asio::buffer(foundItem->second.getItemName()), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	m_serverSocket.read_some(boost::asio::buffer(checker), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
	{
		return false;
	}

	boost::asio::write(m_serverSocket, boost::asio::buffer(foundItem->second.getDescription()), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	m_serverSocket.read_some(boost::asio::buffer(checker), ec);
	if (ec == boost::asio::error::eof)
		return true; // Connection closed cleanly by peer
	else if (ec)
		throw boost::system::system_error(ec);
	if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
	{
		return false;
	}
	
	std::vector<std::string> m_taggedUserNames = foundItem->second.getTaggedUsers();

	for (int i = 0 ;i < m_taggedUserNames.size(); ++i)
	{
		boost::asio::write(m_serverSocket, boost::asio::buffer(m_taggedUserNames[i]), ec);
		if (ec == boost::asio::error::eof)
			return true; // Connection closed cleanly by peer
		else if (ec)
			throw boost::system::system_error(ec);
		m_serverSocket.read_some(boost::asio::buffer(checker), ec);
		if (ec == boost::asio::error::eof)
			return true; // Connection closed cleanly by peer
		else if (ec)
			throw boost::system::system_error(ec);
		if (std::string{ checker.begin(), checker.end() } != std::string{ GOT_IT })
		{
			return false;
		}
	}

	try
	{
		boost::asio::io_service ioService;

		boost::asio::ip::tcp::resolver resolver(ioService);
		auto endpointIterator = resolver.resolve({ address.c_str(), std::to_string(ITEMS_TRANSFER_PORT).c_str() });
		Client client(ioService, endpointIterator, filePath);
		ioService.run();
	}
	catch (std::fstream::failure& e) {
		std::cerr << e.what() << "\n";
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return true;
}


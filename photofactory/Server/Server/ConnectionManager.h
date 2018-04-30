#pragma once
#include "LoginManager.h"
#include "GalleryManager.h"
#include "MessagesDefines.h"
#include <boost/asio.hpp>
#include "Server/RequestResponseFileTransfer.h"
#include "Client/RequestResponseFileTransferClient.h"

class ConnectionManager
{
public:
	ConnectionManager() = delete;
	ConnectionManager(const ConnectionManager&) = delete;
	ConnectionManager(ConnectionManager&&) = delete;
	ConnectionManager(boost::asio::ip::tcp::socket& serverSocket, boost::asio::ip::tcp::socket& clientSocket);
	auto operator = (const ConnectionManager&) = delete;
	auto operator = (ConnectionManager&&) = delete;

	bool handShake();
	bool responseUserSearch();
	bool responseFileSend();
	bool responseLogIn();
	bool responseSignUp();
	bool responseFileReceive(std::string address, std::string filePath);
	bool responseFilesRefresh();
	bool responseUsersRefresh();
	bool responseFileRefresh();

private:
	boost::asio::ip::tcp::socket& m_serverSocket;
	boost::asio::ip::tcp::socket& m_clientSocket;

private:
	std::unique_ptr<User> getNewUserData();
	std::unique_ptr<GalleryItem> getNewGalleryItemData();
};
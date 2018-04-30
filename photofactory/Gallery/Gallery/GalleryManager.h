#pragma once
#include "GalleryItem.h"
#include "XMLParserForGallery.h"
#include <unordered_map>
#include <boost/date_time/gregorian/gregorian.hpp>

class DLL_EXPORT GalleryManager
{
	GalleryManager();
public:
	GalleryManager(const GalleryManager&) = delete;
	GalleryManager(const GalleryManager&&) = delete;
	auto operator=(const GalleryManager&) = delete;
	static GalleryManager& getGalleryManager();

	std::unordered_map<std::string, GalleryItem> loadGalleryForUser(const std::string& username);
	std::unordered_map<std::string, GalleryItem> loadGalleryForUserForTime(const std::string& username, const std::string& time);
	std::unordered_map<std::string, GalleryItem> loadGalleryForUserFromToTime(const std::string& username, const std::string& fromTime,const  std::string& toTime);
	std::unordered_map<std::string, GalleryItem> loadGalleryForUserForExactLocation(const std::string& username, const std::string& location);
	//TODO: std::unordered_map<std::string, GalleryItem> loadGalleryForUserForSomeArea(const std::string& username, const std::string& location, uint64_t radius);
	bool addNewGalleryItem(GalleryItem& currentItem,const  std::string& username);
	bool removeGalleryItem(GalleryItem& name, const std::string& username);
	bool updateGalleryItem(GalleryItem& name, const std::string& username);

private:
	std::string m_xmlFilePath;
};
#pragma once
#include <boost/filesystem.hpp>
#include "tinyxml.h"
#include <unordered_map>
#include <vector>
#include "GalleryItem.h"


class DLL_EXPORT XMLParserForGallery
{
public:
	static XMLParserForGallery& getInstance();
	XMLParserForGallery() = default;
	XMLParserForGallery(const XMLParserForGallery&) = delete;
	auto operator = (const XMLParserForGallery&) = delete;
	XMLParserForGallery(const XMLParserForGallery&&) = delete;
	auto operator = (const XMLParserForGallery&&) = delete;
	bool ParseFile(const std::string& filePath, const std::string& username, std::unordered_map<std::string, GalleryItem>& outUsersMap);
	bool UpdateFile(const std::string& filePath, const std::string& username, std::unordered_map<std::string, GalleryItem>& usersMap) const;
	bool UpdateItem(const std::string& filePath, const std::string& username, GalleryItem& curItem) const;
	bool RemoveItem(const std::string& filePath, const std::string& username, GalleryItem& curItem) const;
private:
	bool WriteItemToFile(TiXmlElement* currentElement, GalleryItem& currentUser) const;
	bool UpdateItemInFile(TiXmlElement* currentElement, GalleryItem& currentUser) const;
};
#pragma once
#include <string>
#include <vector>
#define DLL_EXPORT __declspec(dllexport)

enum DLL_EXPORT GalleryItemType
{
	PHOTO,
	VIDEO
};

class DLL_EXPORT GalleryItem
{
	GalleryItemType m_type;
	std::string m_time;
	std::string m_timeFrom;
	std::string m_timeTo;
	std::string m_location;
	std::string m_localPath;
	std::string m_itemName;
	std::string m_description;
	std::vector<std::string> m_taggedUserNames;
public:
	GalleryItem() = delete;
	GalleryItem(GalleryItemType type, std::string& time, std::string& timeFrom, std::string& timeTo, std::string& location, std::string& itemName, std::string& description, std::vector<std::string> &taggedUsers);
	~GalleryItem();

	std::string& getTime();
	GalleryItemType getItemType();
	std::string& getTimeFrom();
	std::string& getTimeTo();
	std::string& getLocation();
	std::string& getLocalPath();
	std::string& getItemName();
	std::string& getDescription();
	std::vector<std::string>& getTaggedUsers();
};
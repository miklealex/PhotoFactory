#include "GalleryItem.h"

DLL_EXPORT GalleryItem::GalleryItem(GalleryItemType type, std::string& time, std::string& timeFrom, std::string& timeTo, std::string& location, std::string& itemName, std::string& description, std::vector<std::string> &taggedUsers)
	:m_type(type), m_time(time), m_timeFrom(timeFrom), m_timeTo(timeTo), m_location(location), m_itemName(itemName), m_description{ description }, m_taggedUserNames(taggedUsers) {}

DLL_EXPORT GalleryItem::~GalleryItem()
{
	m_taggedUserNames.clear();
}

DLL_EXPORT GalleryItemType GalleryItem::getItemType()
{
	return m_type;
}

DLL_EXPORT std::string& GalleryItem::getDescription()
{
	return m_description;
}

DLL_EXPORT std::string& GalleryItem::getItemName()
{
	return m_itemName;
}

DLL_EXPORT std::string& GalleryItem::getLocalPath()
{
	return m_localPath;
}

DLL_EXPORT std::string& GalleryItem::getLocation()
{
	return m_location;
}

DLL_EXPORT std::string& GalleryItem::getTime()
{
	return m_time;
}

DLL_EXPORT std::string& GalleryItem::getTimeFrom()
{
	return m_timeFrom;
}

DLL_EXPORT std::string& GalleryItem::getTimeTo()
{
	return m_timeTo;
}

DLL_EXPORT std::vector<std::string>& GalleryItem::getTaggedUsers()
{
	return m_taggedUserNames;
}
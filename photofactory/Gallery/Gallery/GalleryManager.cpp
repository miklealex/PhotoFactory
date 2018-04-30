#include "GalleryManager.h"

DLL_EXPORT GalleryManager::GalleryManager()
{
	boost::filesystem::path path = boost::filesystem::current_path() /= std::string("/Gallery.xml");
	const std::string filePath = path.string();
	if (!boost::filesystem::exists(filePath.c_str()))
	{
		boost::filesystem::ofstream out{ filePath };
		out.close();
	}
	m_xmlFilePath = filePath;
}

DLL_EXPORT GalleryManager& GalleryManager::getGalleryManager()
{
	static GalleryManager m_manager;
	return m_manager;
}

DLL_EXPORT std::unordered_map<std::string, GalleryItem> GalleryManager::loadGalleryForUser(const std::string& username)
{
	XMLParserForGallery& xmlParser = XMLParserForGallery::getInstance();
	std::unordered_map<std::string, GalleryItem> currentItemsMap;
	xmlParser.ParseFile(m_xmlFilePath, username, currentItemsMap);
	return currentItemsMap;
}

DLL_EXPORT std::unordered_map<std::string, GalleryItem> GalleryManager::loadGalleryForUserForTime(const std::string& username,const  std::string& time)
{
	XMLParserForGallery& xmlParser = XMLParserForGallery::getInstance();
	std::unordered_map<std::string, GalleryItem> currentItemsMap;
	std::unordered_map<std::string, GalleryItem> filteredItemsMap;
	xmlParser.ParseFile(m_xmlFilePath, username, currentItemsMap);
	for (auto it = currentItemsMap.begin(); it != currentItemsMap.end(); ++it)
	{
		if (it->second.getTime() == time)
		{
			filteredItemsMap.insert(std::make_pair<std::string, GalleryItem>(std::move(it->second.getItemName()), std::move(it->second)));
		}
	}
	return filteredItemsMap;
}

DLL_EXPORT bool GalleryManager::addNewGalleryItem(GalleryItem& curItem, const std::string& username)
{
	return updateGalleryItem(curItem, username);
}

DLL_EXPORT bool GalleryManager::updateGalleryItem(GalleryItem& curItem, const std::string& username)
{
	XMLParserForGallery& xmlParser = XMLParserForGallery::getInstance();
	return xmlParser.UpdateItem(m_xmlFilePath, username, curItem);
}

DLL_EXPORT bool GalleryManager::removeGalleryItem(GalleryItem& curItem, const std::string& username)
{
	XMLParserForGallery& xmlParser = XMLParserForGallery::getInstance();
	return xmlParser.RemoveItem(m_xmlFilePath, username, curItem);
}

DLL_EXPORT std::unordered_map<std::string, GalleryItem> GalleryManager::loadGalleryForUserFromToTime(const std::string& username, const std::string& fromTime, const std::string& toTime)
{
	std::unordered_map<std::string, GalleryItem> curMap = loadGalleryForUser(username);
	boost::gregorian::date dateFrom = boost::gregorian::from_simple_string(fromTime);
	boost::gregorian::date dateTo = boost::gregorian::from_simple_string(toTime);
	std::unordered_map<std::string, GalleryItem> outMap;
	for (auto it = curMap.begin(); it != curMap.end(); ++it)
	{
		std::string itemTimeFrom = it->second.getTimeFrom();
		std::string itemTimeTo = it->second.getTimeTo();
		std::string itemTime = it->second.getTime();
		if (itemTimeFrom.size() > 0 && itemTimeTo.size() > 0)
		{
			if (boost::gregorian::from_simple_string(itemTimeFrom) >= dateFrom && boost::gregorian::from_simple_string(itemTimeTo) <= dateTo)
			{
				outMap.insert(std::make_pair<std::string, GalleryItem>(std::move(it->second.getItemName()), std::move(it->second)));
			}
		}
		else if (itemTime.size() > 0)
		{
			boost::gregorian::date curTime = boost::gregorian::from_simple_string(itemTime);
			if (curTime >= dateFrom && curTime <= dateTo)
			{
				outMap.insert(std::make_pair<std::string, GalleryItem>(std::move(it->second.getItemName()), std::move(it->second)));
			}
		}
		else
		{
			break;
		}
	}
	return outMap;
}

DLL_EXPORT std::unordered_map<std::string, GalleryItem> GalleryManager::loadGalleryForUserForExactLocation(const std::string& username, const std::string& location)
{
	std::unordered_map<std::string, GalleryItem> curMap = loadGalleryForUser(username);
	std::unordered_map<std::string, GalleryItem> outMap;

	for (auto it = curMap.begin(); it != curMap.end(); ++it)
	{
		if (it->second.getLocation() == location)
		{
			outMap.insert(std::make_pair<std::string, GalleryItem>(std::move(it->second.getItemName()), std::move(it->second)));
		}
	}
	return outMap;
}
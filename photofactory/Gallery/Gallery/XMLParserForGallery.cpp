#include "XMLParserForGallery.h"

DLL_EXPORT XMLParserForGallery& XMLParserForGallery::getInstance()
{
	static XMLParserForGallery m_parser;
	return m_parser;
}

DLL_EXPORT bool XMLParserForGallery::ParseFile(const std::string& filePath, const std::string& username, std::unordered_map<std::string, GalleryItem>& outItemsMap)
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

	TiXmlElement* root = doc.FirstChildElement("GalleryItems");
	if (root)
	{
		TiXmlElement* curUser = root->FirstChildElement("user");
		while (curUser)
		{
			if (curUser->Attribute("username") == username)
			{
				TiXmlElement* curEl = curUser->FirstChildElement("item");
				if (!curEl)
				{
					return false;
				}
				while (curEl)
				{
					std::string name = curEl->Attribute("name");
					TiXmlElement* itemProperty = curEl->FirstChildElement("type");
					if (!itemProperty)
					{
						return false;
					}
					std::string itemType = itemProperty->GetText();
					TiXmlElement* itemTim = curEl->FirstChildElement("time");
					std::string itemTime = "";
					if (itemTim)
					{
						itemTime = itemTim->GetText();
					}
					TiXmlElement* itemTimeFrom = curEl->FirstChildElement("timeFrom");
					std::string timeFrom = "";
					if (itemTimeFrom)
					{
						timeFrom = itemTimeFrom->GetText();
					}
					TiXmlElement* itemTimeTo = curEl->FirstChildElement("timeTo");
					std::string timeTo = "";
					if (itemTimeTo)
					{
						timeTo = itemTimeTo->GetText();
					}
					TiXmlElement* itemLocation = curEl->FirstChildElement("location");
					if (!itemLocation)
					{
						return false;
					}
					std::string location = itemLocation->GetText();
					TiXmlElement* itemDescription = curEl->FirstChildElement("description");
					if (!itemDescription)
					{
						return false;
					}
					std::string description = itemDescription->GetText();
					TiXmlElement* itemLocalPath = curEl->FirstChildElement("localPath");
					if (!itemLocalPath)
					{
						return false;
					}
					std::string localPath = itemLocalPath->GetText();
					TiXmlElement* itemTaggedNames = curEl->FirstChildElement("tagged");
					std::vector<std::string> taggedNames;
					if (itemTaggedNames)
					{
						TiXmlElement* curTagged = itemTaggedNames->FirstChildElement("name");
						if (!curTagged)
						{
							return false;
						}
						while (curTagged)
						{
							taggedNames.push_back(curTagged->GetText());
							curTagged = curTagged->NextSiblingElement();
						} 
					}
					if (itemType.size() > 1)
					{
						return false;
					}
					GalleryItem curItem(GalleryItemType(itemType[0] - '0'), itemTime, timeFrom, timeTo, location, name, description, taggedNames);
					outItemsMap.insert(std::make_pair<std::string, GalleryItem>(std::move(name), std::move(curItem)));
					curEl = curEl->NextSiblingElement();
				}
			}
			curUser = curUser->NextSiblingElement();
		}
		return true;
	}
	return false;
}

DLL_EXPORT bool XMLParserForGallery::UpdateFile(const std::string& filePath, const std::string& username, std::unordered_map<std::string, GalleryItem>& usersMap) const
{
	TiXmlDocument doc(filePath.c_str());
	if (!doc.LoadFile())
	{
		TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
		doc.LinkEndChild(declaration);

		TiXmlElement* firstEl = new TiXmlElement("GalleryItems");
		doc.LinkEndChild(firstEl);
		TiXmlElement* root = new TiXmlElement("user");
		root->SetAttribute("username", username.c_str());
		firstEl->LinkEndChild(root);

		for (auto it = usersMap.begin(); it != usersMap.end(); ++it)
		{
			if (!WriteItemToFile(root, it->second)) return false;
		}

	}
	else
	{
		std::vector<std::string> namesV;
		for (auto it = usersMap.begin(); it != usersMap.end(); ++it)
		{
			namesV.push_back(it->second.getItemName().c_str());
		}
		TiXmlElement* items = doc.FirstChildElement("GalleryItems");
		if (!items)
		{
			items = new TiXmlElement("GalleryItems");
			doc.LinkEndChild(items);
		}
		TiXmlElement* curUser = items->FirstChildElement("user");
		int nameIndex = 0;
		if (!curUser)
		{
			curUser = new TiXmlElement("user");
			curUser->SetAttribute("name", namesV[nameIndex].c_str());
			items->LinkEndChild(curUser);
			++nameIndex;
		}
		TiXmlElement* curItem = curUser->FirstChildElement("item");
		for (; curItem != nullptr; curItem = curItem->NextSiblingElement())
		{
			if (!curItem)
			{
				curItem = new TiXmlElement("item");
				curItem->SetAttribute("name", namesV[nameIndex].c_str());
				++nameIndex;
			}
			auto it = usersMap.find(std::string{ curItem->Attribute("name") });
			if (it == usersMap.end())
			{
				curItem->Clear();
				continue;
			}
			if (!UpdateItemInFile(curItem, it->second))
			{
				return false;
			}
		}
		
	}
	doc.SaveFile(filePath.c_str());
	return true;
}

DLL_EXPORT bool XMLParserForGallery::WriteItemToFile(TiXmlElement* currentElement, GalleryItem& currentUser) const
{
	TiXmlElement* curItem = new TiXmlElement("item");
	curItem->SetAttribute("name", currentUser.getItemName().c_str());
	currentElement->LinkEndChild(curItem);
	TiXmlElement* itemType = new TiXmlElement("type");
	itemType->LinkEndChild(new TiXmlText(std::to_string(currentUser.getItemType()).c_str()));
	curItem->LinkEndChild(itemType);
	if (currentUser.getTime().size() != 0)
	{
		TiXmlElement* itemTime = new TiXmlElement("time");
		itemTime->LinkEndChild(new TiXmlText(currentUser.getTime().c_str()));
		curItem->LinkEndChild(itemTime);
	}
	if (currentUser.getTimeFrom().size() != 0)
	{
		TiXmlElement* itemTimeFrom = new TiXmlElement("timeFrom");
		itemTimeFrom->LinkEndChild(new TiXmlText(currentUser.getTimeFrom().c_str()));
		curItem->LinkEndChild(itemTimeFrom);
	}
	if (currentUser.getTimeTo().size() != 0)
	{
		TiXmlElement* itemTimeTo = new TiXmlElement("timeTo");
		itemTimeTo->LinkEndChild(new TiXmlText(currentUser.getTimeTo().c_str()));
		curItem->LinkEndChild(itemTimeTo);
	}
	TiXmlElement* itemLocation = new TiXmlElement("location");
	itemLocation->LinkEndChild(new TiXmlText(currentUser.getLocation().c_str()));
	curItem->LinkEndChild(itemLocation);
	TiXmlElement* itemLocalPath = new TiXmlElement("localPath");
	itemLocalPath->LinkEndChild(new TiXmlText(currentUser.getLocalPath().c_str()));
	curItem->LinkEndChild(itemLocalPath);
	TiXmlElement* itemDescription = new TiXmlElement("description");
	itemDescription->LinkEndChild(new TiXmlText(currentUser.getDescription().c_str()));
	curItem->LinkEndChild(itemDescription);
	TiXmlElement* tagged = new TiXmlElement("tagged");
	curItem->LinkEndChild(tagged);
	for (auto it = currentUser.getTaggedUsers().begin(); it != currentUser.getTaggedUsers().end(); ++it)
	{
		TiXmlElement* curName = new TiXmlElement("name");
		curName->LinkEndChild(new TiXmlText(it->c_str()));
		tagged->LinkEndChild(curName);
	}
	return true;
}

DLL_EXPORT bool XMLParserForGallery::UpdateItemInFile(TiXmlElement* currentElement, GalleryItem& currentUser) const
{
	if (!currentElement)
	{
		return false;
	}
	TiXmlElement* itemType = currentElement->FirstChildElement("type");
	if (!itemType)
	{
		itemType = new TiXmlElement("type");
		itemType->LinkEndChild(new TiXmlText(std::to_string(currentUser.getItemType()).c_str()));
		currentElement->LinkEndChild(itemType);
	}
	else if (std::string{ itemType->GetText() } != std::to_string(currentUser.getItemType()))
	{
		itemType->Clear();
		itemType->LinkEndChild(new TiXmlText(std::to_string(currentUser.getItemType()).c_str()));
	}
	TiXmlElement* itemTime = currentElement->FirstChildElement("time");
	if (!itemTime)
	{
		itemTime = new TiXmlElement("time");
		itemTime->LinkEndChild(new TiXmlText(currentUser.getTime().c_str()));
		currentElement->LinkEndChild(itemTime);
	}
	else if(std::string{itemTime->GetText()} != currentUser.getTime())
	{
		itemTime->Clear();
		itemTime->LinkEndChild(new TiXmlText(currentUser.getTime().c_str()));
	}
	TiXmlElement* itemTimeFrom = currentElement->FirstChildElement("timeFrom");
	if (!itemTimeFrom)
	{
		itemTimeFrom = new TiXmlElement("timeFrom");
		itemTimeFrom->LinkEndChild(new TiXmlText(currentUser.getTimeFrom().c_str()));
		currentElement->LinkEndChild(itemTimeFrom);
	}
	else if (std::string{ itemTimeFrom->GetText() } != currentUser.getTimeFrom())
	{
		itemTimeFrom->Clear();
		itemTimeFrom->LinkEndChild(new TiXmlText(currentUser.getTimeFrom().c_str()));
	}
	TiXmlElement* itemTimeTo = currentElement->FirstChildElement("timeFrom");
	if (!itemTimeTo)
	{
		itemTimeTo = new TiXmlElement("timeTo");
		itemTimeTo->LinkEndChild(new TiXmlText(currentUser.getTimeTo().c_str()));
		currentElement->LinkEndChild(itemTimeTo);
	}
	else if (std::string{ itemTimeTo->GetText() } != currentUser.getTimeTo())
	{
		itemTimeTo->Clear();
		itemTimeTo->LinkEndChild(new TiXmlText(currentUser.getTimeTo().c_str()));
	}
	TiXmlElement* itemLocation = currentElement->FirstChildElement("location");
	if (!itemLocation)
	{
		itemLocation = new TiXmlElement("location");
		itemLocation->LinkEndChild(new TiXmlText(currentUser.getLocation().c_str()));
		currentElement->LinkEndChild(itemLocation);
	}
	else if (std::string{ itemLocation->GetText() } != currentUser.getLocation())
	{
		itemLocation->Clear();
		itemLocation->LinkEndChild(new TiXmlText(currentUser.getLocation().c_str()));
	}
	TiXmlElement* itemLocationPath = currentElement->FirstChildElement("localPath");
	if (!itemLocationPath)
	{
		itemLocationPath = new TiXmlElement("localPath");
		itemLocationPath->LinkEndChild(new TiXmlText(currentUser.getLocalPath().c_str()));
		currentElement->LinkEndChild(itemLocationPath);
	}
	else if (std::string{ itemLocationPath->GetText() } != currentUser.getLocalPath())
	{
		itemLocationPath->Clear();
		itemLocationPath->LinkEndChild(new TiXmlText(currentUser.getLocalPath().c_str()));
	}
	TiXmlElement* itemDescription = currentElement->FirstChildElement("description");
	if (!itemDescription)
	{
		itemDescription = new TiXmlElement("description");
		itemDescription->LinkEndChild(new TiXmlText(currentUser.getDescription().c_str()));
		currentElement->LinkEndChild(itemDescription);
	}
	else if (std::string{ itemDescription->GetText() } != currentUser.getDescription())
	{
		itemDescription->Clear();
		itemDescription->LinkEndChild(new TiXmlText(currentUser.getDescription().c_str()));
	}
	TiXmlElement* tagged = currentElement->FirstChildElement("tagged");
	if (!tagged)
	{
		tagged = new TiXmlElement("tagged");
		currentElement->LinkEndChild(tagged);
	}
	if (currentUser.getTaggedUsers().size())
	{
		TiXmlElement* curTagged = tagged->FirstChildElement("name");
		if (!curTagged)
		{
			curTagged = new TiXmlElement("name");
			curTagged->LinkEndChild(new TiXmlText(currentUser.getTaggedUsers()[0].c_str()));
			tagged->LinkEndChild(curTagged);
		}
		else if (std::string{ curTagged->GetText() } != currentUser.getTaggedUsers()[0])
		{
			curTagged->Clear();
			curTagged->LinkEndChild(new TiXmlText(currentUser.getTaggedUsers()[0].c_str()));
		}
		curTagged = curTagged->NextSiblingElement();
		for (int i = 1; curTagged != nullptr; curTagged = curTagged->NextSiblingElement(), ++i)
		{
			if (!curTagged)
			{
				curTagged = new TiXmlElement("name");
				curTagged->LinkEndChild(new TiXmlText(currentUser.getTaggedUsers()[i].c_str()));
				tagged->LinkEndChild(curTagged);
			}
			else if (std::string{ curTagged->GetText() } != currentUser.getTaggedUsers()[i])
			{
				curTagged->Clear();
				curTagged->LinkEndChild(new TiXmlText(currentUser.getTaggedUsers()[i].c_str()));
			}
		}
		while (curTagged)
		{
			curTagged->Clear();
			curTagged = curTagged->NextSiblingElement();
		}
	}
	return true;
}

DLL_EXPORT bool XMLParserForGallery::UpdateItem(const std::string& filePath, const std::string& username, GalleryItem& curItem) const
{
	TiXmlDocument doc(filePath.c_str());
	if (!doc.LoadFile())
	{
		TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
		doc.LinkEndChild(declaration);

		TiXmlElement* firstEl = new TiXmlElement("GalleryItems");
		doc.LinkEndChild(firstEl);
		TiXmlElement* root = new TiXmlElement("user");
		root->SetAttribute("username", username.c_str());
		firstEl->LinkEndChild(root);

		return WriteItemToFile(root, curItem);

	}
	else
	{
		TiXmlElement* items = doc.FirstChildElement("GalleryItems");
		if (!items)
		{
			items = new TiXmlElement("GalleryItems");
			doc.LinkEndChild(items);
		}
		TiXmlElement* curUser = items->FirstChildElement("user");
		bool found = false;
		if (!curUser)
		{
			curUser = new TiXmlElement("user");
			curUser->SetAttribute("username", username.c_str());
			if (UpdateItemInFile(curUser, curItem))
			{
				doc.SaveFile(filePath.c_str());
				return true;
			}
			else
			{
				return false;
			}
		}
		
		while (curUser && curUser->Attribute("username") != username)
		{
			curUser = curUser->NextSiblingElement();
		}
		if (!curUser)
		{
			curUser = new TiXmlElement("user");
			curUser->SetAttribute("username", username.c_str());
			if (UpdateItemInFile(curUser, curItem))
			{
				doc.SaveFile(filePath.c_str());
				return true;
			}
			else
			{
				return false;
			}
		}
		if (curUser->Attribute("username") == username)
		{
			TiXmlElement* item = curUser->FirstChildElement("item");
			if (!item)
			{
				if (WriteItemToFile(curUser, curItem))
				{
					doc.SaveFile(filePath.c_str());
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				std::string itemName = curItem.getItemName();
				while (item && item->Attribute("name") != itemName.c_str())
				{
					item = item->NextSiblingElement();
				}
				if (!item)
				{
					if (WriteItemToFile(curUser, curItem))
					{
						doc.SaveFile(filePath.c_str());
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					if (UpdateItemInFile(item, curItem))
					{
						doc.SaveFile(filePath.c_str());
						return true;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
}

DLL_EXPORT bool XMLParserForGallery::RemoveItem(const std::string& filePath, const std::string& username, GalleryItem& curItem) const
{
	TiXmlDocument doc(filePath.c_str());
	if (!doc.LoadFile())
	{
		return false;
	}
	else
	{
		TiXmlElement* items = doc.FirstChildElement("GalleryItems");
		if (!items)
		{
			return false;
		}
		TiXmlElement* curUser = items->FirstChildElement("user");
		if (!curUser)
		{
			return false;
		}
		while (curUser && curUser->Attribute("username") != username)
		{
			curUser = curUser->NextSiblingElement();
		}
		if (!curUser)
		{
			return false;
		}
		TiXmlElement* curItemx = curUser->FirstChildElement("item");
		if (!curItemx)
		{
			return false;
		}
		while (curItemx && curItemx->Attribute("name") != curItem.getItemName().c_str())
		{
			curItemx = curItemx->NextSiblingElement();
		}
		if (!curItemx)
		{
			return false;
		}
		else
		{
			curItemx->Clear();
			doc.SaveFile(filePath.c_str());
			return true;
		}
	}
}
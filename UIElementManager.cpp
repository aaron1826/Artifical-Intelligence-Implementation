#include "UIElementManager.h"

#include "Game.h"					//Managers

using namespace DirectX;
using namespace rapidjson;

bool UIElementManager::GetNavigationMenuByTypeID(NavigationalMenu*& ui, size_t type)
{
	for (auto& a : m_NavigationElements)
	{
		//Find free element
		if (a->GetUsageState() && a->GetID() == type)
		{
			if(ui)
				ReleaseNavUIElement(ui);
			//Flag it in use, copy pointer and return true
			a->SetUsageState(false);
			ui = a.get();
			return true;
		}
	}
	//Failed to find any free element
	return false;
}

bool UIElementManager::GetNonNavigationUIByTypeID(NonNavigationUI*& ui, size_t type)
{
	for (auto& a : m_NonNavigationElements)
	{
		//Find free element
		if (a->GetUsageState() && a->GetID() == type)
		{
			//If already a menu holding a menu, release it and ready it for changing ptr
			if (ui)
				ReleaseNonNavUIElement(ui);
			//Flag it in use, copy pointer and return true
			a->SetUsageState(false);
			ui = a.get();
			return true;
		}
	}
	//Failed to find any free element
	return false;
}

void UIElementManager::SetupUIElementsViaFile(std::string& uiFilepath)
{
	//Create and parse document
	Document doc;
	ParseNewJSONDocument(doc, uiFilepath);

	//Assert element existance
	AssertFileMembersType1(doc);

	//Start parsing
	
	//Parse Combat UIs
	const Value& arr = doc["Combat UIs"].GetArray();
	for (unsigned int i(0); i < arr.GetArray().Size(); ++i)
	{
		//Need to configure the object with a different base class, so switch between types
		switch (arr[i]["UI Type"].GetInt())
		{
		case UITypes::NAVIGATEABLE_01:
			SetupNavigationElementType1(arr, i);
			break;
		case UITypes::NON_NAVIGATABLE_01:
			SetupNonNavigationElementType1(arr, i);
			break;
		}
	}
	//Parse Unit Frames & Tooltips
	const Value& arr2 = doc["Unit Frames & Tooltips"].GetArray();
	for (unsigned int i(0); i < arr2.GetArray().Size(); ++i)
	{
		//Need to configure the object with a different base class, so switch between types
		switch (arr2[i]["UI Type"].GetInt())
		{
		case UITypes::NAVIGATEABLE_01:
			SetupNavigationElementType1(arr2, i);
			break;
		case UITypes::NON_NAVIGATABLE_01:
			SetupNonNavigationElementType1(arr2, i);
			break;
		}
	}
}


void UIElementManager::ReleaseNavUIElement(NavigationalMenu*& modeHeldPtr)
{
	//safety check
	if (modeHeldPtr)
	{
		modeHeldPtr->SetUsageState(true);
		modeHeldPtr = nullptr;
	}
}

void UIElementManager::ReleaseNonNavUIElement(NonNavigationUI*& modeHeldPtr)
{
	//safety check
	if (modeHeldPtr)
	{
		modeHeldPtr->SetUsageState(true);
		modeHeldPtr = nullptr;
	}
}

void UIElementManager::AssertFileMembersType1(rapidjson::Document& doc)
{

}

void UIElementManager::SetupNavigationElementType1(const Value& arr, int index)
{
	//Get Game
	Game* game = Game::GetGame();

	//Load associated textures
	auto uiTex = game->GetAssetManager().GetSpriteTextureData(std::string(arr[index]["UI Texture Access Name"].GetString()));
	auto highlightTex = game->GetAssetManager().GetSpriteTextureData(std::string(arr[index]["Cursor Texture Access Name"].GetString()));

	//Create a number of elements from the files specifications
	for (unsigned int i(0); i < arr[index]["Creation Count"].GetUint(); ++i)
	{
		//Create new nav element
		auto newUI = std::make_shared<NavigationalMenu>(uiTex, highlightTex);

		//Setup element with values

		//Set Position details
		newUI->GetPrimarySprite().SetPosition(arr[index]["UI Pos X"].GetFloat(), arr[index]["UI Pos Y"].GetFloat());
		newUI->GetCursorSprite().SetPosition(arr[index]["Cursor Anchor Pos X"].GetFloat(), arr[index]["Cursor Anchor Pos Y"].GetFloat());
		newUI->SetAnchorPosition(newUI->GetCursorSprite().GetPosition());
		newUI->SetTextAnchorPosition(arr[index]["Text Anchor Pos X"].GetFloat(), arr[index]["Text Anchor Pos Y"].GetFloat());

		//Set index details
		newUI->SetIndex(arr[index]["Default Index Pos X"].GetInt(), arr[index]["Default Index Pos Y"].GetInt());
		newUI->SetMaxIndex(arr[index]["Index Max X"].GetInt(), arr[index]["Index Max Y"].GetInt());
		newUI->SetDefaultIndex(arr[index]["Default Index Pos X"].GetInt(), arr[index]["Default Index Pos Y"].GetInt());

		//Set Offsets
		newUI->SetCursorOffset(arr[index]["Cursor Offset X"].GetFloat(), arr[index]["Cursor Offset Y"].GetFloat());

		//Set Frames
		newUI->GetPrimarySprite().SetFrame(arr[index]["UI Frame ID"].GetInt());
		newUI->GetCursorSprite().SetFrame(arr[index]["Cursor Frame ID"].GetInt());

		//Load menu strings
		newUI->GetMenuStrings().reserve(arr[index]["UI Strings"].GetArray().Size());
		for(unsigned int j(0); j < arr[index]["UI Strings"].GetArray().Size(); ++j)
		{
			newUI->GetMenuStrings().push_back(arr[index]["UI Strings"][j]["Message"].GetString());
		}

		//Finalise details
		newUI->SetID(arr[index]["Unique UI ID"].GetInt());
		newUI->SetUsageState(true);

		//Push into container
		m_NavigationElements.push_back(newUI);
	}		
}

void UIElementManager::SetupNonNavigationElementType1(const rapidjson::Value& arr, int index)
{
	//Get Game
	Game* game = Game::GetGame();
	//Load associated textures
	auto uiTex = game->GetAssetManager().GetSpriteTextureData(std::string(arr[index]["UI Texture Access Name"].GetString()));

	//Create a number of elements from the files specifications
	for (unsigned int i(0); i < arr[index]["Creation Count"].GetUint(); ++i)
	{
		auto newUI = std::make_shared<NonNavigationUI>();

		//Setup element with values

		//Set texture
		newUI->GetUISprite().SetTexturePtr(uiTex);
		//Set position details
		newUI->GetUISprite().SetPosition(arr[index]["UI Pos X"].GetFloat(), arr[index]["UI Pos Y"].GetFloat());

		//Load UI strings
		for (unsigned int j(0); j < arr[index]["UI Strings"].GetArray().Size(); ++j)
		{
			newUI->GetTextStrings().push_back(arr[index]["UI Strings"][j]["Message"].GetString());
		}
		//Store message position details
		for (unsigned int k(0); k < arr[index]["String Positions"].GetArray().Size(); ++k)
		{
			newUI->GetTextPositions().push_back(XMFLOAT2(
				arr[index]["String Positions"][k]["Pos X"].GetFloat(),
				arr[index]["String Positions"][k]["Pos Y"].GetFloat())
			);
		}

		//Set frames
		newUI->GetUISprite().SetFrame(arr[index]["UI Frame ID"].GetInt());

		//Finalise details
		newUI->SetID(arr[index]["Unique UI ID"].GetInt());
		newUI->SetUsageState(true);
		newUI->ResetDrawStrings();


		m_NonNavigationElements.push_back(newUI);
	}

}

void UIElementManager::Release()
{
	m_NavigationElements.clear();
	m_NonNavigationElements.clear();
}



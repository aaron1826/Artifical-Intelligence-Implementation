#pragma once
#include "EntityInterface.h"
#include "RapidJSONLoaderUtils.h"	//File loading

#include "NavigatableUIElement.h"	//Menu Items
#include "NonNavigationUIElement.h" //Unit frames, tooltips etc

//Order this in accordance with the frames in the JSON file
enum UIElementIDs
{
	UNIT_MENU_01,
	UNIT_ACTION_MENU_01,
	UNIT_MAGIC_MENU_01,
	TOOLTIP_01,
	FRIENDLY_UNIT_FRAME_01,
	ENEMY_UNIT_FRAME_01,
	DECLARE_TARGET_FRAME_01,
	ABILITY_TOOLTIP_01,
	UI_ID_COUNT
};

enum UITypes
{
	NAVIGATEABLE_01,
	NON_NAVIGATABLE_01,
};

/*
	Specialised manager for holding both navigational and non-navigational UI elements.
	Stores common elements that would be used in more than one mode.
	Modes will get a hold of a pointer to the UI element (if its available), and will be responsible for managing its
	usage and then releasing it back to the manager when done.
*/
class UIElementManager
{
public:

	UIElementManager() { }
	~UIElementManager() { Release(); }

	///////////
	/// Get ///
	///////////

	//Finds first available common UI element of type that is free for use.
	//Flags item is in use on the way out.
	bool GetNavigationMenuByTypeID(NavigationalMenu*& ui, size_t type);

	bool GetNonNavigationUIByTypeID(NonNavigationUI*& ui, size_t type);

	//////////////////
	/// Operations ///
	//////////////////

	//Setup a suite of UI elements via file (Textures must be preloaded)
	void SetupUIElementsViaFile(std::string& uiFilepath);

	//Releases the mode held pointer and flags UI element as free
	static void ReleaseNavUIElement(NavigationalMenu*& modeHeldPtr);
	//Same for Non-Nav UI element >>>>NYI<<<<
	static void ReleaseNonNavUIElement(NonNavigationUI*& modeHeldPtr);

private:

	//Asserts json members for file loading
	void AssertFileMembersType1(rapidjson::Document& doc);
	//Setup Navigation Type 1 Element
	void SetupNavigationElementType1(const rapidjson::Value& arr, int index);
	//Setup NonNavigation Type 1 Element
	void SetupNonNavigationElementType1(const rapidjson::Value& arr, int index);


	//Clear containers
	void Release();

	//Containers for UI types
	std::vector<std::shared_ptr<NavigationalMenu>> m_NavigationElements;
	std::vector<std::shared_ptr<NonNavigationUI>> m_NonNavigationElements;

};
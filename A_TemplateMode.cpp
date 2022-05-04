#include "A_TemplateMode.h"

#include "RapidJSONLoaderUtils.h"   //Map data loader

#include "GeneralUtils.h"		//String Conversion
#include "TilemapUtils.h"		//Tilemap Loading Help

//Game Objects
#include "TestEntity.h"         //Entity Testing
#include "AnimTestEntity.h"	    //Animation Testing
#include "MapTile.h"			//Terrain
#include "UnitEntity.h"			//Game Unit
#include "CursorEntity.h"		//Cursor

using namespace DirectX;
using namespace rapidjson;
using namespace TiledLoaders;


A_PlaceholderMode::A_PlaceholderMode(size_t id)
	:ModeInterface(id), m_State(MODE_STATE::FREE_NAVIGATION)
{
	m_TeamOne.reserve(5);
	m_TeamTwo.reserve(5);

	//Populate Containers
	InitMapTiles();
	m_MapGeneration.GeneticAlgorithm();
	InitObjectContainers();
	InitUIFunctionality();
	FinaliseInit();

}

A_PlaceholderMode::~A_PlaceholderMode()
{
	Release();
}

void A_PlaceholderMode::Enter()
{
	InitModeAudio();
	Game::GetGame()->GetUIManager().GetNonNavigationUIByTypeID(m_TileTooltip, UIElementIDs::TOOLTIP_01);
	UpdateTileTooltip(m_TileTooltip, m_Cursor->GetCurrentTileObject());
}

void A_PlaceholderMode::Update(const GameTimer& gt)
{
	//If a team switch is flagged, call function here (might be alt line of update/render like a state when expanded later)
	if (m_SwitchTeams)
		SwitchTeams();


	//Update Actors
	for (auto& a : m_Actors)
	{
		a->Update(gt);
	}
	//Update Terrain
	for (auto& a : m_MapGeneration.GetFinalMap())
	{
		a->Update(gt);
	}

	//Update Looping Audio
	Game::GetGame()->GetAudioManager().UpdateAudioEngine(Game::GetGame()->GetAudioManager().GetSFXManager().m_SFXInstance0);
	//Check for camera updates
	m_Camera.UpdateCamera();
}

void A_PlaceholderMode::Render(PassData& data)
{
	//Get Command list for draws
	auto commandList = Game::GetGame()->GetCommandList().Get();

	//Render Background
	RenderBackground(data, commandList);

	//Render Scene Objects
	RenderScene(data, commandList);

	//Render UI Elements
	RenderUI(data, commandList);

}

void A_PlaceholderMode::ProcessKey(char key)
{
	switch (m_State)
	{
	case(MODE_STATE::FREE_NAVIGATION):
		ProcessKeyFreeMoveState(key);
		break;
	case(MODE_STATE::UNIT_MENU_OPEN):
		ProcessKeyUnitMenuOpenState(key);
		break;
	case(MODE_STATE::ACTION_MENU_OPEN):
		ProcessKeyUnitActionOpenState(key);
		break;
	case(MODE_STATE::MAGIC_MENU_OPEN):
		ProcessKeyMagicMenuOpenState(key);
		break;
	case(MODE_STATE::MOVE_MENU_OPEN):
		ProcessKeyMoveMenuOpenState(key);
		break;
	case(MODE_STATE::FIND_BASIC_ATTACK_TARGET):
		ProcessKeyFindAttackTargetState(key);
		break;
	case(MODE_STATE::FIND_HEAL_TARGET):
		ProcessKeyFindHealTargetState(key);
	}
}

void A_PlaceholderMode::Release()
{
	//Release Cursor
	delete m_Cursor;
	m_Cursor = nullptr;

	//Release other pointers

	m_UIElement1 = nullptr;
	m_UIElement2 = nullptr;
	m_UIElement3 = nullptr;
	m_UIElement4 = nullptr;
	m_TileTooltip = nullptr;
	m_FriendlyUnitUI = nullptr;
	m_EnemyUnitUI = nullptr;
	m_TextFrame1 = nullptr;
	m_SkillTooltip = nullptr;

	//Release All Containers

	for (auto& a : m_TeamOne)
		a = nullptr;
	for (auto& a : m_TeamTwo)
		a = nullptr;
	
	EntityInterface::ReleaseContainer(m_Backgrounds);
	EntityInterface::ReleaseContainer(m_Actors);
	EntityInterface::ReleaseContainer(m_Projectiles);

	for (auto& a : m_MapGeneration.GetFinalMap())
	{
		delete a;
		a = nullptr;
	}
	m_MapGeneration.GetFinalMap().clear();
}

void A_PlaceholderMode::InitObjectContainers()
{
	//Get Game Handle
	Game* game = Game::GetGame();

	///////////////////////////
	/// Setup background(s) ///
	///////////////////////////

	TestEntity* bg = new TestEntity(game->GetAssetManager().GetSpriteTextureData(std::string("Main_Game_BG_00")));
	EntityInterface::Add(m_Backgrounds, bg);


	//////////////////////
	/// Setup Entities ///
	//////////////////////	

	//Team One

	XMINT2 coords[] =
	{
		{20, 21},
		{21, 22},
		{22, 23},
		{21, 24},
		{20, 25}
	};
	int frames[] =
	{
		3, 5, 5, 5, 7
	};

	std::shared_ptr<SpriteTexture> unitTexs[] =
	{
		game->GetAssetManager().GetSpriteTextureData(std::string("WARRIOR_CLASS_SS_00")),
		game->GetAssetManager().GetSpriteTextureData(std::string("ASSASSIN_CLASS_SS_00")),
		game->GetAssetManager().GetSpriteTextureData(std::string("BARBARIAN_CLASS_SS_00")),
		game->GetAssetManager().GetSpriteTextureData(std::string("NECROMANCER_CLASS_SS_00")),
		game->GetAssetManager().GetSpriteTextureData(std::string("SORCERESS_CLASS_SS_00")),
	};

	for (int i(0); i < 5; ++i)
	{
		UnitEntity* unit = new UnitEntity(unitTexs[i]);

		unit->GetPrimarySprite().SetPosition((coords[i].x * m_MapGeneration.GetTileDims().x) + (m_MapGeneration.GetTileDims().x * 0.5f), (coords[i].y * m_MapGeneration.GetTileDims().y) + (m_MapGeneration.GetTileDims().y * 0.5f));
		unit->SetMapCoordinates(coords[i]);
		unit->SetMapCoordinateMax(m_MapGeneration.GetMapLimit());
		unit->SetTileSize(m_MapGeneration.GetTileDims());
		unit->GetPrimarySprite().SetScale(0.20f, 0.20f);
		unit->GetPrimarySprite().GetAnimator().SetAnimation(frames[i], true, true, false);
		unit->GetPrimarySprite().SetOriginToCenter();
		unit->SetUnitType(UnitEntity::UNIT_TYPE::LAND);
		unit->SetUnitTeamID(1);
		unit->SetUnitClass(1);
		unit->SetUnitBaseEquipment(1, 0, 2, 3, 4);
		unit->InitTotalValues();
		unit->SetSkillsBasedOnClass();

		EntityInterface::Add(m_Actors, unit);
		m_TeamOne.push_back(unit);

	}


	//Team Two

	XMINT2 coords2[] =
	{
		{27, 21},
		{26, 22},
		{25, 23},
		{26, 24},
		{27, 25}
	};

	for (int i(0); i < 5; ++i)
	{
		UnitEntity* unit = new UnitEntity(unitTexs[4-i]);

		unit->GetPrimarySprite().SetPosition((coords2[i].x * m_MapGeneration.GetTileDims().x) + (m_MapGeneration.GetTileDims().x * 0.5f), (coords2[i].y * m_MapGeneration.GetTileDims().y) + (m_MapGeneration.GetTileDims().y * 0.5f));
		unit->SetMapCoordinates(coords2[i]);
		unit->SetMapCoordinateMax(m_MapGeneration.GetMapLimit());
		unit->SetTileSize(m_MapGeneration.GetTileDims());
		unit->GetPrimarySprite().SetScale(0.20f, 0.20f);
		unit->GetPrimarySprite().GetAnimator().SetAnimation(frames[i], true, true, false);
		unit->GetPrimarySprite().SetOriginToCenter();
		unit->SetUnitType(UnitEntity::UNIT_TYPE::LAND);
		unit->SetUnitTeamID(2);
		unit->SetUnitClass(1);
		unit->SetUnitBaseEquipment(1, 0, 2, 3, 4);
		unit->InitTotalValues();
		unit->SetSkillsBasedOnClass();
		unit->GetPrimarySprite().SetEffect(SpriteEffects_FlipHorizontally);

		EntityInterface::Add(m_Actors, unit);
		m_TeamTwo.push_back(unit);
	}

}

void A_PlaceholderMode::InitUIFunctionality()
{
	//Get Game Handle
	Game* game = Game::GetGame();

	////////////////////
	/// Cursor Setup ///
	////////////////////

	//Get new cursor
	m_Cursor = new CursorEntity(Game::GetGame()->GetAssetManager().GetSpriteTextureData(std::string("UI_Atlas_01")));


	//Calculate the approximate middle of the screen by measuring how many tiles can fit on the screen (rounding if required)
	DirectX::XMINT2 m_CursorScreenOffset = { static_cast<int>(game->GetWindowSize().x / m_MapGeneration.GetTileDims().x) / 2, static_cast<int>(game->GetWindowSize().y / m_MapGeneration.GetTileDims().y) / 2 };

	//Offset the cursor to the middle of the screen
	m_Cursor->GetPrimarySprite().SetPosition((m_CursorScreenOffset.x * m_MapGeneration.GetTileDims().x) + (m_MapGeneration.GetTileDims().x * 0.5f), (m_CursorScreenOffset.y * m_MapGeneration.GetTileDims().y) + (m_MapGeneration.GetTileDims().y * 0.5f));
	m_Cursor->GetPrimarySprite().SetFrame(20);
	m_Cursor->GetPrimarySprite().SetOriginToCenter();
	m_Cursor->SetMapCoordinates(m_MapStartingCoords);
	m_Cursor->SetMapCoordinateMax(m_MapGeneration.GetMapLimit());
	m_Cursor->SetTileSize(m_MapGeneration.GetTileDims());

	////////////////////
	/// Camera Setup ///
	////////////////////

	m_Camera.SetupCamera(
		m_CursorScreenOffset,
		XMINT2(0, 0),
		m_MapGeneration.GetMapLimit(),
		m_MapStartingCoords,
		m_MapGeneration.GetTileDims()
	);
}

void A_PlaceholderMode::InitMapTiles()
{
	//
	// JSON Setup
	//

	//Create Document to hold JSON data & parse data
	Document doc;
	ParseNewJSONDocument(doc, std::string("data/sprites/tilemaps/Tilemap_00.json"));

	//Assert all the members we are looking for are present
	AssertBasicTilemapMembers(doc);

	//Grab hold of each sub array
	const Value& layerArr = doc["layers"][0];
	const Value& tileArr = doc["tilesets"][0];

	//
	// Build Objects 
	//

	//Generate some simple, singular frames for each tile mathematically
	std::vector<RECT> frames;
	frames.reserve(tileArr["tilecount"].GetUint());
	GenerateFrameData(doc, 0, frames);

	//Extract & populate container with tile specific data for assignment
	std::unordered_map<int, TileProperties> mapProperties;
	LoadTilePropertiesData(doc, 0, mapProperties);

	//Get the name for the tileset (This should be the same name that is given at TextureLoad time)
	std::string mapName = tileArr["name"].GetString();
	//Get texture pointers for main texture and grid texture
	std::shared_ptr<SpriteTexture> texPtr = Game::GetGame()->GetAssetManager().GetSpriteTextureData(mapName);
	std::shared_ptr<SpriteTexture> gridPtr = Game::GetGame()->GetAssetManager().GetSpriteTextureData(std::string("UI_Atlas_01"));

	//Get the map dimensions to build the map accurately
	int mapRows = doc["height"].GetInt();
	int mapCols = doc["width"].GetInt();
	//Get tile dimensions
	int tileWidth = doc["tilewidth"].GetInt();
	int tileHeight = doc["tileheight"].GetInt();

	//Store map related information for later
	m_MapGeneration.GetTileDims().x = tileWidth;
	m_MapGeneration.GetTileDims().y = tileHeight;
	m_MapGeneration.GetMapLimit().x = mapCols - 1;
	m_MapGeneration.GetMapLimit().y = mapRows - 1;

	//counter for "data" sub-array
	int count = 0;
	//Start building the map (left->right, top->bot order)
	for (int y(0); y < mapRows; ++y)
	{
		for (int x(0); x < mapCols; ++x)
		{
			//Create new object and pass texture
			MapTile* tile = new MapTile(texPtr, gridPtr);

			//Get the appropriate tile ID using the data array value
			unsigned int tileID = doc["layers"][0]["data"][count++].GetUint();

			//Use tileID rotation flags to set rotation (if required)
			tile->GetPrimarySprite().SetRotation(GetTileRotationValue(tileID));

			//Iterate through container and find this tiles unique properties (- 1 because tiled offsets awkwardly)
			std::unordered_map<int, TileProperties>::iterator it = mapProperties.find(tileID - 1);
			if (it != mapProperties.end())
				tile->SetTileProperties(it->second);

			//Decrement the ID to align it with frames container
			--tileID;
			//Use ID to set correct RECT
			tile->GetPrimarySprite().SetRect(frames.at(tileID));
			//The tile might need rotating, so center the origin
			tile->GetPrimarySprite().SetOriginToCenter();

			//Set position through row tiling (add an offset to account for origin shift)
			tile->GetPrimarySprite().SetPosition(
				static_cast<float>((tileWidth * x) + tileWidth * 0.5f),
				static_cast<float>((tileWidth * y) + tileHeight * 0.5f)
			);

			//Set the tiles x/y coordinates in world space
			tile->SetMapCoordinates(x, y);
			tile->SetMapCoordinateMax(m_MapGeneration.GetMapLimit());

			//Configure the secondary sprite
			tile->MirrorTileToGridData();
			tile->GetGridSprite().SetFrame(21);
			tile->GetGridSprite().SetOriginToCenter();
			tile->GetGridSprite().SetColour(XMVECTOR{ 0.5f, 0.5f, 0.5f, 0.5f });


			//Add to container
			m_TileMap.push_back(tile);
		}
	}

	//Now all members are setup, need to set their neighbouring pointers for pathfinding purposes
	for (auto& t : m_TileMap)
	{
		//Get coords
		XMINT2& coords = t->GetMapCoordinates();

		//Do some checking to verify what neighbours should be added
		if (coords.y > 0)				//North Check
			t->SetNeighbourAtIndex(0, m_TileMap.at((coords.x + (coords.y - 1)) + ((coords.y - 1) * m_MapGeneration.GetMapLimit().y)));
		if (coords.x < m_MapGeneration.GetMapLimit().x)	//East Check
			t->SetNeighbourAtIndex(1, m_TileMap.at((coords.x + 1 + coords.y) + (coords.y * m_MapGeneration.GetMapLimit().y)));
		if (coords.y < m_MapGeneration.GetMapLimit().y)	//South Check
			t->SetNeighbourAtIndex(2, m_TileMap.at((coords.x + (coords.y + 1)) + ((coords.y + 1) * m_MapGeneration.GetMapLimit().y)));
		if (coords.x > 0)				//West Check
			t->SetNeighbourAtIndex(3, m_TileMap.at((coords.x - 1 + coords.y) + (coords.y * m_MapGeneration.GetMapLimit().y)));

	}

}

void A_PlaceholderMode::FinaliseInit()
{
	//Run cursor search for edge case errors
	m_Cursor->SearchForTileObject(m_MapGeneration.GetFinalMap(), m_MapGeneration.GetMapLimit().x);
	m_Cursor->SearchForUnitObject(m_Actors, m_MapGeneration.GetMapLimit().x);


	//Need to flag any tile that is occupied by an existing actor as occupied
	for (auto& a : m_Actors)
	{
		XMINT2& coords = a->GetMapCoordinates();
		m_MapGeneration.GetFinalMap().at((coords.x + coords.y) + (coords.y * m_MapGeneration.GetMapLimit().y))->GetTileProperties().occupied = true;
	}

	//Set starting team ID
	m_CurrentTeamID = 1;

}

void A_PlaceholderMode::InitModeAudio()
{
	Game::GetGame()->GetAudioManager().LoadSound(
		Game::GetGame()->GetAudioManager().GetSFXManager().m_SFX8,
		L"data/music/backgroundMusic.wav");

	Game::GetGame()->GetAudioManager().LoadSound(
		Game::GetGame()->GetAudioManager().GetSFXManager().m_SFX0,
		L"data/music/menuNavigation.wav");

	Game::GetGame()->GetAudioManager().PlayLoopingSound(
		Game::GetGame()->GetAudioManager().GetSFXManager().m_SFX8,
		Game::GetGame()->GetAudioManager().GetSFXManager().m_SFXInstance0, 0.4f);
}

void A_PlaceholderMode::OnStateEnter(MODE_STATE newState)
{
	UIElementManager* manager = &Game::GetGame()->GetUIManager();

	//Shift State BEFORE changes
	m_State = newState;

	switch (m_State)
	{
		//////////////////////////
		/// Camera Move States ///
		//////////////////////////

	case MODE_STATE::FREE_NAVIGATION:
		UpdateCursorFreeMode();
		break;
	case MODE_STATE::MOVE_MENU_OPEN:
		UpdateCursorTargetingMode();
		break;
	case MODE_STATE::FIND_BASIC_ATTACK_TARGET:
		UpdateCursorTargetingMode();
		break;
	case MODE_STATE::FIND_HEAL_TARGET:
		UpdateCursorTargetingMode();
		break;


		///////////////////
		/// Menu States ///
		///////////////////

	case MODE_STATE::UNIT_MENU_OPEN:
		break;
	case MODE_STATE::ACTION_MENU_OPEN:
		break;
	case MODE_STATE::ATTACK_MENU_OPEN:
		break;
	case MODE_STATE::MAGIC_MENU_OPEN:
		break;
	case MODE_STATE::EQUIPMENT_MENU_OPEN:
		break;


		/////////////////////
		/// Combat States ///
		/////////////////////

	case MODE_STATE::PHYS_ATTACK_STATE:
		break;
	case MODE_STATE::MAG_ATTACK_STATE:
		break;

	}
}

void A_PlaceholderMode::OnStateExit(MODE_STATE newState)
{
	UIElementManager* manager = &Game::GetGame()->GetUIManager();

	switch (m_State)
	{
		//////////////////////////
		/// Camera Move States ///
		//////////////////////////

	case MODE_STATE::FREE_NAVIGATION:
		break;
	case MODE_STATE::MOVE_MENU_OPEN:
		break;
	case MODE_STATE::FIND_BASIC_ATTACK_TARGET:
		break;
	case MODE_STATE::FIND_HEAL_TARGET:
		break;


		///////////////////
		/// Menu States ///
		///////////////////

	case MODE_STATE::UNIT_MENU_OPEN:		
		break;
	case MODE_STATE::ACTION_MENU_OPEN:
		break;
	case MODE_STATE::ATTACK_MENU_OPEN:
		break;
	case MODE_STATE::MAGIC_MENU_OPEN:
		break;
	case MODE_STATE::EQUIPMENT_MENU_OPEN:
		break;


		/////////////////////
		/// Combat States ///
		/////////////////////

	case MODE_STATE::PHYS_ATTACK_STATE:
		break;
	case MODE_STATE::MAG_ATTACK_STATE:
		break;

	}

	//Shift State BEFORE changes
	m_State = newState;

}

void A_PlaceholderMode::CheckIfTeamActionsSpent()
{
	//PH - Use const team count when added
	int unitCount = 0;
	int count = 5;

	if (m_CurrentTeamID == 1)
	{
		for (auto& a : m_TeamOne)
		{
			//If the unit has no actions, or is not alive, uptick the count
			if (!a->CheckRemainingActions() || !a->GetUnitStateFlags().isAlive)
			{
				++unitCount;
			}
		}
	}
	else
	{
		for (auto& a : m_TeamTwo)
		{
			//If the unit has no actions, or is not alive, uptick the count
			if (!a->CheckRemainingActions() || !a->GetUnitStateFlags().isAlive)
			{
				++unitCount;
			}
		}
	}

	if (unitCount >= count)
		m_SwitchTeams = true;

}

void A_PlaceholderMode::SwitchTeams()
{
	//Check which team is active before the switch and clean up their units on on the way out
	if (m_CurrentTeamID == 1)
		for (auto& a : m_TeamOne)
			a->EnableUnit();
	else
		for (auto& a : m_TeamTwo)
			a->EnableUnit();

	ChangeTeamID();
	UpdateCursorFreeMode();
	m_SwitchTeams = false;
}

void A_PlaceholderMode::ProcessKeyFreeMoveState(char key)
{
	UIElementManager* manager = &Game::GetGame()->GetUIManager();

	switch (key)
	{
	case VK_ESCAPE:
		//Open System Menu?
		break;

	case 'f':
	case 'F':
		//Check if the cursor found a unit of the same team ID
		if (m_Cursor->GetCurrentObject() && static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->GetUnitTeamID() == m_CurrentTeamID &&
			static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->CheckRemainingActions())
		{
			//Get Unit Menu
			manager->GetNavigationMenuByTypeID(m_UIElement1, UIElementIDs::UNIT_MENU_01);

			//Change State
			m_State = MODE_STATE::UNIT_MENU_OPEN;
		}
		break;

	case 'w':
	case 'W':
		m_Cursor->DecrementYCoordinate();
		m_Camera.MoveSceneDown();
		UpdateCursorFreeMode();
		break;

	case 's':
	case 'S':
		m_Cursor->IncrementYCoordinate();
		m_Camera.MoveSceneUp();
		UpdateCursorFreeMode();
		break;

	case 'a':
	case 'A':
		m_Cursor->DecrementXCoordinate();
		m_Camera.MoveSceneRight();
		UpdateCursorFreeMode();
		break;

	case 'd':
	case 'D':
		m_Cursor->IncrementXCoordinate();
		m_Camera.MoveSceneLeft();
		UpdateCursorFreeMode();
		break;

	}
}

void A_PlaceholderMode::ProcessKeyUnitMenuOpenState(char key)
{
	UIElementManager* manager = &Game::GetGame()->GetUIManager();

	switch (key)
	{
	case VK_ESCAPE:
		//Release unit menu & Update Cursor
		manager->ReleaseNavUIElement(m_UIElement1);
		UpdateCursorFreeMode();
		//Change State
		m_State = MODE_STATE::FREE_NAVIGATION;
		break;

	case 'f':
	case 'F':
	
		//Check what menu index is, and change menu based on it
		switch (m_UIElement1->GetMenuIndex().y)
		{
		case 0:  //Action
			//Check if the unit can act, and open menu if so
			if (static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->GetUnitStateFlags().canAct)
			{
				//Open Action Menu
				manager->GetNavigationMenuByTypeID(m_UIElement1, UIElementIDs::UNIT_ACTION_MENU_01);
				//Change State
				m_State = MODE_STATE::ACTION_MENU_OPEN;
			}			
			break;

		case 1:  //Move
			//Generate movement grid from unit position
			if (static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->GetUnitStateFlags().canMove)
			{
				m_PathFinder.GenerateTileGrid(m_MapGeneration.GetFinalMap(), static_cast<UnitEntity*>(m_Cursor->GetCurrentObject()), m_MapGeneration.GetMapLimit().x);
				//Release action menu
				manager->ReleaseNavUIElement(m_UIElement1);
				//Change State
				m_State = MODE_STATE::MOVE_MENU_OPEN;
			}
			break;

		case 2:  //Status
			//Open Status/Equip Menu Here
			break;

		case 3:  //End Turn
			//Run switch team process
			SwitchTeams();
			//Release current UI Elements
			manager->ReleaseNavUIElement(m_UIElement1);
			manager->ReleaseNonNavUIElement(m_FriendlyUnitUI);
			//Change State
			m_State = MODE_STATE::FREE_NAVIGATION;
			break;

		}
		break;
	
	case 'w':
	case 'W':
		m_UIElement1->MoveUp();
		Game::GetGame()->GetAudioManager().PlayOneShot(
			Game::GetGame()->GetAudioManager().GetSFXManager().m_SFX0);
		break;

	case 's':
	case 'S':
		m_UIElement1->MoveDown();
		Game::GetGame()->GetAudioManager().PlayOneShot(
			Game::GetGame()->GetAudioManager().GetSFXManager().m_SFX0);
		break;
	}
}

void A_PlaceholderMode::ProcessKeyUnitActionOpenState(char key)
{
	UIElementManager* manager = &Game::GetGame()->GetUIManager();

	switch (key)
	{
	case VK_ESCAPE:
		//Open Unit Menu
		manager->GetNavigationMenuByTypeID(m_UIElement1, UIElementIDs::UNIT_MENU_01);
		//Change state to Unit Menu
		m_State = MODE_STATE::UNIT_MENU_OPEN;
		break;

	case 'f':
	case 'F':
		//Get Menu Index, execute action based on index
		switch (m_UIElement1->GetMenuIndex().y)
		{
		case 0:
			//Open Targeting Menu
			manager->GetNonNavigationUIByTypeID(m_TextFrame1, UIElementIDs::DECLARE_TARGET_FRAME_01);
			//Release current UI & Update cursor
			manager->ReleaseNavUIElement(m_UIElement1);

			//Enable Effect
			FlagTilesBasic(true);

			UpdateCursorTargetingMode();
			//Change State to attack targeting
			m_State = MODE_STATE::FIND_BASIC_ATTACK_TARGET;
			break;

		case 1:
			//Open Targeting Menu
			manager->GetNonNavigationUIByTypeID(m_TextFrame1, UIElementIDs::DECLARE_TARGET_FRAME_01);
			//Release current UI & Update cursor
			manager->ReleaseNavUIElement(m_UIElement1);
			UpdateCursorTargetingMode();
			//Change State to heal targeting
			m_State = MODE_STATE::FIND_HEAL_TARGET;
			break;

		case 2:
			//Use Skill 2
			break;
		case 3:
			//Use Skill 3
			break;
		}
		break;

	case 'w':
	case 'W':
		m_UIElement1->MoveUp();
		Game::GetGame()->GetAudioManager().PlayOneShot(
			Game::GetGame()->GetAudioManager().GetSFXManager().m_SFX0);

		//Open ability tooltip if on a skill
		switch (m_UIElement1->GetMenuIndex().y)
		{
		case 0: //No ability, so close tooltip if open
			manager->ReleaseNonNavUIElement(m_SkillTooltip);
			break;
		case 1: //Get Skill 1 from the unit and display information on tooltip
			manager->GetNonNavigationUIByTypeID(m_SkillTooltip, UIElementIDs::ABILITY_TOOLTIP_01);
			break;
		case 2: //Get Skill 1 from the unit and display information on tooltip
			manager->GetNonNavigationUIByTypeID(m_SkillTooltip, UIElementIDs::ABILITY_TOOLTIP_01);
			break;
		case 3: //Get Skill 1 from the unit and display information on tooltip
			manager->GetNonNavigationUIByTypeID(m_SkillTooltip, UIElementIDs::ABILITY_TOOLTIP_01);
			break;
		}
		break;

	case 's':
	case 'S':
		m_UIElement1->MoveDown();
		Game::GetGame()->GetAudioManager().PlayOneShot(
			Game::GetGame()->GetAudioManager().GetSFXManager().m_SFX0);

		//Open ability tooltip if on a skill
		switch (m_UIElement1->GetMenuIndex().y)
		{
		case 0: //No ability, so close tooltip if open
			manager->ReleaseNonNavUIElement(m_SkillTooltip);
			break;
		case 1: //Get Skill 1 from the unit and display information on tooltip
			manager->GetNonNavigationUIByTypeID(m_SkillTooltip, UIElementIDs::ABILITY_TOOLTIP_01);
			break;
		case 2: //Get Skill 1 from the unit and display information on tooltip
			manager->GetNonNavigationUIByTypeID(m_SkillTooltip, UIElementIDs::ABILITY_TOOLTIP_01);
			break;
		case 3: //Get Skill 1 from the unit and display information on tooltip
			manager->GetNonNavigationUIByTypeID(m_SkillTooltip, UIElementIDs::ABILITY_TOOLTIP_01);
			break;
		}
		break;

	}
}

void A_PlaceholderMode::ProcessKeyMagicMenuOpenState(char key)
{
	UIElementManager* manager = &Game::GetGame()->GetUIManager();

	switch (key)
	{
	case VK_ESCAPE:
		//Open Unit Menu
		manager->GetNavigationMenuByTypeID(m_UIElement1, UIElementIDs::UNIT_MENU_01);
		//Change state to Unit Menu
		m_State = MODE_STATE::UNIT_MENU_OPEN;
		break;

	case 'f':
	case 'F':
		switch (m_UIElement1->GetMenuIndex().y)
		{
		case 0:
			//Release element, navigate to free nav
			manager->ReleaseNavUIElement(m_UIElement1);
			m_State = MODE_STATE::FIND_BASIC_ATTACK_TARGET;
			break;
		case 1:
			//Heal Here
			manager->ReleaseNavUIElement(m_UIElement1);
			m_State = MODE_STATE::FIND_HEAL_TARGET;
			break;
		case 2:
			//Attack Here
			break;
		}
		break;
	
	case 'w':
	case 'W':
		m_UIElement1->MoveUp();
		Game::GetGame()->GetAudioManager().PlayOneShot(
			Game::GetGame()->GetAudioManager().GetSFXManager().m_SFX0);
		break;
	case 's':
	case 'S':
		m_UIElement1->MoveDown();
		Game::GetGame()->GetAudioManager().PlayOneShot(
			Game::GetGame()->GetAudioManager().GetSFXManager().m_SFX0);
		break;
	case 'a':
	case 'A':
		m_UIElement1->MoveLeft();
		Game::GetGame()->GetAudioManager().PlayOneShot(
			Game::GetGame()->GetAudioManager().GetSFXManager().m_SFX0);
		break;
	case 'd':
	case 'D':
		m_UIElement1->MoveRight();
		Game::GetGame()->GetAudioManager().PlayOneShot(
			Game::GetGame()->GetAudioManager().GetSFXManager().m_SFX0);
		break;
	}
}

void A_PlaceholderMode::ProcessKeyMoveMenuOpenState(char key)
{
	UIElementManager* manager = &Game::GetGame()->GetUIManager();

	switch (key)
	{
	case VK_ESCAPE:
		//Open Unit menu
		manager->GetNavigationMenuByTypeID(m_UIElement1, UIElementIDs::UNIT_MENU_01);
		//Snap cursor & camera back to the unit
		m_Cursor->MoveFixedCursorCoords(m_Cursor->GetCurrentObject()->GetMapCoordinates());
		m_Camera.MoveCameraToCoordinates(m_Cursor->GetCurrentObject()->GetMapCoordinates());
		//Release current manifest
		m_PathFinder.ReleaseManifest();
		//Change State
		m_State = MODE_STATE::UNIT_MENU_OPEN;
		break;

	case 'f':
	case 'F':
		//If current highlighted tile is in the grid
		if (m_PathFinder.IsTileInGrid(static_cast<MapTile*>(m_Cursor->GetCurrentTileObject())))
		{
			//Flag that current tile is no longer occupied
			MapTile::FindTileInArray(m_MapGeneration.GetFinalMap(), m_Cursor->GetCurrentObject()->GetMapCoordinates(),
				m_MapGeneration.GetMapLimit().x)->GetTileProperties().occupied = false;

			//Move unit
			static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->MoveToCoordinate(
				m_Cursor->GetCurrentTileObject()->GetMapCoordinates());

			//Flag current tile as occupied
			MapTile::FindTileInArray(m_MapGeneration.GetFinalMap(), m_Cursor->GetCurrentObject()->GetMapCoordinates(),
				m_MapGeneration.GetMapLimit().x)->GetTileProperties().occupied = true;

			//Release current manifest
			m_PathFinder.ReleaseManifest();

			//Flag the unit as unable to move, then decide on what to do based on flags
			static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->SetMoveState(false);
			//If the unit has no remaining actions
			if (static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->CheckRemainingActions() == false)
			{
				static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->DisableUnit();
				m_State = MODE_STATE::FREE_NAVIGATION;
				CheckIfTeamActionsSpent();
			}
			else //Revert to unit menu
			{
				//Change state back to Unit Menu
				manager->GetNavigationMenuByTypeID(m_UIElement1, UIElementIDs::UNIT_MENU_01);
				m_State = MODE_STATE::UNIT_MENU_OPEN;
			}
		}
		break;

	case 'w':
	case 'W':
		m_Cursor->DecrementYCoordinate();
		m_Camera.MoveSceneDown();
		if (m_Cursor->SearchForTileObject(m_MapGeneration.GetFinalMap(), m_MapGeneration.GetMapLimit().x))
		{
			UpdateTileTooltip(m_TileTooltip, m_Cursor->GetCurrentTileObject());
		}
		break;

	case 's':
	case 'S':
		m_Cursor->IncrementYCoordinate();
		m_Camera.MoveSceneUp();
		if (m_Cursor->SearchForTileObject(m_MapGeneration.GetFinalMap(), m_MapGeneration.GetMapLimit().x))
		{
			UpdateTileTooltip(m_TileTooltip, m_Cursor->GetCurrentTileObject());
		}
		break;

	case 'a':
	case 'A':
		m_Cursor->DecrementXCoordinate();
		m_Camera.MoveSceneRight();
		if (m_Cursor->SearchForTileObject(m_MapGeneration.GetFinalMap(), m_MapGeneration.GetMapLimit().x))
		{
			UpdateTileTooltip(m_TileTooltip, m_Cursor->GetCurrentTileObject());
		}
		break;

	case 'd':
	case 'D':
		m_Cursor->IncrementXCoordinate();
		m_Camera.MoveSceneLeft();
		if (m_Cursor->SearchForTileObject(m_MapGeneration.GetFinalMap(), m_MapGeneration.GetMapLimit().x))
		{
			UpdateTileTooltip(m_TileTooltip, m_Cursor->GetCurrentTileObject());
		}
		break;

	}
}

void A_PlaceholderMode::ProcessKeyFindAttackTargetState(char key)
{
	UIElementManager* manager = &Game::GetGame()->GetUIManager();

	switch (key)
	{
	case VK_ESCAPE:
		//Open Unit menu
		manager->GetNavigationMenuByTypeID(m_UIElement1, UIElementIDs::UNIT_MENU_01);
		//Snap cursor & camera back to the unit
		m_Cursor->MoveFixedCursorCoords(m_Cursor->GetCurrentObject()->GetMapCoordinates());
		m_Camera.MoveCameraToCoordinates(m_Cursor->GetCurrentObject()->GetMapCoordinates());
		//Release current manifest
		m_PathFinder.ReleaseManifest();

		//Erase Effect
		FlagTilesBasic(false);

		//Change State
		m_State = MODE_STATE::UNIT_MENU_OPEN;
		break;

	case 'f':
	case 'F':
		//Check if the cursor is currently holding a unit
		if (m_Cursor->GetSecondObject() && static_cast<UnitEntity*>(m_Cursor->GetSecondObject())->GetUnitTeamID() != m_CurrentTeamID)
		{
			//Do attack
			Game::GetGame()->GetGameplayManager().BasicAttack(static_cast<UnitEntity*>(m_Cursor->GetCurrentObject()), 
				static_cast<UnitEntity*>(m_Cursor->GetSecondObject()));
			
			//Erase Effect
			FlagTilesBasic(false);

			//Flag the unit as unable to move
			static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->SetActState(false);
			//If the unit has no remaining actions
			if (static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->CheckRemainingActions() == false)
			{
				static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->DisableUnit();

				//Post attack clean up to get the UI aligned with the free navigation state
				UpdateCursorFreeMode();
				m_State = MODE_STATE::FREE_NAVIGATION;
				CheckIfTeamActionsSpent();
			}
			else
			{
				//Post attack clean up to get the UI aligned with the free navigation state
				UpdateCursorFreeMode();
				//Change state
				m_State = MODE_STATE::FREE_NAVIGATION;
			}
		}

		break;
	case 'w':
	case 'W':
		m_Cursor->DecrementYCoordinate();
		m_Camera.MoveSceneDown();
		UpdateCursorTargetingMode();

		break;
	case 's':
	case 'S':
		m_Cursor->IncrementYCoordinate();
		m_Camera.MoveSceneUp();
		UpdateCursorTargetingMode();

		break;
	case 'a':
	case 'A':
		m_Cursor->DecrementXCoordinate();
		m_Camera.MoveSceneRight();
		UpdateCursorTargetingMode();

		break;
	case 'd':
	case 'D':
		m_Cursor->IncrementXCoordinate();
		m_Camera.MoveSceneLeft();
		UpdateCursorTargetingMode();

		break;
	}
}

void A_PlaceholderMode::ProcessKeyFindHealTargetState(char key)
{
	UIElementManager* manager = &Game::GetGame()->GetUIManager();

	switch (key)
	{
	case VK_ESCAPE:
		//Open Unit menu
		manager->GetNavigationMenuByTypeID(m_UIElement1, UIElementIDs::UNIT_MENU_01);
		//Snap cursor & camera back to the unit
		m_Cursor->MoveFixedCursorCoords(m_Cursor->GetCurrentObject()->GetMapCoordinates());
		m_Camera.MoveCameraToCoordinates(m_Cursor->GetCurrentObject()->GetMapCoordinates());
		//Release current manifest
		m_PathFinder.ReleaseManifest();
		//Change State
		m_State = MODE_STATE::UNIT_MENU_OPEN;
		break;

	case 'f':
	case 'F':
		//Check if the cursor is currently holding a unit
		if (m_Cursor->GetSecondObject() && static_cast<UnitEntity*>(m_Cursor->GetSecondObject())->GetUnitTeamID() == m_CurrentTeamID)
		{
			//Do heal
			Game::GetGame()->GetGameplayManager().BasicHeal(static_cast<UnitEntity*>(m_Cursor->GetCurrentObject()),
				static_cast<UnitEntity*>(m_Cursor->GetSecondObject()));

			//Flag the unit as unable to move, then decide on what to do based on flags
			static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->SetActState(false);
			//If the unit has no remaining actions
			if (static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->CheckRemainingActions() == false)
			{
				static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->DisableUnit();

				//Post heal clean up to get the UI aligned with the free navigation state
				UpdateCursorFreeMode();
				m_State = MODE_STATE::FREE_NAVIGATION;
				CheckIfTeamActionsSpent();
			}
			else
			{
				//Post heal clean up to get the UI aligned with the free navigation state
				UpdateCursorFreeMode();
				//Change state
				m_State = MODE_STATE::FREE_NAVIGATION;
			}
		}
		break;
	case 'w':
	case 'W':
		m_Cursor->DecrementYCoordinate();
		m_Camera.MoveSceneDown();
		UpdateCursorTargetingMode();

		break;
	case 's':
	case 'S':
		m_Cursor->IncrementYCoordinate();
		m_Camera.MoveSceneUp();
		UpdateCursorTargetingMode();

		break;
	case 'a':
	case 'A':
		m_Cursor->DecrementXCoordinate();
		m_Camera.MoveSceneRight();
		UpdateCursorTargetingMode();

		break;
	case 'd':
	case 'D':
		m_Cursor->IncrementXCoordinate();
		m_Camera.MoveSceneLeft();
		UpdateCursorTargetingMode();

		break;
	}
}

void A_PlaceholderMode::RenderBackground(PassData& data, ID3D12GraphicsCommandList* cl)
{
	//Begin Draw
	data.sceneSB->Begin(cl);


	//Render BGs
	for (auto& a : m_Backgrounds)
		a->Render(*data.sceneSB, data.heap);


	//End Draw
	data.sceneSB->End();
}

void A_PlaceholderMode::RenderScene(PassData& data, ID3D12GraphicsCommandList* cl)
{
	//Begin Draw
	data.sceneSB->Begin(cl, SpriteSortMode_Deferred, m_Camera.GetSceneTransform());


	//Render Terrain
	for (auto& a : m_MapGeneration.GetFinalMap())
		a->Render(*data.sceneSB, data.heap);

	//Render Actors
	for (auto& a : m_Actors)
		a->Render(*data.sceneSB, data.heap);

	//Render Projectiles
	for (auto& a : m_Projectiles)
		a->Render(*data.sceneSB, data.heap);


	//End Draw
	data.sceneSB->End();
}

void A_PlaceholderMode::RenderUI(PassData& data, ID3D12GraphicsCommandList* cl)
{
	//Begin Draw
	data.uiSB->Begin(cl);


	//Draw cursor
	m_Cursor->Render(*data.uiSB, data.heap);

	//Menu Rendering
	switch (m_State)
	{
	case MODE_STATE::FREE_NAVIGATION:
		if (m_FriendlyUnitUI)
		{
			m_FriendlyUnitUI->RenderUI(*data.uiSB, data.heap);
			m_FriendlyUnitUI->RenderUIText(data.uiSB.get(), data.secondarySF.get());
		}
		if (m_EnemyUnitUI)
		{
			m_EnemyUnitUI->RenderUI(*data.uiSB, data.heap);
			m_EnemyUnitUI->RenderUIText(data.uiSB.get(), data.secondarySF.get());
		}
		m_TileTooltip->RenderUI(*data.uiSB, data.heap);
		m_TileTooltip->RenderUIText(data.uiSB.get(), data.secondarySF.get());
		break;

	case MODE_STATE::UNIT_MENU_OPEN:
		m_UIElement1->Render(*data.uiSB, data.heap);
		m_UIElement1->RenderMenuItems(data.uiSB.get(), data.secondarySF.get());
		m_TileTooltip->RenderUI(*data.uiSB, data.heap);
		m_TileTooltip->RenderUIText(data.uiSB.get(), data.secondarySF.get());
		break;

	case MODE_STATE::ACTION_MENU_OPEN:
		m_UIElement1->Render(*data.uiSB, data.heap);
		m_UIElement1->RenderMenuItems(data.uiSB.get(), data.secondarySF.get());
		if (m_SkillTooltip)
		{
			m_SkillTooltip->RenderUI(*data.uiSB, data.heap);
			m_SkillTooltip->RenderUIText(data.uiSB.get(), data.secondarySF.get());
		}
		break;

	case MODE_STATE::MAGIC_MENU_OPEN:
		m_UIElement1->Render(*data.uiSB, data.heap);
		m_UIElement1->RenderMenuItems(data.uiSB.get(), data.secondarySF.get());
		break;

	case MODE_STATE::FIND_BASIC_ATTACK_TARGET:
	case MODE_STATE::FIND_HEAL_TARGET:
		m_TextFrame1->RenderUI(*data.uiSB, data.heap);
		m_TextFrame1->RenderUIText(data.uiSB.get(), data.secondarySF.get());
		if (m_FriendlyUnitUI)
		{
			m_FriendlyUnitUI->RenderUI(*data.uiSB, data.heap);
			m_FriendlyUnitUI->RenderUIText(data.uiSB.get(), data.secondarySF.get());
		}
		if (m_EnemyUnitUI)
		{
			m_EnemyUnitUI->RenderUI(*data.uiSB, data.heap);
			m_EnemyUnitUI->RenderUIText(data.uiSB.get(), data.secondarySF.get());
		}
		break;
	}


	//End Draw
	data.uiSB->End();
}

void A_PlaceholderMode::UpdateCursorFreeMode()
{
	//Grab the UI manager for use
	UIElementManager* manager = &Game::GetGame()->GetUIManager();

	//First, find the underlying tile
	if (m_Cursor->SearchForTileObject(m_MapGeneration.GetFinalMap(), m_MapGeneration.GetMapLimit().x))
		UpdateTileTooltip(m_TileTooltip, m_Cursor->GetCurrentTileObject());

	//Next, check to see if there is a unit occupying the tile, and of what type it ise
	if (m_Cursor->SearchForUnitObject(m_Actors, m_MapGeneration.GetMapLimit().x))
	{

		if (static_cast<UnitEntity*>(m_Cursor->GetCurrentObject())->GetUnitTeamID() == m_CurrentTeamID)
		{
			//Get the element & update the tooltip
			manager->GetNonNavigationUIByTypeID(m_FriendlyUnitUI, UIElementIDs::FRIENDLY_UNIT_FRAME_01);
			UpdateUnitTooltip(m_FriendlyUnitUI, static_cast<UnitEntity*>(m_Cursor->GetCurrentObject()));

			//Release the other menu if it was active (edge case where moving from friendly to enemy doesn't clear ui)
			manager->ReleaseNonNavUIElement(m_EnemyUnitUI);
		}
		else
		{
			//Get the element & update the tooltip
			manager->GetNonNavigationUIByTypeID(m_EnemyUnitUI, UIElementIDs::ENEMY_UNIT_FRAME_01);
			UpdateUnitTooltip(m_EnemyUnitUI, static_cast<UnitEntity*>(m_Cursor->GetCurrentObject()));

			//Release the other menu if it was active (edge case where moving from enemy to friendly doesn't clear ui)
			manager->ReleaseNonNavUIElement(m_FriendlyUnitUI);
		}

	}
	//No match unit found so moved off a unit, so clear the pointers
	else
	{
		manager->ReleaseNonNavUIElement(m_FriendlyUnitUI);
		manager->ReleaseNonNavUIElement(m_EnemyUnitUI);
	}


}

void A_PlaceholderMode::UpdateCursorTargetingMode()
{
	//Grab the UI manager for use
	UIElementManager* manager = &Game::GetGame()->GetUIManager();

	//Updates the cursor, looking only for a second unit
	if (m_Cursor->SearchForSecondUnitObject(m_Actors, m_MapGeneration.GetMapLimit().x))
	{

		//Friendly unit
		if (static_cast<UnitEntity*>(m_Cursor->GetSecondObject())->GetUnitTeamID() == m_CurrentTeamID)
		{

			//Get the element & update the tooltip
			manager->GetNonNavigationUIByTypeID(m_FriendlyUnitUI, UIElementIDs::FRIENDLY_UNIT_FRAME_01);
			UpdateUnitTooltip(m_FriendlyUnitUI, static_cast<UnitEntity*>(m_Cursor->GetSecondObject()));

			//Release the other menu if it was active (edge case where moving from friendly to enemy doesn't clear ui)
			manager->ReleaseNonNavUIElement(m_EnemyUnitUI);
		}
		else
		{
			//Get the element & update the tooltip
			manager->GetNonNavigationUIByTypeID(m_EnemyUnitUI, UIElementIDs::ENEMY_UNIT_FRAME_01);
			UpdateUnitTooltip(m_EnemyUnitUI, static_cast<UnitEntity*>(m_Cursor->GetSecondObject()));

			//Release the other menu if it was active (edge case where moving from enemy to friendly doesn't clear ui)
			manager->ReleaseNonNavUIElement(m_FriendlyUnitUI);
		}
	}
	//No match unit found so might of moved off a unit, so clear the UI
	else
	{
		manager->ReleaseNonNavUIElement(m_FriendlyUnitUI);
		manager->ReleaseNonNavUIElement(m_EnemyUnitUI);
	}
}

void A_PlaceholderMode::UpdateTileTooltip(NonNavigationUI* ui, MapTile* tile)
{
	//Reset existing strings
	ui->ResetDrawStrings();
	ui->ChangeStringByIndex(std::string("Tile"), 0);
	std::stringstream ss;
	ss << tile->GetTileProperties().moveCost;
	ui->AppendToStringByIndex(ss, 1);
}

void A_PlaceholderMode::UpdateUnitTooltip(NonNavigationUI* ui, UnitEntity* unit)
{
	//Reset existing strings
	ui->ResetDrawStrings();
	ui->AppendToStringByIndex(unit->GetUnitClass(), 0);
	std::stringstream ss;
	ss << unit->GetClassTotals().CurrentHP << "/" << unit->GetClassTotals().MaxHP;
	ui->AppendToStringByIndex(ss, 1);
	std::stringstream().swap(ss);
	ss << unit->GetClassTotals().CurrentMP << "/" << unit->GetClassTotals().MaxMP;
	ui->AppendToStringByIndex(ss, 2);
}

void A_PlaceholderMode::FlagTilesBasic(bool enable)
{
	if (enable)
	{
		for (int i(0); i < 4; ++i)
		{
			m_Cursor->GetCurrentTileObject()->GetNeighbourAtIndex(i)->SetDrawGridFlag(true);
			m_Cursor->GetCurrentTileObject()->GetNeighbourAtIndex(i)->GetGridSprite().SetFrame(19);
		}
	}
	else
	{
		for (int i(0); i < 4; ++i)
		{
			m_Cursor->GetCurrentTileObject()->GetNeighbourAtIndex(i)->SetDrawGridFlag(false);
		}
	}

}

void A_PlaceholderMode::ChangeTeamID()
{
	if (m_CurrentTeamID == 1)
		m_CurrentTeamID = 2;
	else
		m_CurrentTeamID = 1;
}



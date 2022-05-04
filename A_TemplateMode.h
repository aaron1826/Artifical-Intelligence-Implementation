#pragma once

#include "ModeInterface.h"			//Parent
#include "Game.h"					//Main Game Object

#include "2DCameraTypes.h"			//Scene Management
#include "MapTilePathfinding.h"		//Pathfinding algorithm for grid
#include "MapGeneticAlgorithm.h"
//Forward Dec
class CursorEntity;

class A_PlaceholderMode : public ModeInterface
{
public:

	enum class MODE_STATE
	{
		//Camera Move States
		FREE_NAVIGATION,
		MOVE_MENU_OPEN,
		FIND_BASIC_ATTACK_TARGET,
		FIND_HEAL_TARGET,
		//Menu Related States
		UNIT_MENU_OPEN,
		ACTION_MENU_OPEN,
		ATTACK_MENU_OPEN,
		MAGIC_MENU_OPEN,
		EQUIPMENT_MENU_OPEN,
		//Combat States
		PHYS_ATTACK_STATE,
		MAG_ATTACK_STATE
	};


	////////////////////
	/// Constructors ///
	////////////////////

	A_PlaceholderMode(size_t id);
	~A_PlaceholderMode();


	/////////////////
	/// Overrides ///
	/////////////////

	void Enter() override;
	bool Exit() override { return true; }
	void Update(const GameTimer& gt) override;
	void Render(PassData& data) override;
	void ProcessKey(char key) override;
	void Release();


private:

	/////////////
	/// Setup ///
	/////////////

	//Setup object containers
	void InitObjectContainers();
	//Setup UI & functionality elements
	void InitUIFunctionality();
	//Setup the map
	void InitMapTiles();
	//Clears loose ends (one-offs and edge case preventions)
	void FinaliseInit();


	 /////////////////
	// Audio Setup //
   /////////////////

	void InitModeAudio();


	//////////////
	/// States ///
	//////////////

	//Change the state BEFORE doing any one-off state operations
	void OnStateEnter(MODE_STATE newState);
	//Change the state AFTER doing any one-off state operations
	void OnStateExit(MODE_STATE newState);

	//Call after disabling unit to check if all the units of current team are disabled
	//if yes, then flips the turn over
	void CheckIfTeamActionsSpent();
	bool m_SwitchTeams = false;

	//Manage the team swap
	void SwitchTeams();


	//Menu States
	void ProcessKeyUnitMenuOpenState(char key);
	void ProcessKeyUnitActionOpenState(char key);
	void ProcessKeyMagicMenuOpenState(char key);
	void ProcessKeyMoveMenuOpenState(char key);

	//Camera Move States
	void ProcessKeyFreeMoveState(char key);
	void ProcessKeyFindAttackTargetState(char key);
	void ProcessKeyFindHealTargetState(char key);


	/////////////////
	/// Rendering ///
	/////////////////
	
	void RenderBackground(PassData& data, ID3D12GraphicsCommandList* cl);
	void RenderScene(PassData& data, ID3D12GraphicsCommandList* cl);
	void RenderUI(PassData& data, ID3D12GraphicsCommandList* cl);

	////////////
	/// Data ///
	////////////

	//Hold on to the tile dims for setups
	//DirectX::XMINT2 m_TileDims = { 0, 0 };
	//DirectX::XMINT2 m_MapLimit = { 0, 0 };
	MODE_STATE m_State;

	/////////////////////////
	/// Object Containers ///
	/////////////////////////

	//Background
	std::vector<EntityInterface*> m_Backgrounds;
	//Scene Actors (Player, Enemies etc)
	std::vector<EntityInterface*> m_Actors;
	//Scene Projectiles
	std::vector<EntityInterface*> m_Projectiles;
	
	//Scene Terrain Objects(Tiles)
	std::vector<MapTile*> m_TileMap;

	//Hold the teams in different containers
	std::vector<UnitEntity*> m_TeamOne;
	std::vector<UnitEntity*> m_TeamTwo;

	/////////////////////
	/// Functionality ///
	/////////////////////

	//Game Object that manages pathfinding in the context of a grid
	MapTilePathfinder m_PathFinder;
	//Manages the matrix for shifting the scene around, producing a camera effect
	FixedDist2DCamera m_Camera;


	//////////
	/// UI ///
	//////////

	//Wrap all cursor check behaviour in one place (general cursor movement)
	void UpdateCursorFreeMode();
	//Updates cursor, keeping the unit on if first pointer, looking for 2nd targets
	void UpdateCursorTargetingMode();

	void UpdateTileTooltip(NonNavigationUI* ui, MapTile* tile);
	void UpdateUnitTooltip(NonNavigationUI* ui, UnitEntity* unit);

	void FlagTilesBasic(bool enable);

	//Holding pointers for menus
	NavigationalMenu* m_UIElement1 = nullptr;
	NavigationalMenu* m_UIElement2 = nullptr;
	NavigationalMenu* m_UIElement3 = nullptr;
	NavigationalMenu* m_UIElement4 = nullptr;
	NonNavigationUI* m_TileTooltip = nullptr;
	NonNavigationUI* m_FriendlyUnitUI = nullptr;
	NonNavigationUI* m_EnemyUnitUI = nullptr;
	NonNavigationUI* m_TextFrame1 = nullptr;
	NonNavigationUI* m_SkillTooltip = nullptr;

	//Cursor
	CursorEntity* m_Cursor = nullptr;


	////////////
	/// Team ///
	////////////

	void ChangeTeamID();

	int m_CurrentTeamID = -1;

	//////////////////////////////////////
	/// Temporary Testing Data/Objects ///
	//////////////////////////////////////

	//Temporary starting coordinates for cursor
	DirectX::XMINT2 m_MapStartingCoords = { 25, 25 };
	
	MapGeneticAlgorithm m_MapGeneration;
};
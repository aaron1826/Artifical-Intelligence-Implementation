#pragma once
#include "D3D.h"               //Parent Class/Core Application (Gets Utils through here)

#include "D3DUtils.h"

//Manager Classes
#include "Input.h"             //Mouse/Keyboard/Gamepad input manager
#include "ModeManager.h"       //Game Mode/State manager
#include "AssetManager.h"      //Store data
#include "AudioManager.h"      //Audio setup/loading/playing manager
#include "GameplayManager.h"
//Testing
#include "UIElementManager.h"  //Houses specialist UI Elements

using Microsoft::WRL::ComPtr;

/*
    Any texture that needs to be loaded should be added here.
    Manages the SRV Heap offset when used in the texture loading process.
*/
enum DescriptorIDs
{
    //Sprites
    MAIN_GAME_BG_00,
    TEST_TILEMAP_00,
    ASSASSIN_CLASS_SS_00,
    BARBARIAN_CLASS_SS_00,
    CENTAUR_CLASS_SS_00,
    NECROMANCER_CLASS_SS_00,
    SORCERESS_CLASS_SS_00,
    WARRIOR_CLASS_SS_00,    
    //Backgrounds
    START_MENU_BG_00,
    MAIN_MENU_BG_00,
    //UI
    UI_ATLAS_01,
    //Font
    SF_PH_01,
    SF_ALLOYINK_20,
    //Total Count
    RESOURCE_COUNT
};
/*
    Put any game modes/states here. Don't add more than is intended for use, 
    and don't use one more than once (mess's with the Manager)
*/
enum ModeNames
{
    START_SCREEN,
    MAIN_MENU,
    MAIN_GAME_MODE,
    MODE_COUNT
};


class Game : public D3D12Application
{
public:

    //Windows Creation (Calls D3D12Application)
    Game(HINSTANCE hInstance);
    Game(const Game& rSide) = delete;
    Game& operator=(const Game& rSide) = delete;
    ~Game();

    bool Initialize() override;


    ///////////
    /// Get ///
    ///////////

    static Game* GetGame() { return m_Game; }

    AssetManager& GetAssetManager() { return m_AssetManager; }
    ModeManager& GetModeManager() { return m_ModeManager; }
    Gamepads& GetControllerManager() { return m_Gamepad; }
    KeyboardAndMouse& GetKBMManager() { return m_KBM; }
    UIElementManager& GetUIManager() { return m_UIManager; }
    AudioManager& GetAudioManager() { return m_AudioManager; }
    GameplayManager& GetGameplayManager() { return m_GameplayManager; }
private:

    static Game* m_Game;

    /////////////////
    /// Overrides ///
    /////////////////

    void Update(const GameTimer& gt) override;
    void Render(const GameTimer& gt) override;

    //Need to intercept some message for InputManager, so we override MessageHandler from parent change it slightly
    LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /////////////////
    /// Rendering ///
    /////////////////

    void PreFrame();
    void PostFrame();
    void CycleFrame();

    /////////////
    /// Setup ///
    /////////////

    void XTKSetup();
    void LoadTextures(DirectX::ResourceUploadBatch& resourceUpload);
    void BuildFrameResources();

    //Load whatever modes you need front loaded here (start screen, main menu, splashes etc)
    void LoadInitialModes();

    ////////////////
    /// Managers ///
    ////////////////

    //Loading, storing, accessing data
    AssetManager m_AssetManager;
    //Mode Manager (Each gamestate goes through this)
    ModeManager m_ModeManager;
    //Gamepad Controls
    Gamepads m_Gamepad;
    //M&K Controls
    KeyboardAndMouse m_KBM;
    //UI
    UIElementManager m_UIManager;
    //Audio
    AudioManager m_AudioManager;
    //Gameplay
    GameplayManager m_GameplayManager;

    ////////////////////
    /// DX Variables ///
    ////////////////////

    //Package together the data modes need into one spot (spritebatchs, fonts etc)
    PassData m_PassData;

    int m_CurrentFrameResourceIndex = 0;
    FrameResource2D* m_CurrentFrameResource2D = nullptr;
    std::vector<std::unique_ptr<FrameResource2D>> m_FrameResources2D;
    
    std::unique_ptr<DirectX::GraphicsMemory> m_GraphicsMemory;
    std::unique_ptr<DirectX::DescriptorHeap> m_ResourceDescriptors;

};
#include "MainMenuMode.h"

MainMenuMode::MainMenuMode(size_t id)
	:ModeInterface(id)
{
	m_Background.SetTexturePtr(Game::GetGame()->GetAssetManager().GetSpriteTextureData(std::string("Main_Menu_BG_00")));
}

MainMenuMode::~MainMenuMode()
{
}

void MainMenuMode::Update(const GameTimer& gt)
{
}

void MainMenuMode::Render(PassData& data)
{
	//Get Command list for draws
	auto commandList = Game::GetGame()->GetCommandList().Get();

	//Begin Scene Draw
	data.sceneSB->Begin(commandList);

	m_Background.Draw(*data.sceneSB, data.heap);

	//End Scene Draw
	data.sceneSB->End();
}

void MainMenuMode::ProcessKey(char key)
{
	switch (key)
	{
	case(VK_SPACE):
		Game::GetGame()->GetModeManager().SwitchMode(ModeNames::MAIN_GAME_MODE);
		break;
	}
}

void MainMenuMode::Release()
{
}

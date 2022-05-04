#pragma once

#include "ModeInterface.h"			//Parent
#include "Game.h"					//Main Game Object

#include "Sprite.h"

class MainMenuMode : public ModeInterface
{
public:

	MainMenuMode(size_t id);
	~MainMenuMode();

	/////////////////
	/// Overrides ///
	/////////////////

	void Enter() override { }
	bool Exit() override { return true; }
	void Update(const GameTimer& gt) override;
	void Render(PassData& data) override;
	void ProcessKey(char key) override;
	void Release() override;

private:

	//Main Background
	Sprite m_Background;

};
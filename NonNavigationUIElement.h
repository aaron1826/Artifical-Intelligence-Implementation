#pragma once

#include "EntityInterface.h"

class NonNavigationUI
{
public:

	NonNavigationUI() {}
	~NonNavigationUI() { Release(); }

	std::vector<std::string>& GetTextStrings() { return m_TextStrings; }
	std::vector<DirectX::XMFLOAT2>& GetTextPositions() { return m_TextPositions; }

	///////////
	/// Get ///
	///////////

	Sprite& GetUISprite() { return m_UISprite; }

	int GetID() { return m_ID; }
	bool GetUsageState() { return m_FreeForUse; }

	///////////
	/// Set ///
	///////////

	void SetID(int id) { m_ID = id; }
	void SetUsageState(bool free) { m_FreeForUse = free; }


	//////////////////
	/// Operations ///
	//////////////////

	void RenderUI(DirectX::SpriteBatch& sb, DirectX::DescriptorHeap* heap) { m_UISprite.Draw(sb, heap); }
	void RenderUIText(DirectX::SpriteBatch* sb, DirectX::SpriteFont* sf);

	//Resets draw string to loaded defaults
	void ResetDrawStrings();
	void ChangeStringByIndex(std::string& msg, unsigned int index);
	void AppendToStringByIndex(std::string& msg, unsigned int index);
	void AppendToStringByIndex(std::stringstream& msg, unsigned int index);

private:

	void Release();

	//Main UI Sprite
	Sprite m_UISprite;

	//Holds the Original Test Strings
	std::vector<std::string> m_TextStrings;
	//Holds the modifiable strings that are drawn
	std::vector<std::string> m_DrawStrings;
	//Holds the UI text positions
	std::vector<DirectX::XMFLOAT2> m_TextPositions;

	//An ID describing what this item is (this has meaning externally)
	int m_ID = -1;
	//Track if this object is in use in another mode
	bool m_FreeForUse = true;
};

#include "NonNavigationUIElement.h"

using namespace DirectX;

void NonNavigationUI::RenderUIText(DirectX::SpriteBatch* sb, DirectX::SpriteFont* sf)
{
	//Setup a normalised origin
	SimpleMath::Vector2 origin = sf->MeasureString(m_TextStrings[0].c_str()) * 0.5f;
	origin.x = 0;
	for (int i(0); i < m_DrawStrings.size(); ++i)
	{
		sf->DrawString(sb, m_DrawStrings[i].c_str(), m_TextPositions[i], Colors::Black, 0.f, origin, 1.f);
	}
}

void NonNavigationUI::ResetDrawStrings()
{
	m_DrawStrings = m_TextStrings;
}

void NonNavigationUI::ChangeStringByIndex(std::string& msg, unsigned int index)
{
	m_DrawStrings[index] = msg;
}

void NonNavigationUI::AppendToStringByIndex(std::string& msg, unsigned int index)
{
	m_DrawStrings[index].append(msg);
}

void NonNavigationUI::AppendToStringByIndex(std::stringstream& msg, unsigned int index)
{
	m_DrawStrings[index].append(msg.str());
}

void NonNavigationUI::Release()
{
	m_TextPositions.clear();
	m_TextStrings.clear();
}

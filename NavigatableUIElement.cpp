#include "NavigatableUIElement.h"

using namespace DirectX;

NavigationalMenu::NavigationalMenu(std::shared_ptr<SpriteTexture> tex, int id)
	:EntityInterface(tex), m_ID(id)
{
	m_CursorHighlight.SetTexturePtr(tex);
}

NavigationalMenu::NavigationalMenu(std::shared_ptr<SpriteTexture> menuTex, std::shared_ptr<SpriteTexture> cursorTex)
	:EntityInterface(menuTex)
{
	m_CursorHighlight.SetTexturePtr(cursorTex);
}

void NavigationalMenu::Render(DirectX::SpriteBatch& batch, DirectX::DescriptorHeap* heap)
{
	GetPrimarySprite().Draw(batch, heap);
	m_CursorHighlight.Draw(batch, heap);
}

void NavigationalMenu::ResetCursor()
{
	m_ActiveIndex = m_IndexDefault;
	m_CursorHighlight.SetPosition(m_CursorAnchorPos.x * m_ActiveIndex.x, m_CursorAnchorPos.y * m_ActiveIndex.y);

}
void NavigationalMenu::MoveUp()
{
	//dec index
	--m_ActiveIndex.y;
	//Check if out of range
	if (m_ActiveIndex.y < 0)
		m_ActiveIndex.y = 0;

	//Set position
	m_CursorHighlight.SetPosition( m_CursorAnchorPos.x + (m_Offsets.x * m_ActiveIndex.x), 
		m_CursorAnchorPos.y + (m_Offsets.y * m_ActiveIndex.y));
}

void NavigationalMenu::MoveDown()
{
	//dec index
	++m_ActiveIndex.y;
	//Check if out of range
	if (m_ActiveIndex.y > m_IndexMax.y)
		m_ActiveIndex.y = m_IndexMax.y;

	//Set position
	m_CursorHighlight.SetPosition(m_CursorAnchorPos.x + (m_Offsets.x * m_ActiveIndex.x),
		m_CursorAnchorPos.y + (m_Offsets.y * m_ActiveIndex.y));
}

void NavigationalMenu::MoveLeft()
{
	//dec index
	--m_ActiveIndex.x;
	//Check if out of range
	if (m_ActiveIndex.x < 0)
		m_ActiveIndex.x = 0;

	//Set position
	m_CursorHighlight.SetPosition(m_CursorAnchorPos.x + (m_Offsets.x * m_ActiveIndex.x),
		m_CursorAnchorPos.y + (m_Offsets.y * m_ActiveIndex.y));
}

void NavigationalMenu::MoveRight()
{
	//dec index
	++m_ActiveIndex.x;
	//Check if out of range
	if (m_ActiveIndex.x > m_IndexMax.x)
		m_ActiveIndex.x = m_IndexMax.x;

	//Set position
	m_CursorHighlight.SetPosition(m_CursorAnchorPos.x + (m_Offsets.x * m_ActiveIndex.x),
		m_CursorAnchorPos.y + (m_Offsets.y * m_ActiveIndex.y));
}

void NavigationalMenu::RenderMenuItems(DirectX::SpriteBatch* sb, DirectX::SpriteFont* sf)
{
	int count = 0;
	//Setup a normalised origin
	SimpleMath::Vector2 origin = sf->MeasureString(m_MenuStrings[0].c_str()) * 0.5f;
	origin.y = 0;
	XMVECTOR pos = { 0, 0 };
	for (int x(0); x < m_IndexMax.x + 1; ++x)
	{
		for (int y(0); y < m_IndexMax.y + 1; ++y)
		{
			pos = {m_TextAnchorPos.x + (m_Offsets.x * x), m_TextAnchorPos.y + (m_Offsets.y * y) };
			sf->DrawString(sb, m_MenuStrings[count].c_str(), pos, Colors::MintCream, 0.f, origin, 1.f);
			++count;
		}
	}
}

void NavigationalMenu::Release()
{
	m_MenuStrings.clear();
}

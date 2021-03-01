#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <cmath>

#include "menu.h"

using namespace hiage;

UIItem::UIItem() : _initialized(false)
{
}

//TODO: fix font
/*bool UIItem::Create(const char * pcLabel, COutlineFont * pFont, CTexture * pBackground)
{
	m_sLabel = pcLabel;

	if (!pBackground)
	{
		return false;
	}

	if (!pFont)
	{
		return false;
	}

	m_pFont = pFont;
	background = pBackground;

	initialized = true;

	return true;
}
*/
void UIItem::render(float x, float y, float z, float width, float height)
{
	if (!_initialized)
	{
		return;
	}

	_background->select();

	glPushMatrix();

	glTranslatef(x,y,z);

	//draw background
	glBegin(GL_QUADS);

	//upper left
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	//upper right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(width, 0.0f, 0.0f);

	//lower right
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(width, -height, 0.0f);

	//lower left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -height, 0.0f);

	glEnd();

	glPopMatrix();

	//TODO: fix font
	//m_pFont->Print(x+10, -(y-(height / 4)), -0.1f, 1.0f, 1.0f, 1.0f, 1.0f, m_sLabel.c_str());
}

UIMenu::UIMenu() : _currentItem(1),_initialized(false)
{
}

//TODO: fix font
//initialize the menu by setting the title and dimensions
/*bool UIMenu::Create(const char * pcTitle, float x, float y, float width, float fItemHeight,
					 int nMaxItemsShow, COutlineFont * pFontTitle, CTexture * pBackground, CTexture * pCursor)
{
	this->title		= title;
	this->width		= width;
	this->itemHeight	= fItemHeight;
	maxItemsShow = nMaxItemsShow;
	m_fX			= x;
	m_fY			= y;
	cursor		= pCursor;
//	background	= pBackground;


	if (!pFontTitle)
	{
		return false;
	}

	if (!pFontTitle->IsLoaded())
	{
		return false;
	}

	m_Font = pFontTitle;

	initialized	= true;

	return true;
}*/

//TODO: Fix font
/*void UIMenu::AddItem(const char * pcLabel, COutlineFont * pFont, CTexture * pBackground)
{
	items.push_back(new UIItem);
	items.back()->Create(pcLabel, pFont, pBackground);
}*/

void UIMenu::render(float z)
{
	if ((!_initialized) || (_items.size() <= 0))
	{
		return;
	}

	glPushMatrix();

	glTranslatef(_x, _y, z);

	if (_items.size() < _maxItemsShow)
	{
		for (unsigned int i = 0; i < _items.size(); i++)
		{
			_items[i]->render(0.0f, _itemHeight * (float)i, 0.0f, _width, _itemHeight);
		}
		drawCursor(_currentItem);
	}
	else
	{
		uint start, end;

		start = (uint)floor((double)_currentItem - ((double)_maxItemsShow / 2));
		end = (uint)ceil((double)_currentItem + ((double)_maxItemsShow / 2));

		if (start < 0)
		{
			start = 0;
			end ++;
		}

		if (end > _items.size())
		{
			end = (unsigned int)_items.size();
		}

		for (unsigned int i = start; i < end; i++)
		{
			_items[i]->render(0.0f, _itemHeight * (float)i, 0.0f, _width, _itemHeight);
		}
		//draw cursor
		drawCursor(_currentItem - start);
	}

	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
}

void UIMenu::drawCursor(int index)
{
	if (!_cursor)
	{
		return;
	}

	//select the cursor texture
	_cursor->select();

	glPushMatrix();

	glTranslatef(-(float)_cursor->getWidth(), -index * _itemHeight + (_itemHeight / 4),-0.1f);

	//draw background
	glBegin(GL_QUADS);

	//upper left
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, (float)_cursor->getHeight(), 0.0f);

	//upper right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f((float)_cursor->getWidth(), (float)_cursor->getHeight(), 0.0f);

	//lower right
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f((float)_cursor->getWidth(), 0.0f, 0.0f);

	//lower left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPopMatrix();
}

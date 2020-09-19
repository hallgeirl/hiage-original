#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <math.h>

#include "ui_menu.h"

using namespace Hinage;

UIItem::UIItem() : initialized(false)
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
	if (!initialized)
	{
		return;
	}

	background->select();

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

UIMenu::UIMenu() : currentItem(1),initialized(false)
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
	if ((!initialized) || (items.size() <= 0))
	{
		return;
	}

	glPushMatrix();

	glTranslatef(x, y, z);

	if (items.size() < maxItemsShow)
	{
		for (unsigned int i = 0; i < items.size(); i++)
		{
			items[i]->render(0.0f, itemHeight * (float)i, 0.0f, width, itemHeight);
		}
		drawCursor(currentItem);
	}
	else
	{
		uint start, end;

		start = (uint)floor((double)currentItem - ((double)maxItemsShow / 2));
		end = (uint)ceil((double)currentItem + ((double)maxItemsShow / 2));

		if (start < 0)
		{
			start = 0;
			end ++;
		}

		if (end > items.size())
		{
			end = (unsigned int)items.size();
		}

		for (unsigned int i = start; i < end; i++)
		{
			items[i]->render(0.0f, itemHeight * (float)i, 0.0f, width, itemHeight);
		}
		//draw cursor
		drawCursor(currentItem - start);
	}

	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
}

void UIMenu::drawCursor(int index)
{
	if (!cursor)
	{
		return;
	}

	//select the cursor texture
	cursor->select();

	glPushMatrix();

	glTranslatef(-(float)cursor->getWidth(), -index * itemHeight + (itemHeight / 4),-0.1f);

	//draw background
	glBegin(GL_QUADS);

	//upper left
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, (float)cursor->getHeight(), 0.0f);

	//upper right
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f((float)cursor->getWidth(), (float)cursor->getHeight(), 0.0f);

	//lower right
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f((float)cursor->getWidth(), 0.0f, 0.0f);

	//lower left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPopMatrix();
}

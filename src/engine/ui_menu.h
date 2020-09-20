#pragma once

#include <string>
#include <vector>
#include "util_fonts.h"
#include "gfx_texture.h"
#include "com_typedefs.h"

namespace Hinage
{
	class UIItem
	{
	private:
		std::string label;
		//TODO: fix font
		//COutlineFont * m_pFont;
		Texture *	background;
		float		x,y,z;
		bool		initialized;

	public:
		UIItem();

		//TODO: fix font
		//bool Create(const char * pcLabel, COutlineFont * pFont, CTexture * pBackground);
		void render(float x, float y, float z, float width, float height);
		void setLabel(const char * label) {this->label = label;}
	};

	//a menu object
	class __IMPORTEXPORT UIMenu
	{
	private:
		//just some template class declarations
//		template class __IMPORTEXPORT std::allocator<CUIItem *>;
//		template class __IMPORTEXPORT std::vector<CUIItem *>;

	private:
		int						currentItem;	//currently selected item
		std::vector<UIItem *>	items;		//all items in the menu

		//dimensions
		float		width, itemHeight;

		unsigned int maxItemsShow;

		float x,y;

		std::string	title;
		bool		initialized;
		//TODO: Fix font
//		COutlineFont *		m_Font;
		Texture *	cursor;

		//CTexture

	private:
		void drawCursor(int index);

	public:
		UIMenu();

		//TODO: Fix font
		//bool Create(const char * pcTitle, float x, float y, float fWidth, float fItemHeight,
//			int nMaxItemsShow, COutlineFont * pFontTitle, CTexture * pBackground, CTexture * pCursor);
		//void AddItem(const char * pcLabel, COutlineFont * pFont, CTexture * pBackground);

		void render(float z);

		void setPosition(float x, float y) { this->x = x; this->y = y; }
	};
}

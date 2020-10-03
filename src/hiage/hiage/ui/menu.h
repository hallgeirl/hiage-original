#pragma once

#include <string>
#include <vector>
#include "../gfx/fonts.h"
#include "../gfx/texture.h"
#include "../core/typedefs.h"

namespace hiage
{
	class UIItem
	{
	private:
		std::string _label;
		//TODO: fix font
		//COutlineFont * m_pFont;
		Texture *	_background;
		float		_x,_y,_z;
		bool		_initialized;

	public:
		UIItem();

		//TODO: fix font
		//bool Create(const char * pcLabel, COutlineFont * pFont, CTexture * pBackground);
		void render(float x, float y, float z, float width, float height);
		void setLabel(const char * label) {this->_label = label;}
	};

	//a menu object
	class __IMPORTEXPORT UIMenu
	{
	private:
		//just some template class declarations
//		template class __IMPORTEXPORT std::allocator<CUIItem *>;
//		template class __IMPORTEXPORT std::vector<CUIItem *>;

	private:
		int						_currentItem;	//currently selected item
		std::vector<UIItem *>	_items;		//all items in the menu

		//dimensions
		float		_width, _itemHeight;

		unsigned int _maxItemsShow;

		float _x,_y;

		std::string	_title;
		bool		_initialized;
		//TODO: Fix font
//		COutlineFont *		m_Font;
		Texture *	_cursor;

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

		void setPosition(float x, float y) { this->_x = x; this->_y = y; }
	};
}

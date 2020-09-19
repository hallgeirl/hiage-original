/*!
	\file $(filename)

	\author $(author)

	Description:
*/

#pragma once

#include <gtkmm.h>
#include "mapeditgame.h"

class TilePalette : public Gtk::Window
{
protected:
    MapEditGame &gameInstance;

protected:
    //tile palette controls
    std::vector<Gtk::Image*>    images;
    std::vector<Gtk::Button*>   buttons;
    Gtk::ScrolledWindow         paletteScroller;
    Gtk::Table                  paletteTable;
    Gtk::Label                  paletteLabel;
    Gtk::VBox                   paletteBox;

    void tileClicked(int id);
    void clearTiles();

public:
    TilePalette(MapEditGame &game);
    ~TilePalette();

    void updateTiles();
};

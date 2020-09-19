/*!
	\file $(filename)

	\author $(author)

	Description:
*/

#include <math.h>
#include <iostream>
#include "tilepalette.h"

using namespace std;
using namespace Hinage;

TilePalette::TilePalette(MapEditGame &game) : gameInstance(game)
{
    add(paletteBox);
    paletteBox.set_border_width(10);

    //create tile/object palette
    paletteTable.resize(25,4);
    paletteTable.set_spacings(5);

    paletteScroller.add(paletteTable);
    paletteScroller.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    paletteScroller.set_border_width(10);
    set_title("Tile Palette");


    paletteBox.pack_start(paletteLabel, false, false, 0);
    paletteBox.pack_start(paletteScroller);


    paletteLabel.set_label("Tile chooser");
    set_default_size(1, 500);

    show_all_children();
}

TilePalette::~TilePalette()
{
    clearTiles();
}

void TilePalette::tileClicked(int id)
{
    //cout << "Clicked " << id << endl;
    gameInstance.setTileMode(id);
}

void TilePalette::clearTiles()
{

    while (images.size() > 0)
    {
        delete images[0];
        images.erase(images.begin());
    }

    while (buttons.size() > 0)
    {
        paletteTable.remove(*buttons[0]);
        delete buttons[0];
        buttons.erase(buttons.begin());
    }
}

//update the tile palette
void TilePalette::updateTiles()
{
    //gameInstance.setTileset("grassland");
    if (!(gameInstance.getTilesetName().length() > 0))
    {
        return;
    }
    Hinage::Tileset * ts = gameInstance.getTilesetManager().requestResourcePtr(gameInstance.getTilesetName().c_str())->resource;
    Hinage::Tileset::iterator iter = ts->begin();

    int tileCount = ts->getTileCount();
    int rows = (int)(ceil(tileCount / 4));

    clearTiles();
    paletteTable.resize(rows,4);

    for (int y = 0; y < 25; y++)
    {
        for (int x = 0; (x < 4) && (y*4+x < tileCount); x++)
        {
            int index = (y*4)+x;
            int tileIndex = (*iter).id;
            //cout << tileIndex << endl;
            images.push_back(new Gtk::Image);
            buttons.push_back(new Gtk::Button);

            images[index]->set(ts->getTextureFileName(tileIndex));
            images[index]->show();

            buttons[index]->add(*images[index]);
            buttons[index]->signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(*this, &TilePalette::tileClicked), tileIndex));
            buttons[index]->show();

            paletteTable.attach(*buttons[index], x, x+1, y, y+1);
            iter++;
        }
    }
}



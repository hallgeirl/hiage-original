/*!
	\file $(filename)

	\author $(author)

	Description:
*/

#pragma once

#include <gtkmm.h>
#include <libglademm.h>
#include "mapeditgame.h"

class ResizeDialog : public Gtk::Dialog
{
private:
    MapEditGame * gameInstance;

protected:
    //Widgets
    Gtk::SpinButton *   resizeWidth;
    Gtk::SpinButton *   resizeHeight;
    Gtk::SpinButton *   resizeLayers;
    Gtk::SpinButton *   resizeTilesize;
    Gtk::Button *       resizeButton;
    Gtk::Button *       cancelButton;


protected:
    //Signal handlers
    void onResize();
    void onCancel();

public:
    ResizeDialog(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& gladeXML);

    void setGameInstance(MapEditGame * game);

    void updateControls();
};

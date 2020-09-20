/*!
	\file $(filename)

	\author $(author)

	Description:
*/

#pragma once

#include <gtkmm.h>
#include <libglademm.h>
#include "mapeditgame.h"

class NewMapDialog : public Gtk::Dialog
{
protected:
    MapEditGame * gameInstance;

    //entries
    Gtk::SpinButton * widthSpin;
    Gtk::SpinButton * heightSpin;
    Gtk::SpinButton * layersSpin;
    Gtk::SpinButton * tileSizeSpin;

    //buttons
    Gtk::Button * okButton;
    Gtk::Button * cancelButton;

    void onOk();
    void onCancel();

public:
    NewMapDialog(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& gladeXML);
    ~NewMapDialog();

    void setGameInstance(MapEditGame * game);
};

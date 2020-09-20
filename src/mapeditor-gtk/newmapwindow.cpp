/*!
	\file $(filename)

	\author $(author)

	Description:
*/

#include <sstream>
#include "newmapdialog.h"


NewMapDialog::NewMapDialog(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& gladeXML) : Gtk::Dialog(cobject) // : game(gameRef), okButton(Gtk::Stock::OK), cancelButton(Gtk::Stock::CANCEL)
{
    //Widgets
    gladeXML->get_widget("newmap_ok", okButton);
    gladeXML->get_widget("newmap_cancel", cancelButton);

    gladeXML->get_widget("newmap_width", widthSpin);
    gladeXML->get_widget("newmap_height", heightSpin);
    gladeXML->get_widget("newmap_layers", layersSpin);
    gladeXML->get_widget("newmap_tilesize", tileSizeSpin);

    //signals
    okButton->signal_clicked().connect(sigc::mem_fun(*this, &NewMapDialog::onOk));
    cancelButton->signal_clicked().connect(sigc::mem_fun(*this, &NewMapDialog::onCancel));

    show_all_children();
}


NewMapDialog::~NewMapDialog()
{
}

void NewMapDialog::setGameInstance(MapEditGame * game)
{
    gameInstance = game;
}

void NewMapDialog::onCancel()
{
    hide();
}

void NewMapDialog::onOk()
{
    gameInstance->createEmptyMap(widthSpin->get_value_as_int(), heightSpin->get_value_as_int(), layersSpin->get_value_as_int(), tileSizeSpin->get_value_as_int());
    gameInstance->setBackground("");
    hide();
}

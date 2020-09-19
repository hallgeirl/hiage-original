/*!
	\file $(filename)

	\author $(author)

	Description:
*/

#include "resizedialog.h"


ResizeDialog::ResizeDialog(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& gladeXML) : Gtk::Dialog(cobject)
{
    //Get the dialog widgets
    gladeXML->get_widget("resize_width", resizeWidth);
    gladeXML->get_widget("resize_height", resizeHeight);
    gladeXML->get_widget("resize_layers", resizeLayers);
    gladeXML->get_widget("resize_tilesize", resizeTilesize);

    gladeXML->get_widget("resize_ok", resizeButton);
    gladeXML->get_widget("resize_cancel", cancelButton);

    resizeButton->signal_clicked().connect(sigc::mem_fun(*this, &ResizeDialog::onResize));
    cancelButton->signal_clicked().connect(sigc::mem_fun(*this, &ResizeDialog::onCancel));

    gameInstance = 0;
}

void ResizeDialog::setGameInstance(MapEditGame * game)
{
    gameInstance = game;
}

void ResizeDialog::onResize()
{
    if (gameInstance)
    {
        gameInstance->resizeMap(resizeWidth->get_value_as_int(), resizeHeight->get_value_as_int(), resizeLayers->get_value_as_int(), resizeTilesize->get_value_as_int());
    }

    hide();
}

void ResizeDialog::onCancel()
{
    hide();
}

void ResizeDialog::updateControls()
{
    if (gameInstance)
    {
        resizeWidth->set_value(gameInstance->getMapWidth());
        resizeHeight->set_value(gameInstance->getMapHeight());
        resizeLayers->set_value(gameInstance->getMapLayers());
    }
}

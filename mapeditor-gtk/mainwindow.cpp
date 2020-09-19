/*!
	\file $(filename)

	\author Hallgeir Lien

	Description:
*/

#include <iostream>
#include <string>
#include <sstream>
#include <libglademm.h>
#include <fstream>
#include "mainwindow.h"
#include "newmapdialog.h"

using namespace std;
using namespace Hinage;


/*
    The constructor for this window will initialize everything - Game engine, get references to the widgets in the .glade-file and everything.
*/
MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& gladeXML) : Gtk::Window(cobject),/* newMapDialog(game), */tilePalette(game)
{
    //initialize engine
    game.initialize(1024, 768, false);

    //setup the idle function (for map drawing and other fun stuff)
    Glib::signal_idle().connect(sigc::mem_fun(*this,&MainWindow::updateMap));


    tilePalette.set_modal(false);

    //get widgets
    //menu
    gladeXML->get_widget("menu_new", menuNew);
    gladeXML->get_widget("menu_open", menuOpen);
    gladeXML->get_widget("menu_save", menuSave);
    gladeXML->get_widget("menu_save_as", menuSaveAs);
    gladeXML->get_widget("menu_quit", menuQuit);

    gladeXML->get_widget("menu_reset_view", menuResetView);
    gladeXML->get_widget("menu_resize", menuResize);

    //creation
    gladeXML->get_widget("create_objects", createObject);
    gladeXML->get_widget("delete_objects", deleteObject);
    gladeXML->get_widget("edit_tiles", editTiles);
    gladeXML->get_widget("object_selection", objectSelector);
    gladeXML->get_widget("layer_spin", layerSpin);
    gladeXML->get_widget("snap_spin", snapSpin);
    gladeXML->get_widget("enable_snap", snapCheck);

    //view control
    gladeXML->get_widget("zoom_control", zoomSlider);

    //properties
    gladeXML->get_widget("apply_properties", applyPropButton);
    gladeXML->get_widget("background_combo", bgCombo);
    gladeXML->get_widget("tileset_combo", tsCombo);


    //map information
    gladeXML->get_widget("width_label", labelWidth);
    gladeXML->get_widget("height_label", labelHeight);
    gladeXML->get_widget("layers_label", labelLayers);
    gladeXML->get_widget("label_filename", labelFilename);

    //scripts
    gladeXML->get_widget("include_scripts", includeScripts);
    gladeXML->get_widget("init_scripts", initScripts);
    gladeXML->get_widget("update_scripts", updateScripts);
    gladeXML->get_widget("shutdown_scripts", shutdownScripts);
    gladeXML->get_widget("add_include_script", addIncludeScript);
    gladeXML->get_widget("add_init_script", addInitScript);
    gladeXML->get_widget("add_update_script", addUpdateScript);
    gladeXML->get_widget("add_shutdown_script", addShutdownScript);
    gladeXML->get_widget("remove_include_script", removeIncludeScript);
    gladeXML->get_widget("remove_init_script", removeInitScript);
    gladeXML->get_widget("remove_update_script", removeUpdateScript);
    gladeXML->get_widget("remove_shutdown_script", removeShutdownScript);

    //console
    gladeXML->get_widget("console_send", sendConsoleCommand);
    gladeXML->get_widget("console_command", consoleEntry);

    /*
        Dialogs
    */
    gladeXML->get_widget_derived("resize_dialog", resizeMapDialog);
    gladeXML->get_widget_derived("newmap_dialog", newMapDialog);

    /*
        Register signal handlers
    */
    //menu
    menuNew->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onNewMap));
    menuOpen->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onOpen));
    menuSave->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onSave));
    menuSaveAs->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onSaveAs));
    menuQuit->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onQuit));

    menuResetView->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onResetView));
    menuResize->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onResizeMap));

    //creation
    createObject->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onModeChanged));
    editTiles->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onModeChanged));
    deleteObject->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onModeChanged));
    layerSpin->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::onLayerChanged));
    snapSpin->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::onSnapChanged));
    snapSpin->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::onSnapChanged));
    snapCheck->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onSnapChanged));

    //view control
    zoomSlider->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::onZoomChanged));

    //properties
    applyPropButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onApplyProperties));

    //scripts
    addIncludeScript->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onAddIncludeScript));
    addInitScript->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onAddInitScript));
    addUpdateScript->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onAddUpdateScript));
    addShutdownScript->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onAddShutdownScript));
    removeIncludeScript->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onRemoveIncludeScript));
    removeInitScript->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onRemoveInitScript));
    removeUpdateScript->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onRemoveUpdateScript));
    removeShutdownScript->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onRemoveShutdownScript));


    //console
    sendConsoleCommand->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::executeConsoleCommand));
    consoleEntry->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::executeConsoleCommand));

    /*
        Initialize dialogs
    */
    resizeMapDialog->setGameInstance(&game);
    newMapDialog->setGameInstance(&game);

    /*
        Initialize combo boxes
    */
    //combo box data
    backgroundsData     = Gtk::ListStore::create(textColumn);
    tilesetsData        = Gtk::ListStore::create(textColumn);
    objectsData         = Gtk::ListStore::create(textColumn);
    includeScriptsData  = Gtk::ListStore::create(textColumn);
    initScriptsData     = Gtk::ListStore::create(textColumn);
    updateScriptsData   = Gtk::ListStore::create(textColumn);
    shutdownScriptsData = Gtk::ListStore::create(textColumn);

    //combo box data
    //fill the BG combo box
    bgCombo->set_model(backgroundsData);
    bgCombo->pack_start(textColumn.columnText);
    for (int i = 0; i < game.getTextureManager().getResourceCount(); i++)
    {
        Gtk::TreeModel::iterator iter = backgroundsData->append();
        (*iter)[textColumn.columnText] = game.getTextureManager().requestResourcePtr(i)->name;
    }
    bgCombo->set_active(0);

    //fill the tileset combo box
    tsCombo->set_model(tilesetsData);
    tsCombo->pack_start(textColumn.columnText);
    for (int i = 0; i < game.getTilesetManager().getResourceCount(); i++)
    {
        Gtk::TreeModel::iterator iter = tilesetsData->append();
        (*iter)[textColumn.columnText] = game.getTilesetManager().requestResourcePtr(i)->name;
    }
    tsCombo->set_active(0);


    //fill in the select object box and assign a handler for the changed signal
    ObjectList &objlist = game.getObjectList();

    objectSelector->set_model(objectsData);
    objectSelector->pack_start(textColumn.columnText);
    for (ObjectList::iterator itr = objlist.begin(); itr != objlist.end(); itr++)
    {

        Gtk::TreeModel::iterator iter = objectsData->append();
        (*iter)[textColumn.columnText] = itr->first;
    }
    objectSelector->set_active(0);
    objectSelector->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::onObjectChanged));


    //initialize model and such for script combos
    includeScripts->set_model(includeScriptsData);
    includeScripts->set_text_column(textColumn.columnText);
    initScripts->set_model(initScriptsData);
    initScripts->set_text_column(textColumn.columnText);
    updateScripts->set_model(updateScriptsData);
    updateScripts->set_text_column(textColumn.columnText);
    shutdownScripts->set_model(shutdownScriptsData);
    shutdownScripts->set_text_column(textColumn.columnText);

    //update the controls (fill in values taken from the map)
    updateControls();
    onZoomChanged();


    //get the filename
    try
    {
        ifstream instream("recent");
        if (!instream.is_open())
        {
            throw IOException("Could not open the config file with the recent filename.");
        }
        instream >> filename;
        instream.close();
    }
    catch (...)
    {
        filename = "";
    }
}


MainWindow::~MainWindow()
{
}


//Draw the map
bool MainWindow::updateMap()
{
    game.run(1, false);
    static Hinage::Timer updateTimer(true);
    static bool first = true;

    if (updateTimer.hasReached(0.5) && first || updateTimer.hasReached(5))
    {
        updateControls();
        updateTimer.reset();
        first = false;
    }

    if (!game.isRunning())
    {
        hide();
    }
    return true;
}


//set the properties of the controls
void MainWindow::updateControls()
{
    //set range of layer spinner
    double currentMin, currentMax;
    layerSpin->get_range(currentMin, currentMax);
    if (currentMax != game.getMapLayers()-1)
    {
        layerSpin->set_range(0, game.getMapLayers() - 1);
    }

    tilePalette.updateTiles();

    //extract map dimensions and set the labels
    stringstream ss;
    std::string temp;
    ss << game.getMapWidth() << " " << game.getMapHeight() << " " << game.getMapLayers() << " ";

    temp = "";
    ss >> temp;
    temp = "Width:\n" + temp;
    labelWidth->set_label(temp);

    temp = "";
    ss >> temp;
    temp = "Height:\n" + temp;
    labelHeight->set_label(temp);

    temp = "";
    ss >> temp;
    temp = "Layers:\n" + temp;
    labelLayers->set_label(temp);

    //set the filename label
    labelFilename->set_label(string("Currently open file: ") + filename);

    //fill the script combo boxes
    includeScriptsData->clear();
    initScriptsData->clear();
    updateScriptsData->clear();
    shutdownScriptsData->clear();

    //include scripts
    for (size_t i = 0; i < game.getScriptCount(MapEditGame::INCLUDE); i++)
    {
        Gtk::TreeModel::iterator iter = includeScriptsData->append();
        (*iter)[textColumn.columnText] = game.getScript(i, MapEditGame::INCLUDE);
    }

    //init scripts
    for (size_t i = 0; i < game.getScriptCount(MapEditGame::INIT); i++)
    {
        Gtk::TreeModel::iterator iter = initScriptsData->append();
        (*iter)[textColumn.columnText] = game.getScript(i, MapEditGame::INIT);
    }

    //update scripts
    for (size_t i = 0; i < game.getScriptCount(MapEditGame::UPDATE); i++)
    {
        Gtk::TreeModel::iterator iter = updateScriptsData->append();
        (*iter)[textColumn.columnText] = game.getScript(i, MapEditGame::UPDATE);
    }

    //shutdown scripts
    for (size_t i = 0; i < game.getScriptCount(MapEditGame::SHUTDOWN); i++)
    {
        Gtk::TreeModel::iterator iter = shutdownScriptsData->append();
        (*iter)[textColumn.columnText] = game.getScript(i, MapEditGame::SHUTDOWN);
    }
}

/*
    Handlers for the file menu
*/
void MainWindow::onNewMap()
{
    //newMapDialog.set_modal(true);
    newMapDialog->set_transient_for(*this);

    int result =newMapDialog->run();

    if (result == 1)
    {
        filename = "";
        updateControls();
    }
}

void MainWindow::onSaveAs()
{
    //open a file chooser dialog
    Gtk::FileChooserDialog dialog("Please select a filename", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_do_overwrite_confirmation(true);
    dialog.set_transient_for(*this);

    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Save", Gtk::RESPONSE_OK);

    int result = dialog.run();

    if (result == Gtk::RESPONSE_OK)
    {
        game.saveMap(dialog.get_filename());
        filename = dialog.get_filename();

        //store the filename
        ofstream outstream("recent");
        outstream << filename;
        outstream.close();
    }
}

void MainWindow::onSave()
{
    //has the map been opened or saved before? if no, open a file chooser dialog.
    if (filename.length() > 0)
    {
        game.saveMap(filename);
    }
    else
    {
        //open a file chooser dialog
        Gtk::FileChooserDialog dialog("Please select a filename", Gtk::FILE_CHOOSER_ACTION_SAVE);
        dialog.set_do_overwrite_confirmation(true);
        dialog.set_transient_for(*this);

        dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
        dialog.add_button("Save", Gtk::RESPONSE_OK);

        int result = dialog.run();

        if (result == Gtk::RESPONSE_OK)
        {
            game.saveMap(dialog.get_filename());
            filename = dialog.get_filename();
        }
    }

    //store the filename
    ofstream outstream("recent");
    outstream << filename;
    outstream.close();
}

void MainWindow::onOpen()
{
    //open a file chooser dialog
    Gtk::FileChooserDialog dialog("Please select a map to load", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);

    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Open", Gtk::RESPONSE_OK);

    int result = dialog.run();

    if (result == Gtk::RESPONSE_OK)
    {
        game.loadMap(dialog.get_filename());
        filename = dialog.get_filename();
        tilePalette.updateTiles();
        updateControls();

        //store the filename
        ofstream outstream("recent");
        outstream << filename;
        outstream.close();
    }
}

void MainWindow::onQuit()
{
    hide();
}

/*
    Handlers for the Tools menu
*/
void MainWindow::onResizeMap()
{
    resizeMapDialog->updateControls();
    resizeMapDialog->present();
}

/*
    Handlers for the View menu
*/
void MainWindow::onResetView()
{
    game.getDisplay().setCamPosition(0,0);
    zoomSlider->set_value(300);
    game.getDisplay().setZoom(300);
}

//When changing from edit tile-mode, to delete mode, or to create object-mode
void MainWindow::onModeChanged()
{
    if (createObject->get_active())
    {
        tilePalette.hide();
        game.setObjectMode((*objectSelector->get_active())[textColumn.columnText]);
    }
    else if (editTiles->get_active())
    {
        tilePalette.show();
        game.setTileMode(0);
    }
    else if (deleteObject->get_active())
    {
        tilePalette.hide();
        game.setDeleteMode();
    }
}

//When changing which layer to draw to
void MainWindow::onLayerChanged()
{
    game.setDrawingLayer(layerSpin->get_value_as_int());
    cout << "Drawing to layer " << layerSpin->get_value_as_int() << endl;
}

//When changing which object to create
void MainWindow::onObjectChanged()
{
    game.setObjectMode((*objectSelector->get_active())[textColumn.columnText]);
}

//When the zoom slider has changed
void MainWindow::onZoomChanged()
{
    game.getDisplay().setZoom(zoomSlider->get_value());
}

//When the "Snap to grid" checkbox is changed, or the snap spin button has changed
void MainWindow::onSnapChanged()
{
    if (snapCheck->get_active())
    {
        game.setSnap(snapSpin->get_value_as_int());
    }
    else
    {
        game.setSnap(1);
    }
}

//When the "Apply Properties" button has been clicked
void MainWindow::onApplyProperties()
{
    game.setBackground((*bgCombo->get_active())[textColumn.columnText]);
    game.setTileset((*tsCombo->get_active())[textColumn.columnText]);

    tilePalette.updateTiles();

}

/*
    Handlers for script removal/adding
*/
void MainWindow::onRemoveShutdownScript()
{
    game.removeScript(shutdownScripts->get_entry()->get_text(), MapEditGame::SHUTDOWN);
    shutdownScripts->get_entry()->set_text("");
    updateControls();
}

void MainWindow::onRemoveUpdateScript()
{
    game.removeScript(updateScripts->get_entry()->get_text(), MapEditGame::UPDATE);
    updateScripts->get_entry()->set_text("");
    updateControls();
}

void MainWindow::onRemoveInitScript()
{
    game.removeScript(initScripts->get_entry()->get_text(), MapEditGame::INIT);
    initScripts->get_entry()->set_text("");
    updateControls();
}

void MainWindow::onRemoveIncludeScript()
{
    game.removeScript(includeScripts->get_entry()->get_text(), MapEditGame::INCLUDE);
    includeScripts->get_entry()->set_text("");
    updateControls();
}

void MainWindow::onAddShutdownScript()
{
    game.addScript(shutdownScripts->get_entry()->get_text(), MapEditGame::SHUTDOWN);
    shutdownScripts->get_entry()->set_text("");
    updateControls();
}

void MainWindow::onAddUpdateScript()
{
    game.addScript(updateScripts->get_entry()->get_text(), MapEditGame::UPDATE);
    updateScripts->get_entry()->set_text("");
    updateControls();
}

void MainWindow::onAddInitScript()
{
    game.addScript(initScripts->get_entry()->get_text(), MapEditGame::INIT);
    initScripts->get_entry()->set_text("");
    updateControls();
}

void MainWindow::onAddIncludeScript()
{
    game.addScript(includeScripts->get_entry()->get_text(), MapEditGame::INCLUDE);
    includeScripts->get_entry()->set_text("");
    updateControls();
}

//Execute a console command
void MainWindow::executeConsoleCommand()
{
    game.scriptVM.executeLine(consoleEntry->get_text());
    consoleEntry->set_text("");
}


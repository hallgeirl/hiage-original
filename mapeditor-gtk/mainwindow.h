/*!
	\file $(filename)

	\author $(author)

	Description:
*/

#pragma once

#include <gtkmm.h>
#include <vector>
#include <libglademm.h>

#include "mapeditgame.h"
#include "newmapdialog.h"
#include "tilepalette.h"
#include "resizedialog.h"



class MainWindow : public Gtk::Window
{
public:
    MapEditGame game;
    //NewMapDialog newMapDialog;
    TilePalette  tilePalette;
    std::string filename;


protected:
    /*
        Widgets
    */
    //menu
    Gtk::MenuItem *     menuNew;
    Gtk::MenuItem *     menuOpen;
    Gtk::MenuItem *     menuSave;
    Gtk::MenuItem *     menuSaveAs;
    Gtk::MenuItem *     menuQuit;

    Gtk::MenuItem *     menuResize;
    Gtk::MenuItem *     menuResetView;

    //creation
    Gtk::RadioButton *  createObject;
    Gtk::RadioButton *  editTiles;
    Gtk::RadioButton *  deleteObject;
    Gtk::ComboBox    *  objectSelector;
    Gtk::SpinButton  *  layerSpin;
    Gtk::SpinButton  *  snapSpin;
    Gtk::CheckButton *  snapCheck;


    //view control
    Gtk::HScale *       zoomSlider;

    //properties
    Gtk::Button *       applyPropButton;
    Gtk::ComboBox *     bgCombo;
    Gtk::ComboBox *     tsCombo;

    //map information
    Gtk::Label *        labelWidth;
    Gtk::Label *        labelHeight;
    Gtk::Label *        labelLayers;
    Gtk::Label *        labelFilename;

    //scripts
    Gtk::ComboBoxEntry* includeScripts;
    Gtk::ComboBoxEntry* initScripts;
    Gtk::ComboBoxEntry* updateScripts;
    Gtk::ComboBoxEntry* shutdownScripts;
    Gtk::Button *       addIncludeScript;
    Gtk::Button *       addInitScript;
    Gtk::Button *       addUpdateScript;
    Gtk::Button *       addShutdownScript;
    Gtk::Button *       removeIncludeScript;
    Gtk::Button *       removeInitScript;
    Gtk::Button *       removeUpdateScript;
    Gtk::Button *       removeShutdownScript;

    //console
    Gtk::Button *   sendConsoleCommand;
    Gtk::Entry *    consoleEntry;

    //menus
    Glib::RefPtr<Gtk::ActionGroup> actionGroup;
    Glib::RefPtr<Gtk::UIManager>    uiManager;

    /*
        Dialogs
    */

    //Resize map-dialog
    ResizeDialog *      resizeMapDialog;
    //New map dialog
    NewMapDialog *      newMapDialog;

    /*
        Combo boxes and their data storage
    */
    //record to store the combo box items
    class TextColumn : public Gtk::TreeModelColumnRecord
    {
    public:
        TextColumn()
        {
            add(columnText);
        }

        Gtk::TreeModelColumn<std::string> columnText;
    };

    TextColumn textColumn;

    //Glib::RefPtr<Gtk::ListStore>
    Glib::RefPtr<Gtk::ListStore> backgroundsData;
    Glib::RefPtr<Gtk::ListStore> tilesetsData;
    Glib::RefPtr<Gtk::ListStore> objectsData;
    Glib::RefPtr<Gtk::ListStore> includeScriptsData;
    Glib::RefPtr<Gtk::ListStore> initScriptsData;
    Glib::RefPtr<Gtk::ListStore> updateScriptsData;
    Glib::RefPtr<Gtk::ListStore> shutdownScriptsData;


protected:
    /*
        Signal handlers
    */
    //execute a line of LUA script (typed from the text box)
    void executeConsoleCommand();

    //render the map
    bool updateMap();

    //apply the map properties
    void onApplyProperties();

    //menu actions
    void onNewMap();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onQuit();

    void onResizeMap();
    void onResetView();


    void onModeChanged();
    void onLayerChanged();
    void onZoomChanged();
    void onObjectChanged();

    //script adding/removal
    void onAddIncludeScript();
    void onAddInitScript();
    void onAddUpdateScript();
    void onAddShutdownScript();
    void onRemoveIncludeScript();
    void onRemoveInitScript();
    void onRemoveUpdateScript();
    void onRemoveShutdownScript();

    //snapping
    void onSnapChanged();


public:
    MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
    virtual ~MainWindow();

    //update all controls with values from the map
    void updateControls();
};

/*!
	\file main.cpp

	\author Hallgeir Lien

	Description:
*/

#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include "mainwindow.h"

using namespace std;

int main(int argc, char** argv)
{
    //Print to a log file instead of console when building the release
    #ifdef RELEASE
	ofstream file("log-mapeditor.log");
	streambuf *oldLogBuffer = clog.rdbuf(file.rdbuf());
	streambuf *oldErrBuffer = cerr.rdbuf(file.rdbuf());*/
    #endif

    Gtk::Main kit(argc, argv);

    Glib::RefPtr<Gnome::Glade::Xml> gladeXML = Gnome::Glade::Xml::create("ui-main.glade");

    MainWindow * window;
    gladeXML->get_widget_derived("main_window", window);

    Gtk::Main::run(*window);

    #ifdef RELEASE
	clog.rdbuf(oldLogBuffer);
	cerr.rdbuf(oldErrBuffer);
	#endif

    return 0;
}

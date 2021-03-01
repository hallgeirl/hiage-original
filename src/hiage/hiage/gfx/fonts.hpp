#pragma once

#include <string>
#include "texture.h"
#include "../util/vector2.h"
#include "renderer.h"

//#include "com_typedefs.h"

namespace hiage
{
    class __IMPORTEXPORT Font
    {
    private:
        int _characterWidth, _characterHeight;
        Texture *_texture;
        char ** _characterTable;
        int _tableRows, _tableCols;
        bool _loaded;

    public:
        Font();
        ~Font();

        void create(Texture * tex, int characterWidth, int characterHeight);
        void getIndexOfCharacter(char c, int &x, int &y);
        void setCharacterTable(char ** characterTable, int cols, int rows);
        void destroyTable();
        char ** getCharacterTable();

        int getTableRows();
        int getTableCols();

        int getCharacterHeight();

        void renderText(Renderer &renderer, std::string text, Vector2<double> position, double scale = 1, double spacing = 0);
    };
}

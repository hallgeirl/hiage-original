#pragma once

#include <string>
#include "texture.h"
#include "../util/vector2.h"
#include "renderer.h"

//#include "com_typedefs.h"

namespace Hinage
{
    class __IMPORTEXPORT Font
    {
    private:
        int characterWidth, characterHeight;
        Texture *texture;
        char ** characterTable;
        int tableRows, tableCols;
        bool loaded;

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

        void renderText(Renderer &renderer, std::string text, Vector2<double> position, float scale = 1, float spacing = 0);
    };
}

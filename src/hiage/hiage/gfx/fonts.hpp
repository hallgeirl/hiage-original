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
        Texture _texture;
        char ** _characterTable;
        int _tableRows, _tableCols;
        bool _loaded;

    public:
        Font();
        ~Font();

        void create(const Texture& tex, int characterWidth, int characterHeight);
        void getIndexOfCharacter(char c, int &x, int &y) const;
        void setCharacterTable(char ** characterTable, int cols, int rows);
        void destroyTable();
        char ** getCharacterTable() const;

        int getTableRows() const;
        int getTableCols() const;

        int getCharacterHeight() const;

        void renderText(Renderer &renderer, std::string text, Vector2<double> position, double scale = 1, double spacing = 0) const;
    };
}

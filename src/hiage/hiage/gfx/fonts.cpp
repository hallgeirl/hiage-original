#include "fonts.hpp"

#include <iostream>
#include <string>
#include "../util/exceptions.h"

using namespace std;
using namespace hiage;

Font::Font() : _tableRows(0), _tableCols(0), _characterWidth(0), _characterHeight(0)
{
    _loaded = false;

    _characterTable = 0;
}

Font::~Font()
{
}

//Create the font from a texture
void Font::create(const Texture& tex, int characterWidth, int characterHeight)
{
    clog << "Creating font from texture with character width " << characterWidth << " and character height " << characterHeight << "...\n" << flush;

    _texture = tex;
    this->_characterWidth = characterWidth;
    this->_characterHeight = characterHeight;

    _loaded = true;
    clog << "OK: Font created successfully.\n" << flush;
}

void Font::getIndexOfCharacter(char c, int &x, int &y)
{
    x = -1;
    y = -1;

    if (!_characterTable)
    {
        return;
    }

    for (int i = 0; i < _tableCols; i++)
    {
        for (int j = 0; j < _tableRows; j++)
        {
            if (_characterTable[i][j] == c)
            {
                x = i;
                y = j;
                return;
            }
        }
    }
}

void Font::setCharacterTable(char ** table, int cols, int rows)
{
    if (_characterTable)
    {
        destroyTable();
    }

    if (!table)
    {
        return;
    }

    //allocate memory for new table
    _characterTable = new char*[cols];
    for (int i = 0; i < cols; i++)
    {
        _characterTable[i] = new char[rows];
    }

    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            _characterTable[i][j] = table[i][j];
        }
    }

    _tableRows = rows;
    _tableCols = cols;

}

char ** Font::getCharacterTable() const
{
    return _characterTable;
}

int Font::getTableCols() const
{
    return _tableCols;
}

int hiage::Font::getCharacterHeight() const
{
    return _characterHeight;
}

int Font::getTableRows() const
{
    return _tableRows;
}

void Font::destroyTable()
{
    if (!_characterTable)
    {
        return;
    }

    for (int i = 0; i < _tableCols; i++)
    {
        delete [] _characterTable[i];
    }

    delete [] _characterTable;

    _characterTable = 0;
}

void Font::renderText(Renderer &renderer, string text, Vector2<double> position, double scale, double spacing)
{
    if (!_loaded)
    {
        return;
    }

    int x,y;

    for (int c = 0; c < text.length(); c++)
    {
        getIndexOfCharacter(text.at(c), x,y);
        //texture->select();
        double xpos = (1 + spacing)*((int)c * _characterWidth*scale) + position.getX();
        double ypos = position.getY();

        double charwidth = (1.0/_texture.getWidth()) * _characterWidth;
        double charheight = (1.0/_texture.getHeight()) * _characterHeight;

        double xoffset = charwidth * x;

        double yoffset = charheight * y;

        renderer.beginRender(ObjectZ::CLOSEST, &_texture);

        if (x >= 0 || y >= 0)
         {

         	renderer.addVertex(xpos, ypos, xoffset, (double)yoffset + charheight);
         	renderer.addVertex((double)xpos + (double)_characterWidth * scale, ypos, (double)xoffset + charwidth, (double)yoffset + charheight);
         	renderer.addVertex((double)xpos + (double)_characterWidth * scale, (double)ypos + (double)_characterHeight * scale, (double)xoffset + charwidth, yoffset);
         	renderer.addVertex(xpos, (double)ypos + (double)_characterHeight * scale, xoffset, yoffset);
        }

        renderer.endRender();
    }
}

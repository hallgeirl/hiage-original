#include <iostream>
#include <string>
#include "fonts.h"
#include "../util/exceptions.h"

using namespace std;
using namespace hiage;

Font::Font()
{
    loaded = false;

    characterTable = 0;
}

Font::~Font()
{
}

//Create the font from a texture
void Font::create(Texture * tex, int characterWidth, int characterHeight)
{
    clog << "Creating font from texture with character width " << characterWidth << " and character height " << characterHeight << "...\n" << flush;

    texture = tex;
    this->characterWidth = characterWidth;
    this->characterHeight = characterHeight;

    if (!texture)
    {
        throw IOException("ERROR: No texture specified for font.");
    }

    loaded = true;
    clog << "OK: Font created successfully.\n" << flush;
}

void Font::getIndexOfCharacter(char c, int &x, int &y)
{
    x = -1;
    y = -1;

    if (!characterTable)
    {
        return;
    }

    for (int i = 0; i < tableCols; i++)
    {
        for (int j = 0; j < tableRows; j++)
        {
            if (characterTable[i][j] == c)
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
    if (characterTable)
    {
        destroyTable();
    }

    if (!table)
    {
        return;
    }

    //allocate memory for new table
    characterTable = new char*[cols];
    for (int i = 0; i < cols; i++)
    {
        characterTable[i] = new char[rows];
    }

    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            characterTable[i][j] = table[i][j];
        }
    }

    tableRows = rows;
    tableCols = cols;

}

char ** Font::getCharacterTable()
{
    return characterTable;
}

int Font::getTableCols()
{
    return tableCols;
}

int Font::getTableRows()
{
    return tableRows;
}

void Font::destroyTable()
{
    if (!characterTable)
    {
        return;
    }

    for (int i = 0; i < tableCols; i++)
    {
        delete [] characterTable[i];
    }

    delete [] characterTable;

    characterTable = 0;
}

void Font::renderText(Renderer &renderer, string text, Vector2<double> position, float scale, float spacing)
{
    if (!loaded)
    {
        return;
    }

    int x,y;

    for (unsigned int c = 0; c < text.length(); c++)
    {
        getIndexOfCharacter(text.at(c), x,y);
        //texture->select();
        float xpos = (1 + spacing)*(c * characterWidth*scale) + position.getX();
        float ypos = position.getY();

        float charwidth = (1.0f/texture->getWidth()) * characterWidth;
        float charheight = (1.0f/texture->getHeight()) * characterHeight;

        float xoffset = charwidth * x;

        float yoffset = charheight * y;

        renderer.beginRender(Renderer::CLOSEST, texture);

        if (x >= 0 || y >= 0)
         {

         	renderer.addVertex(xpos, ypos, xoffset, yoffset + charheight);
         	renderer.addVertex(xpos + characterWidth * scale, ypos, xoffset + charwidth, yoffset + charheight);
         	renderer.addVertex(xpos + characterWidth * scale, ypos + characterHeight * scale, xoffset + charwidth, yoffset);
         	renderer.addVertex(xpos, ypos + characterHeight * scale, xoffset, yoffset);
        }

        renderer.endRender();
    }
}

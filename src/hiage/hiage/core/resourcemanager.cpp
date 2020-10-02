#include "resourcemanager.hpp"
#pragma warning(push, 0)
#include <tinyxml.h>
#pragma warning(pop)

#include <sstream>
#include <iostream>

using namespace hiage;
using namespace std;

std::unique_ptr<Resource<Texture>> TextureManager::loadResource(const std::string& path)
{
	clog << "Loading texture: " << path << endl << flush;
	Texture * texture = new Texture();
	Resource<Texture> * resource = new Resource<Texture>;

	resource->resource = texture;

	//load the xml file
	TiXmlDocument xmlDoc(path.c_str());
	if (!xmlDoc.LoadFile())
	{
		throw IOException("ERROR: Could not open XML file.");
	}

	std::string bitmap;
	TiXmlElement * textureElement = 0;
	TiXmlElement * bitmapElement = 0;

	textureElement = xmlDoc.FirstChildElement("texture");

	//check if it's a texture file
	if (!textureElement)
	{
		throw IOException("ERROR: Could not find XML element <texture>.");
	}


	resource->name = textureElement->Attribute("name");
	clog << "- Name: " << resource->name << endl << flush;

	bitmapElement = textureElement->FirstChildElement("bitmap");
	bitmap = bitmapElement->Attribute("path");

	//check if the texture can be loaded
	/*if (!texture->loadTexture(bitmap.c_str()))
	{
		throw IOException(string("ERROR: Could not load texture ") + bitmap.c_str());
	}*/
	texture->loadTexture(bitmap.c_str());

	resource->strData1 = bitmap;

	clog << "OK: Texture loaded successfully.\n" << flush;

	return std::unique_ptr<Resource<Texture>>(resource);
}

std::unique_ptr<Resource<Sprite>> SpriteManager::loadResource(const std::string& path)
{
	clog << "Loading sprite: " << path << endl << flush;
	Sprite * sprite = new Sprite();
	Resource<Sprite> * resource = new Resource<Sprite>;

	resource->resource = sprite;

	//load a xml file
	TiXmlDocument xmlDoc(path.c_str());

	if (!xmlDoc.LoadFile())
	{
		throw IOException("ERROR: Could not open XML file.");
	}

	TiXmlElement * spriteElement = 0;
	TiXmlElement * textureElement = 0;
	TiXmlElement * animationElement = 0;
	TiXmlElement * frameElement = 0;

	spriteElement = xmlDoc.FirstChildElement("sprite");
	//sprite element not found, abort
	if (!spriteElement)
	{
		throw IOException("ERROR: Could not find XML element <sprite>.");
	}

	//load the name attribute
	resource->name = spriteElement->Attribute("name");
	clog << "- Name: " << resource->name << endl << flush;

	//load frame dimensions
	spriteElement->Attribute("framewidth", &resource->intData1);
	spriteElement->Attribute("frameheight", &resource->intData2);
	clog << "- Frame width: " << resource->intData1 << endl << "- Frame height: " << resource->intData2 << endl << flush;

	//check for valid frame dimensions
	if ((resource->intData1 <= 0) || (resource->intData2 <= 0))
	{
		throw IOException("ERROR: Invalid frame dimensions.");
	}

	//find the texture element
	textureElement = spriteElement->FirstChildElement("texture");
	if (!textureElement)
	{
		throw IOException("ERROR: Could not find XML element <texture>.");
		return 0;
	}
	resource->strData1 = textureElement->Attribute("name");
	clog << "- Texture name: " << resource->strData1 << endl << flush;

	//load animations
	animationElement = spriteElement->FirstChildElement("animation");

	//cycle through all animations
	while (animationElement)
	{
		unsigned int i;

		frameElement = animationElement->FirstChildElement("frame");

		//no frames, no animation. skip to next
		if (!frameElement)
		{
			clog << "Warning: No frames in animation with name " << animationElement->Attribute("name") << ". Skipping. " << endl << flush;
			continue;
		}

		i = sprite->addAnimation(animationElement->Attribute("name"));

		while (frameElement)
		{
			int x, y, nextFrame;
			double delay;
			BoundingBox<double> colBox;
			std::string p_sTemp = "";

			frameElement->Attribute("x",&x);
			frameElement->Attribute("y",&y);
			frameElement->Attribute("nextframe",&nextFrame);
			frameElement->Attribute("delay",&delay);
			clog << "- Frame information:\n" << "  - x: " << x << " y: " << y << endl << "  - Next frame #: " << nextFrame << endl << "  - Frame delay: " << delay << endl << flush;

			//get collision box
			if (frameElement->Attribute("colbox"))
			{
				p_sTemp = frameElement->Attribute("colbox");
			}

			if (p_sTemp.length())
			{
				std::istringstream stream(p_sTemp);

				stream >> colBox.left >> colBox.bottom >> colBox.right >> colBox.top;
			}
			else
			{
				colBox.left = 0;
				colBox.bottom = 0;
				colBox.right = resource->intData1;
				colBox.top = resource->intData2;
			}
			clog << "  - Collision box: " << colBox << endl << flush;

			if ((x >= 0) && (y >= 0) && (nextFrame >= 0) && (delay > 0))
			{
				sprite->addFrame(i, x, y, delay, nextFrame, colBox);
				clog << "OK: Added frame to animation.\n" << flush;
			}
			else
			{
				clog << "Warning: Invalid x, y, nextFrame or delay parameters.\n" << flush;
			}

			frameElement = frameElement->NextSiblingElement("frame");
		}

		animationElement = animationElement->NextSiblingElement("animation");
	}

	clog << "OK: Sprite loaded successfully.\n" << flush;

	return std::unique_ptr<Resource<Sprite>>(resource);
}

std::unique_ptr<Resource<Tileset>> TilesetManager::loadResource(const std::string& path)
{
	clog << "Loading tile set: " << path << endl << flush;
	Tileset * tileset = new Tileset();
	Resource<Tileset> * resource = new Resource<Tileset>;

	resource->resource = tileset;

	//load a xml file
	TiXmlDocument xmlDoc(path.c_str());

	if (!xmlDoc.LoadFile())
	{
		throw IOException("ERROR: Could not open XML file.");
	}

	TiXmlElement * tilesetElement = 0;
	TiXmlElement * tileElement = 0;

	tilesetElement = xmlDoc.FirstChildElement("tileset");
	if (!tilesetElement)
	{
		throw IOException("ERROR: Could not find XML element <tileset>.");
		return 0;
	}

	resource->name = tilesetElement->Attribute("name");
	clog << "- Tile set name: " << resource->name << endl << flush;

	Texture * texture;
	std::string texPath;
	int id, delay, block, nextTile, counter = 0;

	tileElement = tilesetElement->FirstChildElement("tile");

	while (tileElement)
	{
		++counter;
		texture = 0;
		texPath = tileElement->Attribute("texture");
		tileElement->Attribute("id", &id);
		tileElement->Attribute("nexttile",&nextTile);
		if (nextTile != -1)
		{
            tileElement->Attribute("delay",&delay);
		}
        tileElement->Attribute("block",&block);

		clog << "- Tile information: \n  - ID: " << id << "\n  - Block value: " << block;
		if (nextTile != -1)
		{
		    clog << "\n  - Animation delay: " << delay << "\n  - Next tile: " << nextTile;
        }
        clog << endl;

		//TODO: Debug message? And is the Texture deleted?
		texture = new Texture;
		texture->loadTexture(texPath.c_str());

        if (nextTile != -1)
        {
            tileset->addTile(texture, block, nextTile, delay, texPath, id);
        }
        else
        {
            tileset->addTile(texture, block, -1, 0, texPath, id);
        }
		tileElement = tileElement->NextSiblingElement("tile");
	}

	if (counter == 0)
	{
		clog << "Warning: Tile set is empty.\n" << flush;
	}

	return std::unique_ptr<Resource<Tileset>>(resource);
}

std::unique_ptr<Resource<Font>> FontManager::loadResource(const std::string& path)
{
	clog << "Loading font: " << path << endl << flush;
	Font * font = new Font();
	Resource<Font> * resource = new Resource<Font>;

	resource->resource = font;

	//load a xml file
	TiXmlDocument xmlDoc(path.c_str());

	if (!xmlDoc.LoadFile())
	{
		throw IOException("ERROR: Could not open XML file.");
	}

	TiXmlElement * fontElement = 0;
	TiXmlElement * tableElement = 0;
	TiXmlElement * rowElement = 0;
	TiXmlElement * textureElement = 0;

	fontElement = xmlDoc.FirstChildElement("font");
	if (!fontElement)
	{
		throw IOException("ERROR: Could not find XML element <font>.");
	}

	resource->name = fontElement->Attribute("name");
	fontElement->Attribute("charwidth", &resource->intData1);
	fontElement->Attribute("charheight", &resource->intData2);
	clog << "- Font name: " << resource->name << endl << flush;
	clog << "- Character width: " << resource->intData1 << ", character height: " << resource->intData2 << endl << flush;

	//check for valid character dimensions
	if ((resource->intData1 <= 0) || (resource->intData2 <= 0))
	{
		throw IOException("ERROR: Invalid character dimensions.");
	}

	//find the texture element
	textureElement = fontElement->FirstChildElement("texture");
	if (!textureElement)
	{
		throw IOException("ERROR: Could not find XML element <texture>.");
	}
    resource->strData1 = textureElement->Attribute("name");
    clog << "- Texture name: " << resource->strData1 << endl << flush;


	tableElement = fontElement->FirstChildElement("chartable");
    if (!tableElement)
    {
        throw IOException("ERROR: Could not find XML element <chartable>.");
    }

    rowElement = tableElement->FirstChildElement("row");
    if (!rowElement)
    {
        throw IOException("ERROR: Could not find any rows in the character table.");
    }

    //load the character table
    int rowCount = 0;
    unsigned int charsPerRow = 0;

    vector<vector<char> > rows;
    while (rowElement)
    {
        vector<char> row;
        string rawrow = rowElement->GetText();

        //extract characters from comma-seperated string
        for (unsigned int i = 0; i < rawrow.length(); i++)
        {
            if (rawrow.c_str()[i] == ',')
            {
                i++;
                row.push_back(rawrow.c_str()[i]);
            }
            else
            {
                row.push_back(rawrow.c_str()[i]);
            }
        }

        //store the length of the longest row
        if (charsPerRow < row.size())
        {
            charsPerRow = row.size();
        }

        rows.push_back(row);

        rowElement = rowElement->NextSiblingElement("row");
    }

    //allocate memory for character table
    char ** charTable = new char*[charsPerRow];
    rowCount = rows.size();

    for (unsigned int i = 0; i < charsPerRow; i++)
    {
        charTable[i] = new char[rowCount];
    }

    clog << "- Processing character table...\n" << flush;
    for (unsigned int i = 0; i < charsPerRow; i++)
    {
        for (int j = 0; j < rowCount; j++)
        {
            //check for existance of element in vector. if nonexistant, just set the character to null.
            if (rows[j].size() > i)
            {
                charTable[i][j] = rows[j][i];
            }
            else
            {
                charTable[i][j] = 0;
            }
        }
    }
    clog << "- Character table done.\n" << flush;
    font->setCharacterTable(charTable, charsPerRow, rowCount);

    for (unsigned int i = 0; i < charsPerRow; i++)
    {
        delete [] charTable[i];
    }

    delete [] charTable;

    clog << "OK: Font loaded successfully.\n" << flush;

	return std::unique_ptr<Resource<Font>>(resource);
}

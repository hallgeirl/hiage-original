#include "boundingbox.hpp"
#include "resourcemanager.hpp"
#pragma warning(push, 0)
#include <tinyxml.h>
#pragma warning(pop)

#include <json/json.hpp>

#include <sstream>
#include <iostream>

#include <fstream>
#include <streambuf>
#include <filesystem>


using namespace hiage;
using namespace std;
using json = nlohmann::json;


void TextureManager::loadResource(const std::string& path)
{
	clog << "Loading texture: " << path << endl << flush;

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

	auto resourceName = std::string(textureElement->Attribute("name"));
	bitmapElement = textureElement->FirstChildElement("bitmap");
	bitmap = bitmapElement->Attribute("path");

	loadTextureResource(bitmap, resourceName);

	clog << "OK: Texture loaded successfully.\n" << flush;
}

void TextureManager::loadTextureResource(const std::string& path, const std::string& name)
{
	clog << "Loading texture bitmap: " << path << endl << flush;

	auto& resource = initResource(name);
	resource.resource = Texture();

	clog << "- Name: " << resource.name << endl << flush;

	resource.resource.loadTexture(getResourcePath(path).c_str());

	resource.strData1 = path;
}

void SpriteManager::loadResource(const std::string& path)
{
	clog << "Loading sprite: " << path << endl << flush;

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
	auto spriteName = spriteElement->Attribute("name");
	auto& resource = initResource(spriteName);
	resource.resource = Sprite();
	auto& sprite = resource.resource;
	
	clog << "- Name: " << resource.name << endl << flush;

	//load frame dimensions
	spriteElement->Attribute("framewidth", &resource.intData1);
	spriteElement->Attribute("frameheight", &resource.intData2);
	clog << "- Frame width: " << resource.intData1 << endl << "- Frame height: " << resource.intData2 << endl << flush;

	//check for valid frame dimensions
	if ((resource.intData1 <= 0) || (resource.intData2 <= 0))
	{
		throw IOException("ERROR: Invalid frame dimensions.");
	}

	//find the texture element
	textureElement = spriteElement->FirstChildElement("texture");
	if (!textureElement)
	{
		throw IOException("ERROR: Could not find XML element <texture>.");
	}
	resource.strData1 = textureElement->Attribute("name");
	clog << "- Texture name: " << resource.strData1 << endl << flush;

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

		auto velocitySpeedupFactorAttr = animationElement->Attribute("velocitySpeedupFactor");
		double velocitySpeedupFactor = 0;
		if (velocitySpeedupFactorAttr)
		{
			animationElement->Attribute("velocitySpeedupFactor", &velocitySpeedupFactor);
		}

		i = sprite.addAnimation(animationElement->Attribute("name"), velocitySpeedupFactor);
		

		while (frameElement)
		{
			int x, y, nextFrame;
			double delay;
			std::string p_sTemp = "";

			frameElement->Attribute("x",&x);
			frameElement->Attribute("y",&y);
			frameElement->Attribute("nextframe",&nextFrame);
			frameElement->Attribute("delay",&delay);
			clog << "- Frame information:\n" << "  - x: " << x << " y: " << y << endl << "  - Next frame #: " << nextFrame << endl << "  - Frame delay: " << delay << endl << flush;

			if ((x >= 0) && (y >= 0) && (nextFrame >= 0) && (delay > 0))
			{
				sprite.addFrame(i, x, y, delay, nextFrame);
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
}

void TilesetManager::loadResource(const std::string& path)
{
	clog << "Loading tile set: " << path << endl << flush;

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
	}

	auto& resource = initResource(tilesetElement->Attribute("name"));
	clog << "- Tile set name: " << resource.name << endl << flush;
	auto& tileset = resource.resource;

	std::string texPath;
	int id, delay = 0, block, nextTile, counter = 0;

	tileElement = tilesetElement->FirstChildElement("tile");

	while (tileElement)
	{
		++counter;

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

		_textureManager.loadTextureResource(texPath, texPath);
		auto& texture = _textureManager.requestResourceRef(texPath);

        if (nextTile != -1)
        {
            tileset.addTile(&texture.resource, block, nextTile, delay, texPath, id);
        }
        else
        {
            tileset.addTile(&texture.resource, block, -1, 0, texPath, id);
        }
		tileElement = tileElement->NextSiblingElement("tile");
	}

	if (counter == 0)
	{
		clog << "Warning: Tile set is empty.\n" << flush;
	}
}

void FontManager::loadResource(const std::string& path)
{
	clog << "Loading font: " << path << endl << flush;

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
	auto& resource = initResource(fontElement->Attribute("name"));
	auto& font = resource.resource;
	fontElement->Attribute("charwidth", &resource.intData1);
	fontElement->Attribute("charheight", &resource.intData2);
	clog << "- Font name: " << resource.name << endl << flush;
	clog << "- Character width: " << resource.intData1 << ", character height: " << resource.intData2 << endl << flush;

	//check for valid character dimensions
	if ((resource.intData1 <= 0) || (resource.intData2 <= 0))
	{
		throw IOException("ERROR: Invalid character dimensions.");
	}

	//find the texture element
	textureElement = fontElement->FirstChildElement("texture");
	if (!textureElement)
	{
		throw IOException("ERROR: Could not find XML element <texture>.");
	}
    resource.strData1 = textureElement->Attribute("name");
    clog << "- Texture name: " << resource.strData1 << endl << flush;


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
    size_t charsPerRow = 0;

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
    auto rowCount = rows.size();

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
    font.setCharacterTable(charTable, (int)charsPerRow, (int)rowCount);

    for (unsigned int i = 0; i < charsPerRow; i++)
    {
        delete [] charTable[i];
    }

    delete [] charTable;

    clog << "OK: Font loaded successfully.\n" << flush;
}


std::string getPropertyName(const std::string& propertyNameRoot, const std::string& propertyKey)
{
	if (propertyNameRoot.size() == 0)
		return propertyKey;

	return propertyNameRoot + "." + propertyKey;
}

void getPropertiesRecursively(json& j, ComponentProperties& properties, const std::string& propertyNameRoot)
{
	for (auto& prop : j.items())
	{
		if (prop.value().is_object())
		{
			getPropertiesRecursively(prop.value(), properties, getPropertyName(propertyNameRoot, prop.key()));
		}
		else if (prop.value().is_string())
		{
			properties[getPropertyName(propertyNameRoot,prop.key())] = static_cast<std::string>(prop.value());
		}
		else if (prop.value().is_number())
		{
			properties[getPropertyName(propertyNameRoot, prop.key())] = static_cast<double>(prop.value());
		}
		else if (prop.value().is_array())
		{
			if (prop.value().size() > 0)
			{
				if (prop.value()[0].is_number())
				{
					std::vector<double> values;
					for (auto& v : prop.value())
					{
						values.push_back(static_cast<double>(v));
					}

					properties[getPropertyName(propertyNameRoot, prop.key())] = values;
				}
				else
				{
					std::vector<std::string> values;
					for (auto& v : prop.value())
					{
						values.push_back(v);
					}

					properties[getPropertyName(propertyNameRoot, prop.key())] = values;
				}
			}
		}
	}
}

void getPropertiesRecursively(json& j, ComponentProperties& properties)
{
	getPropertiesRecursively(j, properties, "");
}

void ObjectManager::loadResource(const std::string& path)
{
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	auto j = json::parse(str);

	auto& name = j.at("name");
	auto& type = j.at("type");

	auto& resource = initResource(name);
	auto& objectDesc = resource.resource;
	objectDesc.name = name;
	objectDesc.type = type;
	

	auto& components = j.at("components");

	for (auto& c : components)
	{
		ComponentDescriptor compDesc;

		auto& componentType = c.at("type");
		if (c.contains("properties"))
		{
			auto& properties = c.at("properties");
			getPropertiesRecursively(properties, compDesc.properties);
		}
		
		compDesc.type = componentType;
		objectDesc.components.push_back(compDesc);
	}
}

/*!
	\file com_game.h

	\author Hallgeir Lien

	Contains declaration of the Renderer class.
*/

#pragma once

#include "texture.h"
#include <vector>

namespace hiage
{
	enum class ObjectZ
	{
		//closest to the screen you get. best used for text, GUI and other stuff that should be all the way up front.
		CLOSEST = 0,
		//not as close as CLOSEST, but quite close nonetheless.
		//probably best used for stuff that needs to be behind text, but nothing else (for instance GUI frames)
		FRONT = 1,
		//probably best used for stuff that should be in front of objects, but behind text and GUI-elements like borders, for instance GUI backgrounds.
		NEAR_FRONT = 2,
		//in front of the MIDDLE objects - probably best used for objects that should be in front of objects, but behind the GUI. For instance foreground bushes on a 2d-sidescroller.
		MIDDLE_FRONT = 3,
		//in front of the background, but behind GUI and foreground.
		MIDDLE = 4,
		//behind "most" of the objects, but still in front of the actual background. Example: background clouds.
		MIDDLE_BACK = 5,
		//behind everything except the actual background. Example: Objects hiding out in clouds.
		NEAR_BACK = 6,
		//background layer.
		BACK = 7,
		//behind everything.
		FURTHEST = 8

	};

	/*!
		Abstraction for rendering. Handles rendering of everything.
	*/
	class Renderer
	{
	public:
		/*!
			Enum that describes an renderable objects Z-position relative to the others.
			This is to control what objects that should be rendered in front of what.
		*/

		static const int LAYERCOUNT = 9;

	private:
		struct Vertex
		{
			Vertex()
			{
				x = y = texX = texY = 0;
			}

			Vertex(double xpos, double ypos, double texx, double texy)
			{
				x = xpos;
				y = ypos;
				texX = texx;
				texY = texy;
			}

			double x;
			double y;

			double texX;
			double texY;
		};


		struct RenderObject
		{
			RenderObject()
			{
				texture = 0;
				zposition = ObjectZ::CLOSEST;
			}

			RenderObject(ObjectZ objz, Texture *tex)
			{
				texture = tex;
				zposition = objz;
			}

			Texture *texture;				//pointer to the texture used to render this object
			ObjectZ zposition;	//
			std::vector<Vertex> vertices;	//list of vertices (with texture coordinates) to render
		};

		//buffer of renderable objects (in seperate arrays so sorting shouldn't be neccesary)
		//std::vector<std::vector<RenderObject> > renderObjects;
		std::vector<RenderObject> renderObjects[LAYERCOUNT];

		int currentRenderObject;
		ObjectZ currentZ;
		bool recordingVertices;

	public:
		Renderer();
		~Renderer() throw ();

		/*!
			Start getting vertices for rendering.
			\param zposition The z-position to render the object in. Must be one of the constants defined in the enum ObjectZ.
			\param texture Pointer to a texture to use for rendering. Pass a null-pointer for no texture.
		*/
		void beginRender(ObjectZ zposition, Texture *texture);
		/*!
			Add a vertex to render. A multiple of 4 vertices must be added.
			\param x x-position of the vertex to be drawn.
			\param y y-position of the vertex to be drawn.
			\param texX The texture's x-coordinate for this vertex.
			\param
			\note Vertices must be added in multiples of 4. Failure to do this will result in an exception being thrown.
		*/
		void addVertex(double x, double y, double texX, double texY);

		/*!
			End rendering. This checks if a multiple of 4 vertices has been added.
		*/
		void endRender();

		/*!
			Clear the rendering buffer. It should be cleared before every frame.
		*/
		void clearBuffer();

		/*!
			Render all the objects in the buffer. It should be done after every frame.
		*/
		void renderBuffer();

	};

}

/*!
	\file $(filename)
	\author $(author)

	Contains implementation of the Vector2 class.
*/
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "../util/exceptions.h"
#include "renderer.h"

using namespace std;
using namespace hiage;

Renderer::Renderer() : currentZ(ObjectZ::CLOSEST)
{
	recordingVertices = false;
	currentRenderObject = -1;

	//CLOSEST = 7
	/*renderObjects.push_back(vector<RenderObject>());
	//FRONT = 6
	renderObjects.push_back(vector<RenderObject>());
	//NEAR_FRONT = 4
	renderObjects.push_back(vector<RenderObject>());
	//MIDDLE_FRONT = 2
	renderObjects.push_back(vector<RenderObject>());
	//MIDDLE = objects
	renderObjects.push_back(vector<RenderObject>());
	//MIDDLE_BACK = 0
	renderObjects.push_back(vector<RenderObject>());
	//NEAR_BACK = 1
	renderObjects.push_back(vector<RenderObject>());
	//BACK = 3
	renderObjects.push_back(vector<RenderObject>());
	//FURTHEST = 5
	renderObjects.push_back(vector<RenderObject>());*/
}

Renderer::~Renderer() throw ()
{

}

//start the rendering process
void Renderer::beginRender(ObjectZ zposition, Texture *texture)
{
	if (recordingVertices)
	{
		throw Exception("ERROR: Can't begin rendering: Recording. Call Renderer::endRender() before attempting to start another rendering process.");
	}

	recordingVertices = true;
	currentZ = zposition;

	renderObjects[static_cast<int>(currentZ)].push_back(RenderObject(zposition, texture));
	currentRenderObject = (int)renderObjects[static_cast<size_t>(currentZ)].size() - 1;
}

//add a vertex
void Renderer::addVertex(double x, double y, double texX, double texY)
{
	if (!recordingVertices)
	{
		throw Exception("ERROR: Can't add vertex to renderer: Not recording. Call Renderer::beginRender() before rendering.");
	}

	renderObjects[static_cast<int>(currentZ)][currentRenderObject].vertices.push_back(Vertex(x, y, texX, texY));
}

//end the rendering process
void Renderer::endRender()
{
	if (!recordingVertices)
	{
		throw Exception("ERROR: Can't end rendering: Not recording. Call Renderer::beginRender() before attempting to stop rendering.");
	}

	if (renderObjects[static_cast<int>(currentZ)][currentRenderObject].vertices.size() % 4 != 0)
	{
		throw Exception("ERROR: Can't finish rendering: Amount of vertices not a multiple of 4.");
	}

	recordingVertices = false;
}

//render the vertexes stored in this buffer
void Renderer::renderBuffer()
{
	if (recordingVertices)
	{
		throw Exception("ERROR: Can't render the vertex buffer: Recording. Call Renderer::endRender() before attempting to render the buffer.");
	}

	//TODO: Sort arrays based on texture ID.

	//render closest first, furthest last
	for (int z = 0; z < LAYERCOUNT; z++)
	{
		for (unsigned int o = 0; o < renderObjects[z].size(); o++)
		{
			//select the texture if the pointer is not null. otherwise, disable texturing.
			if (renderObjects[z][o].texture)
			{
				if (o == 0 || renderObjects[z][o-1].texture && renderObjects[z][o].texture->getTextureID() != renderObjects[z][o-1].texture->getTextureID())
				{
					renderObjects[z][o].texture->select();
				}
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
			}

			//draw vertices
			glBegin(GL_QUADS);
			for (unsigned int v = 0; v < renderObjects[z][o].vertices.size(); v++)
			{
				glTexCoord2d(renderObjects[z][o].vertices[v].texX, renderObjects[z][o].vertices[v].texY);
				glVertex2d(renderObjects[z][o].vertices[v].x, renderObjects[z][o].vertices[v].y);
			}
			glEnd();

			//restore the texturing if the texture pointer was null.
			if (!renderObjects[z][o].texture)
			{
				glEnable(GL_TEXTURE_2D);
			}
		}
	}
}

//clear the buffer
void Renderer::clearBuffer()
{
	if (recordingVertices)
	{
		throw Exception("ERROR: Can't clear the vertex buffer: Recording. Call Renderer::endRender() before attempting to clear the buffer.");
	}

	//clear all the buffers

	for (int i = 0; i < 9; i++)
	{
		renderObjects[i].clear();
	}
}


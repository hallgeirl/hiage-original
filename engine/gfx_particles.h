#pragma once

#include <list>
#include <boost/smart_ptr.hpp>

#include "com_typedefs.h"
#include "util_vector2.h"
#include "gfx_renderer.h"
#include "gfx_texture.h"


namespace Hinage
{
	typedef struct
	{
		Vector2<double> position, direction;
		double velocity, lifetime;
		int width, height;
		ubyte red, green, blue, alpha;
	} ParticleCreateData;

	//particle
	class Particle
	{
	private:
		Vector2<double> position, speed;
		double lifetime;
		int width, height;
		ubyte red, green, blue, alpha;

	public:
		Particle(ParticleCreateData &particleData);

		void update(double timefactor);
		void render(Renderer &renderer, Renderer::ObjectZ z, Texture * texture);
		bool isAlive();
	};


	//particle system wrapper
	class __IMPORTEXPORT ParticleSystem
	{
	private:
		//todo: really use boost::shared_ptr?
		typedef boost::shared_ptr<Particle> ParticlePtr;
		typedef std::list<ParticlePtr> ParticleArray;

	private:
		ParticleArray	particles;
		Texture *		texture;

	public:
		ParticleSystem();
		ParticleSystem(Texture * texture);

		void setTexture(Texture * texture);

		void addParticle(ParticleCreateData &particleData);

		void update(double timefactor);
		void render(Renderer &renderer, Renderer::ObjectZ z);
	};
}

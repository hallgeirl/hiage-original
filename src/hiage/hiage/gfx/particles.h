#pragma once
#include "../core/typedefs.h"
#include "../util/vector2.h"
#include "renderer.h"
#include "texture.h"

#include <list>


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
		typedef std::shared_ptr<Particle> ParticlePtr;
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

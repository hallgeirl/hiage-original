
#include "particles.h"

using namespace hiage;

Particle::Particle(ParticleCreateData &particleData)
{
	position = particleData.position;
	speed = particleData.direction.normalize() * particleData.velocity;
	lifetime = particleData.lifetime;
	width = particleData.width;
	height = particleData.height;
	red = particleData.red;
	green = particleData.green;
	blue = particleData.blue;
}

void Particle::update(double timefactor)
{
	position += speed * timefactor;
	lifetime -= timefactor;

	if (lifetime < 0)
		lifetime = 0;
}

bool Particle::isAlive()
{
	return (lifetime != 0);
}

void Particle::render(Renderer &renderer, ObjectZ z, Texture * texture)
{
/*	glPushMatrix();

	if (texture)
		texture->select();
	else
		glDisable(GL_TEXTURE_2D);*/

	double x = position.getX();
	double y = position.getY();
	renderer.beginRender(z, texture);

	renderer.addVertex(-(double)width / 2 + x, (double)height / 2 + y, 0, 0);
	renderer.addVertex((double)width / 2 + x, (double)height / 2 + y, 1, 0);
	renderer.addVertex((double)width / 2 + x, -(double)height / 2 + y, 1, 1);
	renderer.addVertex(-(double)width / 2 + x, -(double)height / 2 + y, 0, 1);

	renderer.endRender();
}

ParticleSystem::ParticleSystem()
{
	_texture = 0;
}

ParticleSystem::ParticleSystem(Texture * texture)
{
	this->_texture = texture;
}

void ParticleSystem::addParticle(ParticleCreateData &particleData)
{
	ParticlePtr particle(new Particle(particleData));

	_particles.push_back(particle);
}

void ParticleSystem::update(double timefactor)
{
	ParticleArray::iterator itr = _particles.begin();

	while (itr != _particles.end())
	{
		(*itr)->update(timefactor);

		if (!(*itr)->isAlive())
			itr = _particles.erase(itr);
		else
			++itr;
	}
}

void ParticleSystem::render(Renderer &renderer, ObjectZ z)
{
	ParticleArray::iterator itr = _particles.begin();

	while (itr != _particles.end())
	{
		(*itr)->render(renderer, z, _texture);

		++itr;
	}
}

void ParticleSystem::setTexture(Texture * texture)
{
	this->_texture = texture;
}

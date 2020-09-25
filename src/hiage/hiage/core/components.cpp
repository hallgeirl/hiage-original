#include "components.hpp"

#include <memory>

using namespace hiage;
using namespace std;

void PhysicalComponent::setPosition(double x, double y)
{
	position.set(x, y);
}

const Vector2<double>& PhysicalComponent::getPosition() const
{
	return position;
}

void MovableComponent::setVelocity(double xVel, double yVel)
{
	velocity.set(xVel, yVel);
}

const Vector2<double>& MovableComponent::getVelocity()
{
	return velocity;
}

void MovableComponent::accellerate(double magnitude, const Vector2<double>& directionNormalized)
{
	velocity += directionNormalized * magnitude;
}

struct B {
	virtual void bar() { std::cout << "B::bar\n"; }
	virtual ~B() = default;
};
struct D : B
{
	D() { std::cout << "D::D\n"; }
	~D() { std::cout << "D::~D\n"; }
	void bar() override { std::cout << "D::bar\n"; }
};


unique_ptr<Component> ComponentFactory::createComponent(const std::string& name) const
{
	if (name == "physical")
	{
		return make_unique<PhysicalComponent>();
	}
	else if (name == "movable")
	{
		return make_unique<MovableComponent>();
	}
	else if (name == "renderable")
	{
		return make_unique<RenderableComponent>();
	}

	throw runtime_error("Component type not found: " + name);
}
	
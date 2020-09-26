#include "components.hpp"

#include <memory>

using namespace hiage;
using namespace std;


hiage::Component::Component(int typeId) : typeId(typeId)
{
}

hiage::Component::~Component()
{
}

int hiage::Component::getTypeId()
{
	return 0;
}

hiage::PhysicalComponent::PhysicalComponent() : Component(PhysicalComponent::TYPEID)
{

}

void PhysicalComponent::setPosition(double x, double y)
{
	position.set(x, y);
}

const Vector2<double>& PhysicalComponent::getPosition() const
{
	return position;
}

hiage::MovableComponent::MovableComponent() : Component(MovableComponent::TYPEID)
{
}

void MovableComponent::setVelocity(double xVel, double yVel)
{
	velocity.set(xVel, yVel);
}

const Vector2<double>& MovableComponent::getVelocity() const
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



std::unique_ptr<Component> hiage::ComponentManager::createComponentCore(const std::string& name) const
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

shared_ptr<Component> ComponentManager::createComponent(const std::string& type)
{
	if (componentCache.find(type) == componentCache.end())
		componentCache[type] = vector<shared_ptr<Component>>();

	componentCache[type].push_back(createComponentCore(type));
	
	return componentCache[type][componentCache[type].size()-1];
}

std::vector<shared_ptr<Component>> hiage::ComponentManager::getComponentsOfType(const std::string& type)
{
	if (componentCache.find(type) == componentCache.end())
		return vector<shared_ptr<Component>>();

	return componentCache[type];
}

hiage::RenderableComponent::RenderableComponent() : Component(RenderableComponent::TYPEID)
{

}

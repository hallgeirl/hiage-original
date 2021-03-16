#include "mariocomponents.hpp"

using namespace std;
using namespace hiage;

GroundMonsterControllerComponent::GroundMonsterControllerComponent(const ComponentProperties& properties)
{
    if (properties.contains("initialDirection"))
        direction = get<std::string>(properties.at("initialDirection"));
}

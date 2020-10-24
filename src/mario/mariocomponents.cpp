#include "mariocomponents.hpp"

using namespace std;
using namespace hiage;

GroundMonsterControllerProperties GroundMonsterControllerComponent::createState(const ComponentProperties& properties)
{
    GroundMonsterControllerProperties props;

    props.direction = "left";
    if (properties.contains("initialDirection"))
        props.direction = get<std::string>(properties.at("initialDirection"));
    
    return props;
}

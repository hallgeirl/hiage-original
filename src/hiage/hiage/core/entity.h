/*
File: entity.h

Author: Hallgeir Lien

Description: Header for the most basic entity classes/interfaces:
- EntityBase (Abstract)
- RenderableEntity (Abstract)
- MovableEntity (Abstract)
- PhysicalEntity
*/

#pragma once

#include <iostream>
#include "../gfx/sprite.h"
#include "typedefs.h"
#include "../util/vector2.h"
#include "../gfx/tilemap.h"

namespace Hinage
{
	//base entity interface
	class EntityBase
	{
	public:
		virtual ~EntityBase() {}
		virtual void setDestructionFlag(bool val) = 0;

		virtual bool getDestructionFlag() const = 0;
	};

	//interface for movable objects
	class MovableEntity
	{
	public:
		virtual					~MovableEntity() {}

		virtual void 			accellerate(double magnitude, const Vector2<double> direction) 	= 0;
		virtual void 			setVelocity(const Vector2<double> newVelocity)  				= 0;
		virtual Vector2<double> getVelocity() const												= 0;
		virtual void 			setMaxSpeed(double magnitude) 									= 0;
		virtual void 			update(double frameTime) 										= 0;
	};

	//interface for visible objects (:P)
	class RenderableEntity
	{
	public:
		virtual ~RenderableEntity() {}

		virtual void render(Renderer &renderer, Renderer::ObjectZ z) = 0;
	};

	//physical object, with position and everything. base for all in-game objects.
	class PhysicalEntity : public EntityBase, RenderableEntity, MovableEntity
	{
	private:
		bool destroyFlag;

	protected:
		Vector2<double>	position;   //current position
		Vector2<double> velocity;      //current speed
		Sprite *		sprite;


		bool 				hFlip, vFlip;	//flip sprite?
		bool 				canCollide;		//true if the entity can collide with other entities or the world
		double              frameTime;      //time used last frame
		std::string         entityName;     //name of the object
		std::string         entityType;

        struct TileCollisionData
        {
            TileCollisionData(Vector2<int> tp, Vector2<double> cp)
            {
                tilepos = tp; colpos = cp;
            }
            Vector2<int> tilepos;
            Vector2<double> colpos;
        };

    public:
		//script containers
		std::vector<std::string> initScripts;
		std::vector<std::string> updateScripts;
		std::vector<std::string> shutdownScripts;
		std::vector<std::string> collisionScripts;  //collision vs tilemap
		std::vector<std::string> objectCollisionScripts; //collision vs another object

		bool                collided;
		bool                collidedWithMap;
		std::vector<TileCollisionData> collidedTiles;

	public:
                                PhysicalEntity();
		virtual 			    ~PhysicalEntity();

		void 				    createFromFile(std::string path, Sprite * sprite);

		//EntityBase
		virtual void 		    setDestructionFlag(bool val);
		virtual bool 		    getDestructionFlag() const;

		//RenderableEntity
		virtual void 		    render(Renderer &renderer, Renderer::ObjectZ z);

		void 				    setSprite(Sprite * sprite);
		const Sprite * 		    getSprite() const;

		//MovableEntity
		virtual void 			accellerate(double magnitude, const Vector2<double> direction);
		virtual void 			setVelocity(const Vector2<double> newSpeed);
		virtual Vector2<double> getVelocity() const;
		virtual void 			setMaxSpeed(double magnitude);
		virtual void 			update(double frameTime);
        //end of base class implementations

        //various methods to get/set position of the entity
		void 				    setPosition(double x, double y);
		void 				    setPosition(Vector2<double> position);
		void                    setX(double x);
		void                    setY(double y);
		Vector2<double>		    getPosition() const;
		double                  getX();
		double                  getY();

        //various methods to get/set the speed of the entity (overloading the ones above)
        void                    accellerate(double magnitude, double xdir, double ydir);
        void                    setVelocity(double x, double y);
        void                    setXVelocity(double x);
        void                    setYVelocity(double y);
        double                  getXVelocity();
        double                  getYVelocity();
        double                  getSpeed(); //get the absolute value of the speed vector

        //get the collision borders
        int                     getHitboxLeft();
        int                     getHitboxRight();
        int                     getHitboxTop();
        int                     getHitboxBottom();
		//check for collision
		bool                    willCollide(PhysicalEntity &target, double frameTime, Vector2<double> &colPos1, Vector2<double> &colPos2);
		bool                    willCollideWithMap(Tilemap &tilemap, double frameTime);
		//turn on/off collision detection
		void                    setCollision(bool value);

        void                    flipx(bool value);
        void                    flipy(bool value);



        //play animation
        bool                    playAnimation(std::string animationName, bool resetIfRunning, double animationSpeed);

        //get name of entity
        std::string             getName();
        std::string             getType();


	};
}

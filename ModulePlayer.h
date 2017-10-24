#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "p2SString.h"
#include "ModuleTextures.h"
#include "Box2D/Box2D/Box2D.h"
#include <string>

class PhysBody;

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

private:
	//Function to create both flippers
	void CreateFlippers();

	//Function to create ball launcher
	void CreateLauncher();

	//Function to create ball
	void CreateBall();
	
public:

	SDL_Texture* ball_tex;
	SDL_Texture* flipper_tex;
	
	uint score = 0;
	uint tries = 3;

private:

	PhysBody* flipperL;
	PhysBody* pivotL;
	b2RevoluteJoint* jointL;

	PhysBody* flipperR;
	PhysBody* pivotR;
	b2RevoluteJoint* jointR;

	PhysBody* launcher;
	PhysBody* launcher_pivot;
	b2PrismaticJoint* jointLauncher;

	PhysBody* ball;

	p2SString* tmp = new p2SString("[POKEMON PINBALL] | SCORE: %d", score);

};
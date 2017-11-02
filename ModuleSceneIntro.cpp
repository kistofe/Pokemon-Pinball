#include "Globals.h"
#include "Application.h"
#include "Animation.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"
#include "ModuleWindow.h"

#include "SDL\include\SDL.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	pinball_tex = dugtrio_tex = pikachu_tex = starmie_tex = panel_bor_tex = panel_tex = starmie2_tex = lighted_bouncer_tex = blue_light_tex = NULL;

	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	//Loading music
	App->audio->PlayMusic("pinball/audio/Themes/Field_Theme.ogg");
	
	//Loading Sfx
	bonus_fx				= App->audio->LoadFx("pinball/audio/Sfx/bonus.wav");
	lose_ball_fx			= App->audio->LoadFx("pinball/audio/Sfx/Lose ball.wav");
	lose_fx					= App->audio->LoadFx("pinball/audio/Sfx/Lose.wav");

	App->renderer->camera.x = App->renderer->camera.y = 0;
	
	//Loading Textures
	pinball_tex			= App->textures->Load("pinball/images/Pinball.png");
	dugtrio_tex			= App->textures->Load("pinball/images/dugtrio.png");
	pikachu_tex			= App->textures->Load("pinball/images/pikachu.png");
	starmie_tex			= App->textures->Load("pinball/images/starmie.png");
	starmie2_tex		= App->textures->Load("pinball/images/starmie2.png");
	panel_bor_tex		= App->textures->Load("pinball/images/border.png");
	panel_tex			= App->textures->Load("pinball/images/Panel.png");
	blue_light_tex		= App->textures->Load("pinball/images/blue_light.png");
	red_light_tex		= App->textures->Load("pinball/images/red_light.png");
	light_off_tex		= App->textures->Load("pinball/images/light_off.png");
	lighted_bouncer_tex = App->textures->Load("pinball/images/lighted bouncer.png");

	//Adding Animations
	AddSceneAnimations();
		
	//Create every scenario part
	App->physics->AddPinballParts();
	AddBouncers();

	//Calling function to create sensors
	AddSensors();

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	
	//Unloading Textures
	App->textures->Unload(pinball_tex);
	App->textures->Unload(dugtrio_tex);
	App->textures->Unload(pikachu_tex);
	App->textures->Unload(starmie_tex);
	App->textures->Unload(starmie2_tex);
	App->textures->Unload(panel_bor_tex);
	App->textures->Unload(panel_tex);
	App->textures->Unload(lighted_bouncer_tex);


	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	
	// All draw functions ------------------------------------------------------
	
	//BLIT TEXTURES --------------------------------------------

	//Pinball Textures
	App->renderer->Blit(pinball_tex, 0, 0, NULL);

	//Lighted Bouncer Textures
	if (App->player->has_hitted_bouncer[0] == true)
		App->renderer->Blit(lighted_bouncer_tex, 190, 250, NULL);
	
	if (App->player->has_hitted_bouncer[1] == true)
		App->renderer->Blit(lighted_bouncer_tex, 270, 220, NULL);
	
	if (App->player->has_hitted_bouncer[2] == true)
		App->renderer->Blit(lighted_bouncer_tex, 250, 308, NULL);
	
	//----------------------------------------------------------

	// BLIT ANIMATIONS -------------------------------------------

	//Right dugtrio animation
	App->renderer->Blit(dugtrio_tex, 411, 500, &(Dugtrio_right.GetCurrentFrame()));

	//Left dugtrio animation
	App->renderer->Blit(dugtrio_tex, 0, 500, &(Dugtrio_left.GetCurrentFrame()));

	//Pikachu animation
	App->renderer->Blit(pikachu_tex, 20, 780, &(Pikachu.GetCurrentFrame()));
	App->renderer->Blit(pikachu_tex, 420, 780, &(Pikachu.GetCurrentFrame()));

	//Starmie animation
	App->renderer->Blit(starmie_tex, 0, 450, &(Starmie.GetCurrentFrame()));

	//Panel Border Animation
	App->renderer->Blit(panel_bor_tex, 168, 498, &(panel_border.GetCurrentFrame()));

	//Panel Animation
	App->renderer->Blit(panel_tex, 171, 501, &(panel.GetCurrentFrame()));

	//Second Starmie animation
	App->renderer->Blit(starmie2_tex, 133, 315, &(Starmie2.GetCurrentFrame()));

	// --------------------------------------------------------------

	//Check Ball collision to animate scenario ----------------------

	// Blue lights beside the flippers

	if (round_lights_on[0])
		App->renderer->Blit(blue_light_tex, 30, 618, NULL);
	else if (!round_lights_on[0])
		App->renderer->Blit(light_off_tex, 30, 618, NULL);
	if (round_lights_on[1])
		App->renderer->Blit(blue_light_tex, 78, 618, NULL);
	else if (!round_lights_on[1])
		App->renderer->Blit(light_off_tex, 78, 618, NULL);
	if (round_lights_on[2])
		App->renderer->Blit(blue_light_tex, 381, 618, NULL);
	else if (!round_lights_on[2])
		App->renderer->Blit(light_off_tex,381, 618, NULL);
	if (round_lights_on[3])
		App->renderer->Blit(blue_light_tex, 429, 618, NULL);
	else if (!round_lights_on[3])
		App->renderer->Blit(light_off_tex, 429, 618, NULL);
	if (round_lights_on[4])
		App->renderer->Blit(red_light_tex, 153, 339, NULL);
	else if (!round_lights_on[4])
		App->renderer->Blit(light_off_tex, 153, 339, NULL);
	
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::AddBouncers()
{
	bouncers[0] = App->physics->CreateCircle(198, 257, 27, b2_staticBody, 1.5f, 20);
	bouncers[1] = App->physics->CreateCircle(274, 227, 27, b2_staticBody, 1.5f, 20);
	bouncers[2] = App->physics->CreateCircle(255, 312, 27, b2_staticBody, 1.5f, 20);
}

void ModuleSceneIntro::AddSensors()
{
	// Creating Dying Sensor
	dying_sensor			= App->physics->CreateRectangleSensor(0, SCREEN_HEIGHT + 50, SCREEN_WIDTH, 50);

	// -------------------------------------------------------------------------------
	
	// Creating Animation Sensors

	//Three sensors at the right
	arrow_sensor_right1		= App->physics->CreateRectangleSensor(350, 515, 40, 43, 90.075);
	arrow_sensor_right2		= App->physics->CreateRectangleSensor(367, 485, 40, 43, 90.075);
	arrow_sensor_right3		= App->physics->CreateRectangleSensor(384, 450, 40, 43, 90.075);

	//Voltorb sensors
	voltorb_sensor[0]		= App->physics->CreateCircleSensor(198, 256, 29);
	voltorb_sensor[1]		= App->physics->CreateCircleSensor(274, 227, 29);
	voltorb_sensor[2]		= App->physics->CreateCircleSensor(255, 312, 29);

	//Blue lights

	roundlights[0] = App->physics->CreateCircleSensor(30, 618, PIXEL_TO_METERS(21), 50);
	roundlights[1] = App->physics->CreateCircleSensor(78, 618, PIXEL_TO_METERS(21), 50);
	roundlights[2] = App->physics->CreateCircleSensor(381, 618, PIXEL_TO_METERS(21), 50);
	roundlights[3] = App->physics->CreateCircleSensor(429, 618, PIXEL_TO_METERS(21), 50);
	roundlights[4] = App->physics->CreateCircleSensor(105, 332, PIXEL_TO_METERS(21), 30);

	// ---------------------------------------------------------------------------------


	//Creating other Sensors

	//Diglett Sensors
	diglett_sensor1 = App->physics->CreateCircleSensor(85, 550, 25, 30);
	diglett_sensor2 = App->physics->CreateCircleSensor(395, 550, 25, 30);

	// ---------------------------------------------------------------------------------

	//Ball catcher sensor
	ball_catcher = App->physics->CreateCircleSensor(50, 57, 25, 70);

	// --------------------------------------------------------------------------------
}

void ModuleSceneIntro::AddSceneAnimations()
{
	//Right Dugtrio Animation
	Dugtrio_right.PushBack({   0, 0, 69, 90 });
	Dugtrio_right.PushBack({  75, 0, 69, 90 });
	Dugtrio_right.PushBack({ 150, 0, 69, 90 });
	Dugtrio_right.PushBack({ 225, 0, 69, 90 });
	Dugtrio_right.loop = true;
	Dugtrio_right.speed = 0.05f;

	//Left Dugtrio Animation
	Dugtrio_left.PushBack({   0, 120, 69, 90 });
	Dugtrio_left.PushBack({  75, 120, 69, 90 });
	Dugtrio_left.PushBack({ 150, 120, 69, 90 });
	Dugtrio_left.PushBack({ 225, 120, 69, 90 });
	Dugtrio_left.loop = true;
	Dugtrio_left.speed = 0.05f;

	//Pikachu Animation
	Pikachu.PushBack({   0, 0, 48, 48 });
	Pikachu.PushBack({  54, 0, 42, 48 });
	Pikachu.PushBack({ 102, 6, 45, 42 });
	Pikachu.loop = true;
	Pikachu.speed = 0.05f;

	//Starmie Animation
	Starmie.PushBack({ 0, 0, 54, 48 });
	Starmie.PushBack({ 60, 0, 54, 48 });
	Starmie.loop = true;
	Starmie.speed = 0.01f;

	//Panel Border Animation
	panel_border.PushBack({  0,  0, 144, 96 });
	panel_border.PushBack({ 154, 0, 144, 96 });
	panel_border.PushBack({ 304, 0, 144, 96 });
	panel_border.loop = true;
	panel_border.speed = 0.015f;

	//Panel Animation
	panel.PushBack({   0,   0, 138, 90 }); 
	panel.PushBack({   0, 100, 138, 90 });
	panel.PushBack({   0, 200, 138, 90 });
	panel.PushBack({   0, 300, 138, 90 });
	panel.PushBack({   0, 400, 138, 90 });
	panel.PushBack({   0, 500, 138, 90 });
	panel.PushBack({ 148,   0, 138, 90 });
	panel.PushBack({ 148, 100, 138, 90 });
	panel.PushBack({ 148, 200, 138, 90 });
	panel.PushBack({ 148, 300, 138, 90 });
	panel.PushBack({ 148, 400, 138, 90 });
	panel.PushBack({ 148, 500, 138, 90 });
	panel.PushBack({ 296,   0, 138, 90 });
	panel.PushBack({ 296, 100, 138, 90 });
	panel.PushBack({ 296, 200, 138, 90 });
	panel.PushBack({ 296, 300, 138, 90 });
	panel.PushBack({ 296, 400, 138, 90 });
	panel.PushBack({ 296, 500, 138, 90 });
	panel.PushBack({ 444,   0, 138, 90 });
	panel.PushBack({ 444, 100, 138, 90 });
	panel.PushBack({ 444, 200, 138, 90 });
	panel.PushBack({ 444, 300, 138, 90 });
	panel.PushBack({ 444, 400, 138, 90 });
	panel.PushBack({ 444, 500, 138, 90 });
	panel.loop = true;
	panel.speed = 0.0025f; 

	//Second Starmie animation
	Starmie2.PushBack({   0, 0, 63, 69 });
	Starmie2.PushBack({  69, 0, 63, 69 });
	Starmie2.loop = true;
	Starmie2.speed = 0.05f;
	
}


#pragma once

#include <Core/World.h>
#include <Physics/Physics2D.h>
#include <Graphics/Render.h>
#include "../Gameplay/ParticleSystem.h"
#include "../Gameplay/BlasteroidsAssets.h"

class SpaceshipIcon : public igad::Entity
{
public:
	SpaceshipIcon(igad::World& world);

	virtual ~SpaceshipIcon();

	void Init(int player, int joystick);

	virtual void  Update(float dt)  override;

	int GetPlayer() const { return _player; }

private:

	// void SetButtonMapping();

	// void GetInput(igad::Vector2& direction, float& thrust, bool& shoot, bool& push);

	int							_player						= -1;
	int							_joystick					= -1;
	igad::Color					_color						= igad::Color::White;
	igad::Transform*			_transform					= nullptr;
	igad::MeshRenderer*			_renderer					= nullptr;
	igad::MeshRenderer*			_markingsRenderer			= nullptr;
	ParticleSystem*				_particleSystem				= nullptr;
	int							_particlesPerSecond			= 55;
	float						_particleTime				= 0.0;
	
	// Button mapping
	int _upButton;
	int _downButton;
	int _leftButon;
	int _rightButton;
	int _shootButton;
	int _thrustButton;
};

class ScoreScreen : public igad::World
{
public:

	ScoreScreen();

	virtual void Update(float dt) override;

	virtual void Render() override;

	virtual bool IsDone() override;

	const std::vector<igad::Color> PlayerColors =
	{
		igad::Color("ec0b8b"),
		igad::Color("2cdecf"),
		igad::Color("f07e09"),
		igad::Color("f400ff")
	};


protected:
	igad::PhysicsManager2D*		_physicsManager2D = nullptr;
	igad::RenderManager*		_renderManager = nullptr;
	float						_timer;
	ParticleSystem*				_particleSystem = nullptr;
	BlasteroidsAssets*			_assets = nullptr;
};

#pragma once

#include <Core/Entity.h>
#include <Core/Transform.h>
#include <Physics/Physics2D.h>
#include <Graphics/MeshRenderer.h>
#include "Wrapper.h"
#include "ParticleSystem.h"
#include "Push.h"

class Blasteroids;

class InfoBar : public igad::Entity
{
public:
	InfoBar(igad::World& world);

	virtual ~InfoBar();

	void Init(int player);

	void UpdateInfo(float health, float push, float ammo, bool special);

protected:
	igad::Color					_color		= igad::Color::White;
	igad::Transform*			_transform	= nullptr;
	igad::MeshRenderer*			_renderer	= nullptr;
	igad::Mesh*					_mesh		= nullptr;
};

class SpaceShip : public igad::Entity
{
public:

	SpaceShip(igad::World& world);

	virtual ~SpaceShip();

	void Init(int player, int joystick);

	virtual void  Update(float dt)  override;

	virtual void PostUpdate(float dt) override;

	virtual void OnCollision(igad::Collision2D& collision, int body) override;

	int GetPlayer() const { return _player; }

	void AddHealth(float h) { _health += h; }

	void AddAmmo(int ammo) { _ammo += ammo; if (_ammo > 12) _ammo = 12; }

	void GenerateTurningTorque(igad::Vector2 direction, igad::Vector2 forward);

private:

	void SetButtonMapping();

	void GetInput(igad::Vector2& direction, float& thrust, bool& shoot, bool& push);

	int							_player						= -1;
	int							_joystick					= -1;
	bool						_shot						= false;
	int							_ammo						= 10;
	float						_health						= 100.0f;
	float						_pushTimer					= 0.0f;
	igad::Color					_color						= igad::Color::White;
	igad::Transform*			_transform					= nullptr;
	igad::MeshRenderer*			_renderer					= nullptr;
	igad::MeshRenderer*			_markingsRenderer			= nullptr;
	igad::PhysicsBody2D*		_physicsBody				= nullptr;
	Wrapper*					_wrapper					= nullptr;
	ParticleSystem*				_particleSystem				= nullptr;
	igad::Light*				_light						= nullptr;
	int							_particlesPerSecond			= 55;
	float						_particleTime				= 0.0;
	Blasteroids*				_blasteroidsWorld			= nullptr;
	InfoBar*					_infoBar					= nullptr;

	// Button mapping
	int _upButton;
	int _downButton;
	int _leftButon;
	int _rightButton;
	int _shootButton;
	int _thrustButton;
};
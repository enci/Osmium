#pragma once

#include <Core/World.h>
#include <Physics/Physics2D.h>
#include <Graphics/Render.h>

#include "BlasteroidsCamera.h"
#include "ParticleSystem.h"
#include "BlasteroidsAssets.h"


class Blasteroids : public igad::World
{
public:

	Blasteroids();

	virtual void Update(float dt) override;

	virtual void Render() override;

	ParticleSystem* GetParticles();

	BlasteroidsCamera* GetCamera() const { return _camera; }

	virtual bool IsDone() override;

	void CreateAmmo();

	//void AddParticles();

	static std::vector<int> _score;
	void CreateAsteroidsLevel();
	void CreateOrangeLevel();
	void CreateCrystalsLevel();
	static int _players;


public:

	const std::vector<igad::Color> PlayerColors =
	{
		igad::Color("ec0b8b"),
		igad::Color("2cdecf"),
		igad::Color("f07e09"),
		igad::Color("f400ff")
	};

	const float WorldWidth;
	const float WorldHeight;
	const float WorldDepth;

protected:
	void CrateDust();
	void CreateStars();
	//void CreateAmmo();
	void UpdateRenderFlags();

	igad::PhysicsManager2D*		_physicsManager2D	= nullptr;
	igad::RenderManager*		_renderManager		= nullptr;
	BlasteroidsCamera*			_camera				= nullptr;
	ParticleSystem*				_particleSystem		= nullptr;
	BlasteroidsAssets*			_assets				= nullptr;
	bool						_renderScene		= true;
	bool						_debugRender		= false;
	bool						_isover				= false;
	igad::Color					_background			= igad::Color::White;
	static int					_count;
};

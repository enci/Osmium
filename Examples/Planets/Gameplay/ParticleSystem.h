#pragma once
#include <Graphics/Render.h>
#include <Graphics/MeshRenderer.h>

struct ParticleSimulation
{
	float			Age;
	float			OneOverLifeSpan;
	igad::Vector3	Velocity;
	float			Damping;
	float			BeginSize;
	float			EndSize;
	igad::Color     BeginColor;
	igad::Color     EndColor;
	float			RotationSpeed;
};

struct ParticleRendering
{
	igad::Vector3	Position;
	float			Size;
	igad::Color		Color;
	igad::Vector3	Axis;
	float			Rotation;
};



class ParticleSystem : igad::Renderable
{
	const int kMaxDirecationalLights = 5;
	const int kMaxPointLights = 5;

public:
	ParticleSystem(igad::Entity& entity);
	~ParticleSystem();

	void Init(int maxParticles);

	virtual void Update(float dt);

	virtual void Draw() override;

	virtual void AddParticle(	const igad::Vector3& pos,
								const igad::Vector3& vel,
								const igad::Vector3& axis,
								float rotationSpeed,
								const igad::Color& beginColor,
								const igad::Color& endColor,
								float beginSize,
								float endSize,
								float lifeSpan,
								float damping = 0.02f);

	virtual void SetShader(igad::Shader* shader) override;

	virtual	void ActivateShader(
		const igad::Matrix44& view,
		const igad::Matrix44& proj,
		const std::vector<igad::Light*> lights) override;

private:

	int						_count		= 0;
	std::vector<ParticleSimulation>	_particleSimData;
	std::vector<ParticleRendering>	_particleRendData;
	GLuint					_vao = 0;
	GLuint					_vbo = 0;
	igad::ShaderParameter* _directionaLightsCountParam = nullptr;
	igad::ShaderParameter* _pointLightsCountParam = nullptr;
	igad::ShaderParameter* _modelViewProjParam = nullptr;	
	std::vector<std::unique_ptr<igad::LightShaderParameter>> _dirLightParams;
	std::vector<std::unique_ptr<igad::LightShaderParameter>> _pointLightParams;
};

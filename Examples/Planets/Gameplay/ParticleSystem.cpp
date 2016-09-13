
#include "ParticleSystem.h"

using namespace std;
using namespace igad;

ParticleSystem::ParticleSystem(Entity& entity) : Renderable(entity)
{
}

ParticleSystem::~ParticleSystem()
{
	if(_vao != 0)
	{
		glDeleteBuffers(1, &_vao);
		_vao = 0;
	}

	if (_vbo != 0)
	{
		glDeleteBuffers(1, &_vbo);
		_vbo = 0;
	}
}

void ParticleSystem::Init(int maxParticles)
{
	_particleSimData.resize(maxParticles);
	_particleRendData.resize(maxParticles);
}

void ParticleSystem::Update(float dt)
{
	for (size_t i = 0; i < _count; i++)
	{
		ParticleSimulation& ps = _particleSimData[i];
		ParticleRendering& pr = _particleRendData[i];
		pr.Position += ps.Velocity * dt;
		ps.Velocity -= ps.Velocity * (ps.Damping * dt);
		ps.Age += dt * ps.OneOverLifeSpan;
		pr.Rotation += dt * ps.RotationSpeed;
		pr.Color = Lerp(ps.BeginColor, ps.EndColor, ps.Age);
		pr.Size = Lerp(ps.BeginSize, ps.EndSize, ps.Age);
		if(ps.Age > 1)
		{
			swap(_particleSimData[i], _particleSimData[_count]);
			swap(_particleRendData[i], _particleRendData[_count]);
			_count--;
			i--;
		}
	}
}

void ParticleSystem::Draw()
{
	glBindVertexArray(_vao);

	if(_count > 0)
	{		
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		float *ptr = (float*)&_particleRendData[0].Position;
		glBufferSubData(GL_ARRAY_BUFFER, 0, _count * sizeof(ParticleRendering), ptr);	
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_POINTS, 0, _count);		
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::AddParticle(	const Vector3& pos,
									const Vector3& vel,
									const igad::Vector3& axis,
									float rotationSpeed,
									const Color& beginColor,
									const Color& endColor,
									float beginSize,
									float endSize,
									float lifeSpan,
									float damping)
{
	if(_count < _particleSimData.size() - 1)
	{
		ParticleSimulation& ps = _particleSimData[_count];
		ParticleRendering& pr = _particleRendData[_count];

		pr.Position = pos;
		ps.Velocity = vel;
		ps.Age = 0.0f;
		ps.OneOverLifeSpan = 1.0f / lifeSpan;
		ps.BeginColor = beginColor;
		ps.EndColor = endColor;
		pr.Color = beginColor;
		ps.BeginSize = beginSize;
		ps.EndSize = endSize;
		pr.Size = beginSize;
		ps.Damping = damping;
		pr.Axis = axis;
		ps.RotationSpeed = rotationSpeed;
		pr.Rotation = RandInRange(0.0f, igad::TwoPi);

		_count++;
	}
}

void ParticleSystem::SetShader(igad::Shader* shader)
{
	_shader = shader;

	ShaderAttribute* positionAttr = shader->GetAttribute("a_position");
	ShaderAttribute* sizeAttr = shader->GetAttribute("a_size");
	ShaderAttribute* colorAttr = shader->GetAttribute("a_color");
	ShaderAttribute* axisAttr = shader->GetAttribute("a_axis");
	ShaderAttribute* rotationAttr = shader->GetAttribute("a_rotation");
	_modelViewProjParam = shader->GetParameter("u_modelViewProjection");

	_directionaLightsCountParam = shader->GetParameter("u_directionalLightsCount");
	_pointLightsCountParam = shader->GetParameter("u_pointLightsCount");
	
	for (size_t i = 0; i < kMaxDirecationalLights; i++)
	{
		string name = "u_directionalLights[" + to_string(i) + "]";
		auto lprm = new LightShaderParameter(_shader, name);
		_dirLightParams.push_back(unique_ptr<LightShaderParameter>(lprm));
	}

	for (size_t i = 0; i < kMaxPointLights; i++)
	{
		string name = "u_pointLights[" + to_string(i) + "]";
		auto lprm = new LightShaderParameter(_shader, name);
		_pointLightParams.push_back(unique_ptr<LightShaderParameter>(lprm));
	}


	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	float *ptr = (float*)&_particleRendData[0].Position;
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleRendering) * _particleSimData.size(), ptr, GL_STREAM_DRAW);


	positionAttr->SetAttributePointer(	3, GL_FLOAT, GL_FALSE, sizeof(ParticleRendering),
										(void*)offsetof(ParticleRendering, Position));

	colorAttr->SetAttributePointer(	4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ParticleRendering),
									(void*)offsetof(ParticleRendering, Color));

	sizeAttr->SetAttributePointer(	1, GL_FLOAT, GL_FALSE, sizeof(ParticleRendering),
									(void*)offsetof(ParticleRendering, Size));

	axisAttr->SetAttributePointer(	3, GL_FLOAT, GL_FALSE, sizeof(ParticleRendering),
									(void*)offsetof(ParticleRendering, Axis));

	rotationAttr->SetAttributePointer(	1, GL_FLOAT, GL_FALSE, sizeof(ParticleRendering),
										(void*)offsetof(ParticleRendering, Rotation));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::ActivateShader(const Matrix44& view,
									const Matrix44& proj,
									const vector<Light*> lights)
{
	_shader->Activate();
	_modelViewProjParam->SetValue(proj * view);

	int pointLightsCount = 0;
	int dirLightsCount = 0;
	size_t maxDir = _dirLightParams.size();
	size_t maxPoint = _pointLightParams.size();
	for (auto l : lights)
	{
		if (l->GetLightType() == Light::DIRECTIONAL_LIGHT && dirLightsCount < maxDir)
		{
			_dirLightParams[dirLightsCount++]->SetValue(*l);
		}
		else if (l->GetLightType() == Light::POINT_LIGHT && pointLightsCount < maxPoint)
		{
			_pointLightParams[pointLightsCount++]->SetValue(*l);
		}
	}

	_directionaLightsCountParam->SetValue(dirLightsCount);
	_pointLightsCountParam->SetValue(pointLightsCount);
}
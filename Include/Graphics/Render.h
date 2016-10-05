#pragma once

#include <Core/Component.h>
#include <Core/Entity.h>
#include <Core/World.h>
#include <Graphics/Shader.h>
#include <Graphics/Color.h>
#include <Core/Transform.h>

namespace Osm
{

class Shader;
class MeshRenderer;
class Renderable;
class Light;
class Camera;
class Transform;

///
/// RenderManager
///
class RenderManager : public Component<World>
{
public:

	RenderManager(World& world) : Component(world) {}

	void Render();

	void Add(Renderable* renderable);

	void Add(Light* light);

	void Add(Camera* camera);

	void Remove(Renderable* renderable);

	void Remove(Light* light);

	void Remove(Camera* camera);

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif


protected:

	std::vector<Renderable*>	_renderables;
	std::vector<Light*>			_lights;
	std::vector<Camera*>		_cameras;
};

///
/// RenderManagerComponent
/// A small utility to make sure object add/remove themselves from
/// the manager.
///
template<class T>
class RenderManagerComponent : public Component<Entity>
{
public:

	RenderManagerComponent(Entity& entity) : Component<Entity>(entity)
	{
		auto rmng = _owner.GetWorld().GetComponent<RenderManager>();
		ASSERT(rmng);
		rmng->Add(static_cast<T*>(this));
	}

	virtual ~RenderManagerComponent()
	{
		auto rmng = _owner.GetWorld().GetComponent<RenderManager>();
		ASSERT(rmng);
		rmng->Remove(static_cast<T*>(this));
	}
};

///
/// Renderable
/// A base class for evertying that can be rendered with the
/// render manager.
///
class Renderable : public RenderManagerComponent<Renderable>
{
public:
	/// Creates a new renderable and adds it to the render manager. 
	/// When deletes it will automatically get removed.
	Renderable(Entity& entity);

	/// Get the shader used to render this object
	Shader* GetShader()	{ return _shader; }

	/// Any logic specific to setting a shader, like setting
	/// VAOs, attributes and such, goes in this method 
	virtual void SetShader(Shader* shader) = 0;

	/// Gets called to activate a shader. Will not get called
	/// for every Renderable, but only when switching shaders
	virtual	void ActivateShader(
		const Matrix44& view,
		const Matrix44& proj,
		const std::vector<Light*> lights) = 0;

	/// Used for the actual draw-call. Will get called
	/// for every Renderable 
	virtual void Draw() = 0;

protected:
	Shader* _shader = nullptr;
};

///
/// Camera
///
class Camera : public RenderManagerComponent<Camera>
{
public:

	Camera(Entity& entity);

	/// Get the projection matrix
	const Matrix44& Projection() const { return _projection; }

	/// Set the projection matrix
	void SetProjection(const Matrix44& p) { _projection = p; }

	/// Get the view matrix
	Matrix44 GetView();

	/// Set the view matrix. This will affect the trasform of the Camera
	void SetView(Matrix44 view);

protected:

	Matrix44 _projection;
	Transform* _transform = nullptr;
};

///
/// Light
///
class Light : public RenderManagerComponent<Light>
{
public:
	enum LightType
	{
		DIRECTIONAL_LIGHT,
		POINT_LIGHT
	};

public:

	Light(Entity& entity);

	LightType GetLightType() const { return _lightType; }

	void SetLightType(LightType lightType) { _lightType = lightType; }

	Color GetColor() const { return _color; }

	void SetColor(Color color) { _color = color; }

	float GetAttenuation() const { return _attenuation; }

	void SetAttenuation(float attenuation) { _attenuation = attenuation; }

	Vector3 GetDirection() const { return _transform->GetTransform().GetZAxis(); }

	Vector3 GetPosition() const { return _transform->GetTransform().GetTranslation(); }

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif

protected:

	LightType	_lightType		= DIRECTIONAL_LIGHT;
	Color		_color			= Color::White;
	float		_attenuation	= 0.5f;
	Transform* _transform		= nullptr;
};

}
#pragma once

#include <Core/Component.h>
#include <Core/Entity.h>
#include <Core/World.h>
#include <Graphics/Shader.h>
#include <Graphics/Color.h>
#include <Core/Transform.h>
#include <Graphics/Mesh.h>

namespace Osm
{

class Shader;
class MeshRenderer;
class Renderable;
class Light;
class Camera;
class Transform;
//class FullScreenPass;

///
/// RenderManager
///
class RenderManager : public Component<World>
{
public:

	RenderManager(World& world);

	void Render();

	void Add(Renderable* renderable);

	void Add(Light* light);

	void Add(Camera* camera);

	void Remove(Renderable* renderable);

	void Remove(Light* light);

	void Remove(Camera* camera);

#ifdef INSPECTOR
	virtual void Inspect() override;

	int DrawCalls = 0;
	int ShaderSwitches = 0;
#endif

protected:

	std::vector<Renderable*>	_renderables;
	std::vector<Light*>			_lights;
	std::vector<Camera*>		_cameras;

	GLuint						_framebuffer;
	GLuint						_colorbuffer;
	GLuint						_depthbuffer;
	GLuint						depthMapFBO;
	GLuint						depthMap;

	Shader*						_fullScreenPass	= nullptr;
	Shader*						_bloomShader	= nullptr;
	Shader*						_FXAAShader		= nullptr;
	Shader*						_shadowPass		= nullptr;	
	
#ifdef 	INSPECTOR
	enum
	{
		GEOMETRY_PASS = 0,
		LIGHT_PASS,
		SSAO_PASS,
		BLOOM_PASS,
		FORWARD_PASS,
		SHADOW_PASS,
		RENDER_PASSES_NUM
	} RenderPasses;
	GLuint	_queries[RENDER_PASSES_NUM];
	GLuint	_queryResults[RENDER_PASSES_NUM];
	bool	_firstFrame = true;
#endif
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
		Camera* camera,
		const std::vector<Light*> lights) = 0;

	/// Used for the actual draw-call. Will get called
	/// for every Renderable 
	virtual void Draw() = 0;

	/// Used for the actual draw-call. Will get called
	/// for every Renderable 
	virtual void DrawDepth(Matrix44 viewProjection) = 0;

	/// Check if this renderable casts a shadow
	bool GetShadowCasting() const { return _castShadow; }

	/// Set if this renderable should cast a shadow
	void SetShadowCasting(bool castShadow) { _castShadow = castShadow; }

protected:
	Shader* _shader			= nullptr;
	bool	_castShadow		= true;
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

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif

	float GetFogNear() const { return _fogNear; }

	void SetFogNear(float fogNear) { _fogNear = fogNear; }

	float GetFogFar() const { return _fogFar; }

	void SetFogFar(float fogFar) { _fogFar = fogFar; }

	float GetFogGamma() const { return _fogGamma; }

	void SetFogGamma(float fogGamma) { _fogGamma = fogGamma; }

	Color GetFogNearColor() const { return _fogNearColor; }

	void SetFogNearColor(const Color& fogNearColor) { _fogNearColor = fogNearColor; }

	Color GetFogFarColor() const { return _fogFarColor; }

	void SetFogFarColor(const Color& fogFarColor) { _fogFarColor = fogFarColor; }

	Color GetClearColor() const { return _clearColor; }

	void SetClearColor(const Color& clearColor) { _clearColor = clearColor; }

protected:

	Matrix44 _projection;
	Transform* _transform = nullptr;

	float _fogNear;
	float _fogFar;
	float _fogGamma;
	Color _fogNearColor;
	Color _fogFarColor;
	Color _clearColor;
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

	Vector3 GetDirection() const { return _transform->GetOrientation().GetZAxis(); }

	Vector3 GetPosition() const { return _transform->GetWorld().GetTranslation(); }

	float GetIntensity() const { return _intensity; }

	void SetIntensity(float intensity) { _intensity = intensity; }

	float GetRadius() const { return _radius; }

	void SetRadius(float radius) { _radius = radius; }

	bool GetShadowCasting() const { return _castShadow; }

	void SetShadowCasting(bool castShadow) { _castShadow = castShadow; }

	Vector3 GetColorAsVector() const;

#ifdef INSPECTOR
	void Inspect() override;
#endif

protected:

	LightType	_lightType		= DIRECTIONAL_LIGHT;
	Color		_color			= Color::White;
	float		_attenuation	= 0.5f;
	Transform*	_transform		= nullptr;
	float		_intensity		= 1.0f;
	float		_radius			= 10.0f;
	bool		_castShadow		= false;

};

inline Vector3 Light::GetColorAsVector() const

{ return Vector3(
	_color.r * (_intensity / 255.0f),
	_color.g * (_intensity / 255.0f),
	_color.b * (_intensity / 255.0f));
}

}

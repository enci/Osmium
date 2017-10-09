#include <Graphics/Render.h>
#include <Core/Entity.h>
#include <algorithm>
#include <Core/Transform.h>
#include <Graphics/Shader.h>
#include <Core/Game.h>
#include <imgui.h> 
#include <Graphics/DebugRenderer.h>
#include <Core/Resources.h>
#include <Graphics/Texture.h>
#include <Graphics/MeshRenderer.h>

#define PROFILE_OPENGL 1

using namespace Osm;



RenderManager::RenderManager(World& world)
	: Component(world)
	, _frameBuffer(0)
	//, _renderedTexture(0)
{
#if defined(INSPECTOR) && PROFILE_OPENGL 
	glGenQueries(RENDER_PASSES_NUM, _queries);
#endif

	// Get settings
	const auto& settings = Game.Settings();
	const auto width = settings.ScreenWidth;
	const auto height = settings.ScreenHeight;

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);

	// Position
	glGenTextures(1, &_positionBuffer);
	glBindTexture(GL_TEXTURE_2D, _positionBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _positionBuffer, 0);

	// Normal
	glGenTextures(1, &_normalBuffer);
	glBindTexture(GL_TEXTURE_2D, _normalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _normalBuffer, 0);

	// Albedo
	glGenTextures(1, &_albedoBuffer);
	glBindTexture(GL_TEXTURE_2D, _albedoBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _albedoBuffer, 0);

	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// The depth buffer
	glGenRenderbuffers(1, &_depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		ASSERT(false);
	

	_fullScreenPass = Game.Resources().LoadResource<Shader>(
		"./Assets/Shaders/Include/RenderTexture.vsh",
		"./Assets/Shaders/Include/RenderTexture.fsh");

	_deferredPass = Game.Resources().LoadResource<Shader>(
		"./Assets/Shaders/Include/DeferredPass.vsh",
		"./Assets/Shaders/Include/DeferredPass.fsh");
	_positionTarget = Game.Resources().CreateResource<RenderTarget>(_positionBuffer);
	_normalTarget = Game.Resources().CreateResource<RenderTarget>(_normalBuffer);
	_albedoTarget = Game.Resources().CreateResource<RenderTarget>(_albedoBuffer);
	_directionaLightsCountParam = _deferredPass->GetParameter("u_directionalLightsCount");
	_pointLightsCountParam = _deferredPass->GetParameter("u_pointLightsCount");
	_eyePosParam = _deferredPass->GetParameter("u_eyePos");
	for (int i = 0; i < kMaxDirecationalLights; i++)
	{
		string name = "u_directionalLights[" + to_string(i) + "]";
		auto lprm = new LightShaderParameter(_deferredPass, name);
		_dirLightParams.push_back(unique_ptr<LightShaderParameter>(lprm));
	}

	for (int i = 0; i < kMaxPointLights; i++)
	{
		string name = "u_pointLights[" + to_string(i) + "]";
		auto lprm = new LightShaderParameter(_deferredPass, name);
		_pointLightParams.push_back(unique_ptr<LightShaderParameter>(lprm));
	}

	_FXAAShader = Game.Resources().LoadResource<Shader>(
		"./Assets/Shaders/Include/FXAA.vsh",
		"./Assets/Shaders/Include/FXAA.fsh");
}

RenderManager::~RenderManager()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteBuffers(1, &_positionBuffer);
	glDeleteBuffers(1, &_normalBuffer);
	glDeleteBuffers(1, &_albedoBuffer);
	glDeleteBuffers(1, &_depthBuffer);
	glDeleteBuffers(1, &_frameBuffer);

	Game.Resources().ReleaseResource(_fullScreenPass);
	Game.Resources().ReleaseResource(_bloomShader);
	Game.Resources().ReleaseResource(_FXAAShader);
	Game.Resources().ReleaseResource(_deferredPass);
}


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void RenderManager::Render()
{
	if (!_enabled)
		return;

	// Get settings
	const auto& settings = Game.Settings();
	const auto width = settings.ScreenWidth;
	const auto height = settings.ScreenHeight;

#if defined(INSPECTOR)
	DrawCalls = 0;
	ShaderSwitches = 0;
#if PROFILE_OPENGL
	if (!_firstFrame)
		glGetQueryObjectuiv(_queries[FORWARD_PASS],
			GL_QUERY_RESULT,
			&_queryResults[FORWARD_PASS]);

	glBeginQuery(GL_TIME_ELAPSED, _queries[FORWARD_PASS]);
#endif
#endif

	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
	const Color clear = _cameras.size() > 0 ? _cameras[0]->GetClearColor() : Color::Black;
	glClearColor(clear.r / 255.0f, clear.g / 255.0f, clear.b / 255.0f, 10.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	Shader* activeShader = nullptr;

	for (auto l : _lights)
	{
		if (l->GetLightType() == Light::POINT_LIGHT)
		{
			gDebugRenderer.AddSphere(
				DebugRenderer::Categories::RENDERING,
				l->GetPosition(),
				l->GetRadius(),
				l->GetColor());
		}
	}

	for (auto c : _cameras)
	{
		Matrix44 view = c->GetView();
		Matrix44 projection = c->Projection();

		for (auto renderer : _renderables)
		{
			if(!renderer->GetEnbled())
				continue;

			if (renderer->GetShader() != activeShader)
			{
				renderer->ActivateShader(c, _lights);
				activeShader = renderer->GetShader();
#ifdef INSPECTOR
				ShaderSwitches++;
#endif
			}
			renderer->Draw();
#ifdef INSPECTOR
			DrawCalls++;
#endif
		}
	}

	// Composite
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	_deferredPass->Activate();
	_deferredPass->GetParameter("gbf_position")->SetValue(*_positionTarget);
	_deferredPass->GetParameter("gbf_normal")->SetValue(*_normalTarget);
	_deferredPass->GetParameter("gbf_albedo")->SetValue(*_albedoTarget);

	auto viewMatrix = _cameras[0]->GetView();
	auto viewInv = viewMatrix;
	viewInv.Invert();
	Vector3 eyePos = viewInv * Vector3(0, 0, 0);
	_eyePosParam->SetValue(eyePos);

	int pointLightsCount = 0;
	int dirLightsCount = 0;
	size_t maxDir = _dirLightParams.size();
	size_t maxPoint = _pointLightParams.size();
	for (auto l : _lights)
	{
		if (!l->GetEnbled())
			continue;

		if (l->GetLightType() == Light::DIRECTIONAL_LIGHT && dirLightsCount < (int)maxDir)
		{
			_dirLightParams[dirLightsCount++]->SetValue(*l);
		}
		else if (l->GetLightType() == Light::POINT_LIGHT && pointLightsCount < (int)maxPoint)
		{
			_pointLightParams[pointLightsCount++]->SetValue(*l);
		}
	}

	_directionaLightsCountParam->SetValue(dirLightsCount);
	_pointLightsCountParam->SetValue(pointLightsCount);
	
	RenderQuad();

#if defined(INSPECTOR)
	DrawCalls = 0;
#if PROFILE_OPENGL
	glEndQuery(GL_TIME_ELAPSED);
	_firstFrame = false;
#endif	
#endif
}

void RenderManager::Add(Renderable* renderable)
{
	_renderables.push_back(renderable);
}

void RenderManager::Add(Light* light)
{
	_lights.push_back(light);
}

void RenderManager::Add(Camera* camera)
{
	_cameras.push_back(camera);
}

void RenderManager::Remove(Renderable* renderable)
{
	_renderables.erase(remove(_renderables.begin(), _renderables.end(), renderable));
}

void RenderManager::Remove(Light* light)
{
	_lights.erase(remove(_lights.begin(), _lights.end(), light));
}

void RenderManager::Remove(Camera* camera)
{
	_cameras.erase(remove(_cameras.begin(), _cameras.end(), camera));
}

#ifdef INSPECTOR
void RenderManager::Inspect()
{
	ImGui::Checkbox("Enabled", &_enabled);
	ImGui::Text("Total renderables: %d", _renderables.size());
	ImGui::Text("Draw Calls: %d", DrawCalls);
	ImGui::Text("Shader Switches: %d", ShaderSwitches);
	
#if PROFILE_OPENGL
	double time = _queryResults[FORWARD_PASS] / 1000000.0;
	ImGui::Text("Render Time: %.3f", time);
#endif
}
#endif

Renderable::Renderable(Entity& entity) :  RenderManagerComponent(entity)
{
}

Camera::Camera(Entity& entity) : RenderManagerComponent(entity)
{
	_transform = _owner.GetComponent<Transform>();
	_fogNear = 80.0f;
	_fogFar = 1500.0;
	_fogGamma = 1.0f;
	_fogNearColor = Color::Black.Transparent();
	_fogFarColor = Color::Black.Transparent();
	ASSERT(_transform);
}

Matrix44 Camera::GetView()
{
	Matrix44 view = _transform->GetWorld();
	view.Invert();
	return view;
}

void Camera::SetView(Matrix44 view)
{
	ASSERT(!_transform->GetParent());
	view.Invert();
	_transform->SetLocal(view);
}

Light::Light(Entity& entity)
	: RenderManagerComponent(entity)
	, _lightType(DIRECTIONAL_LIGHT)
{
	_transform = _owner.GetComponent<Transform>();
	ASSERT(_transform);
}

#ifdef INSPECTOR

void Camera::Inspect()
{
	ImGui::DragFloat("Fog Near", &_fogNear);
	ImGui::DragFloat("Fog Far", &_fogFar);
	ImGui::DragFloat("Fog Gamma", &_fogGamma);
	ImGui::OsmColor("Near Color", _fogNearColor);
	ImGui::OsmColor("Far Color", _fogFarColor);
	ImGui::OsmColor("Clear Color", _clearColor);
}

void Light::Inspect()
{
	ImGui::Checkbox("Enabled", &_enabled);
	ImGui::OsmColor("Color", _color);
	ImGui::DragFloat("Intensity", &_intensity, 0.01f);
	ImGui::DragFloat("Radius", &_radius, 0.1f);
}

#endif


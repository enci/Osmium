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
#include <Graphics/Uniforms.h>

#define PROFILE_OPENGL 1

using namespace Osm;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

RenderManager::RenderManager(World& world) : Component(world)
{
#if defined(INSPECTOR) && PROFILE_OPENGL 
	glGenQueries(RENDER_PASSES_NUM, _queries);
#endif

	// Get settings
	const auto& settings = Game.Settings();
	const auto width = settings.ScreenWidth;
	const auto height = settings.ScreenHeight;

	//  -- MSAA framebuffer --
	glGenFramebuffers(1, &_msaaFramebuffer);					// Create
	glBindFramebuffer(GL_FRAMEBUFFER, _msaaFramebuffer);		// Bind FBO
	// Color buffer
	glGenTextures(1, &_msaaColorbuffer);						// Create MSAA color attachent
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _msaaColorbuffer);	// Bind
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, settings.MSAASamples, GL_RGB, width, height, GL_TRUE); // Set storage
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);				// Unbind
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _msaaColorbuffer, 0);	// Attach it
	// Depth buffer
	glGenRenderbuffers(1, &_msaaDepthbuffer);					// Create
	glBindRenderbuffer(GL_RENDERBUFFER, _msaaDepthbuffer);		// Bind
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, settings.MSAASamples, GL_DEPTH_COMPONENT, width, height); // Set storage
	glBindRenderbuffer(GL_RENDERBUFFER, 0);						// Unbind
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _msaaDepthbuffer); // Attach
	
	// Check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		ASSERT(false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// -- Resloved framebuffer --
	glGenFramebuffers(1, &_reslovedFramebuffer);					// Create
	glBindFramebuffer(GL_FRAMEBUFFER, _reslovedFramebuffer);		// Bind FBO
	// Color buffer
	glGenTextures(1, &_reslovedColorbuffer);						// Resolved
	glBindTexture(GL_TEXTURE_2D, _reslovedColorbuffer);				// Bind
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);	// Set storage
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Filtering
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _reslovedColorbuffer, 0);
	// Depth buffer
	glGenRenderbuffers(1, &_reslovedDepthbuffer);					// Create
	glBindRenderbuffer(GL_RENDERBUFFER, _reslovedDepthbuffer);		// Bind
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height); // Set storage
	glBindRenderbuffer(GL_RENDERBUFFER, 0);							// Unbind
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _reslovedDepthbuffer); // Attach

	// Check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		ASSERT(false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// Shadows end stuff	
	_fullScreenPass = Game.Resources().LoadResource<Shader>(
		"./Assets/Shaders/Include/RenderTexture.vsh",
		"./Assets/Shaders/Include/RenderTexture.fsh");

	_shadowPass = Game.Resources().LoadResource<Shader>(
		"./Assets/Shaders/Include/DepthOnly.vsh",
		"./Assets/Shaders/Include/DepthOnly.fsh");

	/*
	_bloomShader = Game.Resources().LoadResource<Shader>(
		"./Assets/Shaders/Include/RenderTexture.vsh",
		"./Assets/Shaders/Include/Bloom.fsh");
	*/

	_FXAAShader = Game.Resources().LoadResource<Shader>(
		"./Assets/Shaders/Include/FXAA.vsh",
		"./Assets/Shaders/Include/FXAA.fsh");

	
	// Create UBO
	_uniforms = new ShaderActivationUniforms();
	glGenBuffers(1, &_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
	glBufferData(
		GL_UNIFORM_BUFFER,
		sizeof(ShaderActivationUniforms),
		_uniforms,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, _ubo);	// Hardcode to slot 3
}

RenderManager::~RenderManager()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteTextures(1, &_msaaColorbuffer);
	glDeleteRenderbuffers(1, &_msaaDepthbuffer);
	glDeleteFramebuffers(1, &_msaaFramebuffer);

	glDeleteTextures(1, &_reslovedColorbuffer);
	glDeleteRenderbuffers(1, &_reslovedDepthbuffer);
	glDeleteFramebuffers(1, &_reslovedFramebuffer);

	glDeleteBuffers(1, &_ubo);
}


// Renders a 1x1 XY quad in NDC
void RenderQuad()
{
	static unsigned int quadVAO = 0;
	static unsigned int quadVBO = 0;

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

	for (auto l : _lights)
	{
		if(l->GetShadowCasting())
		{
			if (!l->_shadowMapFBO)
				l->CreateShadowBuffer();
			ASSERT(l->_shadowMapFBO);

			glViewport(0, 0, l->GetShadowResolution(), l->GetShadowResolution());
			glBindFramebuffer(GL_FRAMEBUFFER, l->_shadowMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);

			Matrix44 view = l->GetOwner().GetComponent<Transform>()->GetWorld();
			view.Invert();
			//view = Matrix44::CreateIdentity();

			float size = 100.0f;
			Matrix44 proj = Matrix44::CreateOrtho(
				-l->_shadowVolume.x, l->_shadowVolume.x,
				-l->_shadowVolume.y, l->_shadowVolume.y,
				-l->_shadowVolume.z, l->_shadowVolume.y);

			l->_shadowMatrix = proj * view;

			_shadowPass->Activate();
			for (auto r : _renderables)
			{
				if(!r->GetEnbled())
					continue;

				Matrix44 model;
				auto t = r->GetOwner().GetComponent<Transform>();
				if (t)
					model = t->GetWorld();
				Matrix44 mvp = l->_shadowMatrix * model;
				_shadowPass->GetParameter("u_modelViewProjection")->SetValue(mvp);
				Matrix44 identity;
				r->DrawDepth(identity);
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}


	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, _msaaFramebuffer);
	const Color clear = _cameras.size() > 0 ? _cameras[0]->GetClearColor() : Color::Black;
	glClearColor(clear.r / 255.0f, clear.g / 255.0f, clear.b / 255.0f, 10.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	
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

	Shader* activeShader = nullptr;
	
	for (auto c : _cameras)
	{	
		UpdateUBO(c);

		for (auto renderer : _renderables)
		{
			if(!renderer->GetEnbled())
				continue;

			if (renderer->GetShader() != activeShader)
			{
				renderer->ActivateShader(c, _lights);
				activeShader = renderer->GetShader();
				//activeShader->Activate();
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

#if defined(INSPECTOR)
	DrawCalls = 0;
#if PROFILE_OPENGL
	glEndQuery(GL_TIME_ELAPSED);	
	_firstFrame = false;
#endif	
#endif

	glBindFramebuffer(GL_READ_FRAMEBUFFER, _msaaFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _reslovedFramebuffer);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	
	_FXAAShader->Activate();
	_FXAAShader->GetParameter("frameBufSize")->SetValue(
		Vector2((float)width, (float)height));

	uint program = _FXAAShader->GetProgram();	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _reslovedColorbuffer);
	
	RenderQuad();
}

void RenderManager::UpdateUBO(Camera* camera)
{
	auto _viewMatrix = camera->GetView();
	auto _projectionMatrix = camera->Projection();
	//Vector3 eyePos = -1.0f * _viewMatrix.GetTranslation();
	auto viewInv = _viewMatrix;
	viewInv.Invert();
	Vector3 eyePos = viewInv * Vector3(0, 0, 0);

	_uniforms->u_projection = _projectionMatrix;
	_uniforms->u_view = _viewMatrix;
	_uniforms->u_eyePos = eyePos;
	_uniforms->u_fogNear = camera->GetFogNear();
	_uniforms->u_fogFar = camera->GetFogFar();
	_uniforms->u_fogExp = camera->GetFogGamma();
	_uniforms->u_fogColorNear = ToVector4(camera->GetFogNearColor());
	_uniforms->u_fogColorFar = ToVector4(camera->GetFogFarColor());
	_uniforms->u_time = Game.Time().ElapsedTime;

	int pointLightsCount = 0;
	int dirLightsCount = 0;
	size_t maxDir = DIR_LIGHT_COUNT;
	size_t maxPoint = POINT_LIGHT_COUNT;
	for (auto l : _lights)
	{
		if (!l->GetEnbled())
			continue;

		if (l->GetLightType() == Light::DIRECTIONAL_LIGHT && dirLightsCount < int(maxDir))
		{
			_uniforms->u_directionalLights[dirLightsCount].castShadow = l->GetShadowCasting();
			_uniforms->u_directionalLights[dirLightsCount].direction = l->GetDirection();
			_uniforms->u_directionalLights[dirLightsCount].color = l->GetColorAsVector();
			// _uniforms.u_directionalLights[dirLightsCount].shadowMatrix = l->GetShadowMatrix();
			dirLightsCount++;

		}
		else if (l->GetLightType() == Light::POINT_LIGHT && pointLightsCount < int(maxPoint))
		{
			_uniforms->u_pointLights[pointLightsCount].position = l->GetPosition();
			_uniforms->u_pointLights[pointLightsCount].radius = l->GetRadius();
			_uniforms->u_pointLights[pointLightsCount].color = l->GetColorAsVector();
			pointLightsCount++;
		}
	}
	_uniforms->u_directionalLightsCount = dirLightsCount;
	_uniforms->u_pointLightsCount = pointLightsCount;

	glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
	glBufferData(
		GL_UNIFORM_BUFFER,
		sizeof(ShaderActivationUniforms),
		_uniforms,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

Matrix44 Camera::GetView() const
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
	, _castShadow(false)
{
	_transform = _owner.GetComponent<Transform>();
	ASSERT(_transform);
}

Light::~Light()
{
}

void Light::CreateShadowBuffer()
{
	// Shadows being made	
	glGenFramebuffers(1, &_shadowMapFBO);

	glGenTextures(1, &_shadowMap);
	glBindTexture(GL_TEXTURE_2D, _shadowMap);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH_COMPONENT,
		_shadowResolution,
		_shadowResolution,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT,
		nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_GREATER);

	glBindFramebuffer(GL_FRAMEBUFFER, _shadowMapFBO);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		_shadowMap,
		0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		ASSERT(false);

	_shadowTexture = new RenderTarget(_shadowMap);
}

void Light::DeleteShadowBuffer()
{
	// TODO: Cleanup here
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
	// bool reCreateShadowBuff = false;
	if(ImGui::Checkbox("Cast Shadows", &_castShadow))
	{
		if (_castShadow)
			CreateShadowBuffer();
		else
			DeleteShadowBuffer();
	}

	if (_castShadow)
	{
		if (ImGui::InputInt("Shadow Resolution", &_shadowResolution))
		{
			DeleteShadowBuffer();
			CreateShadowBuffer();
		}

		if (_shadowMap != 0)
		{
			ImTextureID id = (void*)((UINT_PTR)(_shadowMap));
			auto w = ImGui::GetWindowWidth();
			ImGui::Image(id, ImVec2(w, w));
		}

		/*
		const char* resolutions[] =
		{
		"128",

		};
		ImGui::ListBox()
		*/
	}
}

#endif


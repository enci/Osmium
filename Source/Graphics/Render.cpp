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

#define PROFILE_OPENGL 1

using namespace Osm;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

RenderManager::RenderManager(World& world)
	: Component(world)
	, _framebuffer(0)
	, _colorbuffer(0)
{
#if defined(INSPECTOR) && PROFILE_OPENGL 
	glGenQueries(RENDER_PASSES_NUM, _queries);
#endif

	// Get settings
	const auto& settings = Game.Settings();
	const auto width = settings.ScreenWidth;
	const auto height = settings.ScreenHeight;

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

	// The texture we're going to render to
	glGenTextures(1, &_colorbuffer);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, _colorbuffer);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// The depth buffer
	glGenRenderbuffers(1, &_depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorbuffer, 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _colorbuffer, 0);

	// Set the list of draw buffers.
	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		ASSERT(false);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Shadows end
	
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

			Light* l = _lights[0];
			l->GetPosition();

			Matrix44 view = l->GetOwner().GetComponent<Transform>()->GetWorld();
			view.Invert();

			float size = 100.0f;
			Matrix44 proj = Matrix44::CreateOrtho(
				-size, size,
				-size, size,
				-200.0f,
				200.0f);

			_shadowPass->Activate();
			for (auto r : _renderables)
			{
				Matrix44 model;
				auto t = r->GetOwner().GetComponent<Transform>();
				if (t)
					model = t->GetWorld();
				Matrix44 mvp = proj * view * model;
				_shadowPass->GetParameter("u_modelViewProjection")->SetValue(mvp);
				Matrix44 identity;
				r->DrawDepth(identity);
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}


	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
	const Color clear = _cameras.size() > 0 ? _cameras[0]->GetClearColor() : Color::Black;
	glClearColor(clear.r / 255.0f, clear.g / 255.0f, clear.b / 255.0f, 10.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	

	/*
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
	*/

	Shader* activeShader = nullptr;
	
	for (auto c : _cameras)
	{
		// Matrix44 view = c->GetView();
		// Matrix44 projection = c->Projection();

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

#if defined(INSPECTOR)
	DrawCalls = 0;
#if PROFILE_OPENGL
	glEndQuery(GL_TIME_ELAPSED);	
	_firstFrame = false;
#endif	
#endif

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	
	_FXAAShader->Activate();
	_FXAAShader->GetParameter("frameBufSize")->SetValue(
		Vector2((float)width, (float)height)
	);
	uint program = _FXAAShader->GetProgram();	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _colorbuffer);
	
	RenderQuad();
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

	// _shadowTexture

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


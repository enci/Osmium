#include <Graphics/Render.h>
#include <Core/Entity.h>
#include <algorithm>
#include <Core/Transform.h>
#include <Graphics/Shader.h>
#include <Core/Engine.h>
#include <imgui.h> 

using namespace Osm;

RenderManager::RenderManager(World& world)
	: Component(world)
	, _framebufferName(0)
	, _renderedTexture(0)
{
	/*
	// Get settings
	auto settings = Engine.Settings();
	auto width = settings.ScreenWidth;
	auto height = settings.ScreenHeight;

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &_framebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferName);

	// The texture we're going to render to
	glGenTextures(1, &_renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, _renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// The depth buffer
	glGenRenderbuffers(1, &_depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderedTexture, 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _renderedTexture, 0);

	// Set the list of draw buffers.
	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		ASSERT(false);

	// Render to our framebuffer

	

	// Render on the whole framebuffer, complete from the lower left corner to the upper right
	//	glViewport(0, 0, 1024, 768); 
	*/
}

void RenderManager::Render()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, _framebufferName);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Shader* activeShader = nullptr;

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
			}
			renderer->Draw();
		}
	}
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

void RenderManager::Inspect()
{
	ImGui::Text("Total renderables: %d", _renderables.size());
}

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

Light::Light(Entity& entity) : RenderManagerComponent(entity)
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
	ImGui::OsmColor("Color", _color);
}

#endif


#include <Graphics/Render.h>
#include <Core/Entity.h>
#include <algorithm>
#include <Core/Transform.h>
#include <Graphics/Shader.h>

using namespace Osm;

void RenderManager::Render()
{
	Shader* activeShader = nullptr;

	for (auto c : _cameras)
	{
		Matrix44 view = c->GetView();
		Matrix44 projection = c->Projection();

		for (auto renderer : _renderables)
		{
			if (renderer->GetShader() != activeShader)
			{
				renderer->ActivateShader(view, projection, _lights);
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

Renderable::Renderable(Entity& entity) :  RenderManagerComponent(entity)
{
}

Camera::Camera(Entity& entity) : RenderManagerComponent(entity)
{
	_transform = _owner.GetComponent<Transform>();
	ASSERT(_transform);
}

Matrix44 Camera::GetView()
{
	Matrix44 view = _transform->GetTransform();
	view.Invert();
	return view;
}

void Camera::SetView(Matrix44 view)
{
	Matrix44& trns = _transform->GetTransform();
	Matrix44 temp = view;
	temp.Invert();
	trns = temp;
}

Light::Light(Entity& entity) : RenderManagerComponent(entity)
{
	_transform = _owner.GetComponent<Transform>();
	ASSERT(_transform);
}
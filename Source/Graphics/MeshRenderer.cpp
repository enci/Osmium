#include "stdafx.h"
#include <MeshRenderer.h>
#include <Transform.h>

using namespace igad;

MeshRenderer::MeshRenderer(Entity& entity)
	: Renderable(entity)	
	, _diffuse(Color::White)
	, _ambient(Color::Black)
{
}

MeshRenderer::~MeshRenderer()
{
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &_vao);
}

void MeshRenderer::SetShader(Shader* shader)
{
	ASSERT(_mesh);

	_shader = shader;

	_projParam = shader->GetParameter("u_projection");
	_modelParam = shader->GetParameter("u_model");
	_viewParam = shader->GetParameter("u_view");
	_textureParam = shader->GetParameter("u_texture");
	_eyePosParam = shader->GetParameter("u_eyePos");
	_modelViewParam = shader->GetParameter("u_modelView");
	_modelViewProjParam = shader->GetParameter("u_modelViewProj");
	_positionAttrib = shader->GetAttribute("a_position");
	_normalAttrib = shader->GetAttribute("a_normal");
	_textureAttrib = shader->GetAttribute("a_texture");
	_directionaLightsCountParam = shader->GetParameter("u_directionalLightsCount");
	_pointLightsCountParam = shader->GetParameter("u_pointLightsCount");
	_diffuseParam = shader->GetParameter("u_diffuse");
	_ambientParam = shader->GetParameter("u_ambient");

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
	GL_GET_ERROR();
	glBindVertexArray(_vao);
	GL_GET_ERROR();

	const GLuint* vbo = _mesh->GetVertexBuffers();

#ifdef DEBUG
	if (glIsBuffer(vbo[0]) != GL_TRUE)
	{
		LOG("The vertex buffer is not a valid buffer (VBO).");
	}
	if (glIsBuffer(vbo[1]) != GL_TRUE)
	{
		LOG("The index buffer is not a valid buffer (VBO)");
	}
#endif

	// Bind the buffers to the global state
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	GL_GET_ERROR();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	GL_GET_ERROR();

	// Default to VBO values, the pointer addresses are interpreted as byte-offsets.
	const void* firstPosition = reinterpret_cast<const void*>(offsetof(VertexFormat, Position));
	const void* firstNormal = reinterpret_cast<const void*>(offsetof(VertexFormat, Normal));
	const void* firstTexture = reinterpret_cast<const void*>(offsetof(VertexFormat, Texture));

	GLsizei size = sizeof(VertexFormat);
	_positionAttrib->SetAttributePointer(3, GL_FLOAT, GL_FALSE, size, firstPosition);
	_normalAttrib->SetAttributePointer(3, GL_FLOAT, GL_FALSE, size, firstNormal);
	_textureAttrib->SetAttributePointer(2, GL_FLOAT, GL_FALSE, size, firstTexture);

	glBindVertexArray(0);
}

void MeshRenderer::ActivateShader(	const Matrix44& view,
									const Matrix44& proj,
									const vector<Light*> lights)
{
	_viewMatrix = view;
	_projectionMatrix = proj;

	_shader->Activate();
	_projParam->SetValue(proj);
	_viewParam->SetValue(view);

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

void MeshRenderer::Draw()
{
	const Matrix44& model = GetEntity().GetComponent<Transform>()->GetTransform();
	Matrix44 modelView = _viewMatrix * model;
	Matrix44 modelViewProjection = _projectionMatrix * modelView;
	Vector3 eyePos = -1.0f * _viewMatrix.GetTranslation();

	_modelParam->SetValue(model);
	_modelViewParam->SetValue(modelView);
	_modelViewProjParam->SetValue(modelViewProjection);
	_eyePosParam->SetValue(eyePos);
	_diffuseParam->SetValue(_diffuse);
	_ambientParam->SetValue(_ambient);
	_textureParam->SetValue(*_texture);

	glBindVertexArray(_vao);
	GL_GET_ERROR();

	const void* firstIndex = reinterpret_cast<const void*>(0);
	glDrawElements(GL_TRIANGLES, _mesh->GetIndexCount(), GL_UNSIGNED_SHORT, firstIndex);
	GL_GET_ERROR();

	glBindVertexArray(0);
}

LightShaderParameter::LightShaderParameter(Shader* shader, const string& name) :
	_positionParam(shader->GetParameter(name + ".position")),
	_directionParam(shader->GetParameter(name + ".direction")),
	_colorParam(shader->GetParameter(name + ".color")),
	_attenuationParam(shader->GetParameter(name + ".attenuation"))
{}

void LightShaderParameter::SetValue(const Light& light)
{
	_colorParam->SetValue(light.GetColor());

	if (light.GetLightType() == Light::DIRECTIONAL_LIGHT)
	{
		Vector3 dir = light.GetDirection();
		_directionParam->SetValue(dir);
	}
	else if (light.GetLightType() == Light::POINT_LIGHT)
	{
		Vector3 pos = light.GetPosition();
		_positionParam->SetValue(pos);
	}
}
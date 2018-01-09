#include <Graphics/MeshRenderer.h>
#include <Core/Transform.h>
#include <imgui/imgui.h>
#include "Core/Game.h"
#include <Graphics/Texture.h>
#include <Graphics/Uniforms.h>

using namespace Osm;

MeshRenderer::MeshRenderer(Entity& entity)
	: Renderable(entity)	
	, _diffuse(Color::White)
	, _ambient(Color::Black)
	, _uniforms(new ShaderActivationUniforms)
{
}

MeshRenderer::~MeshRenderer()
{
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &_vao);
}

void MeshRenderer::SetMesh(Mesh* mesh)
{
	_mesh = mesh;

	if(_shader)
		CreateVAO();
}

void MeshRenderer::SetShader(Shader* shader)
{	
	_shader = shader;		

	_projParam = shader->GetParameter("u_projection");
	_modelParam = shader->GetParameter("u_model");
	_viewParam = shader->GetParameter("u_view");
	_textureParam = shader->GetParameter("u_texture");
	_eyePosParam = shader->GetParameter("u_eyePos");
	_modelViewParam = shader->GetParameter("u_modelView");
	_modelViewProjParam = shader->GetParameter("u_modelViewProjection");
	_positionAttrib = shader->GetAttribute("a_position");
	_normalAttrib = shader->GetAttribute("a_normal");
	_textureAttrib = shader->GetAttribute("a_texture");
	_directionaLightsCountParam = shader->GetParameter("u_directionalLightsCount");
	_pointLightsCountParam = shader->GetParameter("u_pointLightsCount");
	_diffuseParam = shader->GetParameter("u_diffuse");
	_ambientParam = shader->GetParameter("u_ambient");
	_fogNearParam = shader->GetParameter("u_fogNear");
	_fogFarParam = shader->GetParameter("u_fogFar");
	_fogExpParam = shader->GetParameter("u_fogExp");
	_fogNearColorParam = shader->GetParameter("u_fogColorNear");
	_fogFarColorParam = shader->GetParameter("u_fogColorFar");
	_timeParam = shader->GetParameter("u_time");

	for (int i = 0; i < kMaxDirecationalLights; i++)
	{
		string name = "u_directionalLights[" + to_string(i) + "]";
		auto lprm = new LightShaderParameter(_shader, name);
		_dirLightParams.push_back(unique_ptr<LightShaderParameter>(lprm));
	}

	for (int i = 0; i < kMaxPointLights; i++)
	{
		string name = "u_pointLights[" + to_string(i) + "]";
		auto lprm = new LightShaderParameter(_shader, name);
		_pointLightParams.push_back(unique_ptr<LightShaderParameter>(lprm));
	}

	auto id = _shader->GetProgram();
	auto idx = glGetUniformBlockIndex(id, "ShaderActivationUniforms");
	if (idx != 4294967295)
	{
		// Create UBO
		glGenBuffers(1, &_ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
		glBufferData(
			GL_UNIFORM_BUFFER,
			sizeof(ShaderActivationUniforms),
			NULL,
			GL_STATIC_DRAW);		
		glUniformBlockBinding(id, idx, 0);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ShaderActivationUniforms), _uniforms);
		//glBufferData(GL_UNIFORM_BUFFER, 0, sizeof(_uniforms), &_uniforms);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, _ubo, 0, sizeof(ShaderActivationUniforms));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	if(_mesh)
		CreateVAO();
}

void MeshRenderer::ActivateShader(	Camera* camera,
					const vector<Light*> lights)
{	
	_viewMatrix = camera->GetView();
	_projectionMatrix = camera->Projection();
	//Vector3 eyePos = -1.0f * _viewMatrix.GetTranslation();
	auto viewInv = _viewMatrix;
	viewInv.Invert();
	Vector3 eyePos = viewInv * Vector3(0, 0, 0);

	_shader->Activate();
	_projParam->SetValue(_projectionMatrix);
	_viewParam->SetValue(_viewMatrix);
	_eyePosParam->SetValue(eyePos);
	_fogNearParam->SetValue(camera->GetFogNear());
	_fogFarParam->SetValue(camera->GetFogFar());
	_fogExpParam->SetValue(camera->GetFogGamma());
	_fogNearColorParam->SetValue(camera->GetFogNearColor());
	_fogFarColorParam->SetValue(camera->GetFogFarColor());
	_timeParam->SetValue(Game.Time().ElapsedTime);

	int pointLightsCount = 0;
	int dirLightsCount = 0;
	size_t maxDir = _dirLightParams.size();
	size_t maxPoint = _pointLightParams.size();
	for (auto l : lights)
	{
		if(!l->GetEnbled())
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

	if(_ubo == 4294967295)
		return;	

	_uniforms->u_projection = _projectionMatrix;
	_uniforms->u_view = _viewMatrix;
	_uniforms->u_eyePos = eyePos;
	//_uniforms->u_fogNear = camera->GetFogNear();
	//_uniforms->u_fogFar = camera->GetFogFar();
	//_uniforms->u_fogExp =  camera->GetFogGamma();
	//_uniforms->u_fogColorNear = ToVector4(camera->GetFogNearColor());
	//_uniforms->u_fogColorFar = ToVector4(camera->GetFogFarColor());
	//_uniforms->u_time = Game.Time().ElapsedTime;

	
	pointLightsCount = 0;
	dirLightsCount = 0;
	maxDir = _dirLightParams.size();
	maxPoint = _pointLightParams.size();
	for (auto l : lights)
	{
		if (!l->GetEnbled())
			continue;

		if (l->GetLightType() == Light::DIRECTIONAL_LIGHT && dirLightsCount < int(maxDir))
		{
			// _uniforms.u_directionalLights[dirLightsCount].castShadow = l->GetShadowCasting();
			// _uniforms.u_directionalLights[dirLightsCount].direction = l->GetDirection();
			// _uniforms.u_directionalLights[dirLightsCount].color = l->GetColorAsVector();
			// _uniforms.u_directionalLights[dirLightsCount].shadowMatrix = l->GetShadowMatrix();
			dirLightsCount++;

		}
		else if (l->GetLightType() == Light::POINT_LIGHT && pointLightsCount < int(maxPoint))
		{
			// _uniforms.u_pointLights[pointLightsCount].position = l->GetPosition();
			// _uniforms.u_pointLights[pointLightsCount].radius = l->GetRadius();
			// _uniforms.u_pointLights[pointLightsCount].color = l->GetColorAsVector();
		}
	}
	_uniforms->u_directionalLightsCount = dirLightsCount;
	//_uniforms.u_pointLightsCount = pointLightsCount;
	

	
	glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, _ubo, 0, sizeof(ShaderActivationUniforms));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ShaderActivationUniforms), _uniforms);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void MeshRenderer::Draw()
{
#ifdef INSPECTOR
	if (_shader->Reloaded)
		CreateVAO();
#endif	

	const Matrix44& model = GetOwner().GetComponent<Transform>()->GetWorld();
	Matrix44 modelView = _viewMatrix * model;
	Matrix44 modelViewProjection = _projectionMatrix * modelView;	

	_modelParam->SetValue(model);
	_modelViewParam->SetValue(modelView);
	_modelViewProjParam->SetValue(modelViewProjection);
	_diffuseParam->SetValue(_diffuse);
	_ambientParam->SetValue(_ambient);
	_textureParam->SetValue(*_texture);

	glBindVertexArray(_vao);

	const void* firstIndex = reinterpret_cast<const void*>(0);
	glDrawElements(GL_TRIANGLES, _mesh->GetIndexCount(), GL_UNSIGNED_SHORT, firstIndex);

	glBindVertexArray(0);
}

void MeshRenderer::DrawDepth(Matrix44 viewProjection)
{
	glBindVertexArray(_vao);

	glDrawElements(GL_TRIANGLES, _mesh->GetIndexCount(), GL_UNSIGNED_SHORT, nullptr);

	glBindVertexArray(0);

	/*
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,						// attribute
		3,						// number of elements per vertex element
		GL_FLOAT,				// the type of each element
		GL_FALSE,				// take our values as-is or normalize
		sizeof(VertexFormat),	// no extra data between each position
		0						// offset of first element
	);
	*/
}

bool MeshRenderer::CreateVAO()
{
	if (_vao != 0)
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);
		_vao = 0;
	}

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	const GLuint* vbo = _mesh->GetVertexBuffers();

	// Bind the buffers to the global state
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);

	
#ifdef DEBUG
	if (glIsBuffer(vbo[0]) != GL_TRUE)
	{
		LOG("The vertex buffer is not a valid buffer (VBO).");
		return false;
	}
	if (glIsBuffer(vbo[1]) != GL_TRUE)
	{
		LOG("The index buffer is not a valid buffer (VBO)");
		return false;
	}
#endif

	// Default to VBO values, the pointer addresses are interpreted as byte-offsets.
	const void* firstPosition = reinterpret_cast<const void*>(offsetof(VertexFormat, Position));
	const void* firstNormal = reinterpret_cast<const void*>(offsetof(VertexFormat, Normal));
	const void* firstTexture = reinterpret_cast<const void*>(offsetof(VertexFormat, Texture));

	GLsizei size = sizeof(VertexFormat);
	_positionAttrib->SetAttributePointer(3, GL_FLOAT, GL_FALSE, size, firstPosition);
	_normalAttrib->SetAttributePointer(3, GL_FLOAT, GL_FALSE, size, firstNormal);
	_textureAttrib->SetAttributePointer(2, GL_FLOAT, GL_FALSE, size, firstTexture);

	glBindVertexArray(0);

	return true;
}

#ifdef INSPECTOR
void MeshRenderer::Inspect()
{
	ImGui::Checkbox("Enabled", &_enabled);
	ImGui::OsmColor("Diffuse", _diffuse);
	ImGui::OsmColor("Ambient", _ambient);		
}
#endif

LightShaderParameter::LightShaderParameter(Shader* shader, const string& name) :
	_positionParam(shader->GetParameter(name + ".position")),
	_directionParam(shader->GetParameter(name + ".direction")),
	_colorParam(shader->GetParameter(name + ".color")),
	_radiusParam(shader->GetParameter(name + ".radius")),
	_attenuationParam(shader->GetParameter(name + ".attenuation")),
	_shadowInvTransform(shader->GetParameter(name + ".shadowInvTransform")),
	_shadowMap(shader->GetParameter(name + ".shadowMap")),
	_castShadow(shader->GetParameter(name + ".castShadow"))
{}

void LightShaderParameter::SetValue(const Light& light)
{
	_colorParam->SetValue(light.GetColorAsVector());

	if (light.GetLightType() == Light::DIRECTIONAL_LIGHT)
	{
		Vector3 dir = light.GetDirection();
		_directionParam->SetValue(dir);
		
		if(light.GetShadowCasting())
		{
			_castShadow->SetValue(true);
			_shadowInvTransform->SetValue(light._shadowMatrix);
			const Texture* shadowMap = light.GetRenderTarget();
			_shadowMap->SetValue(*shadowMap);
		}
		else
		{
			_castShadow->SetValue(false);
		}
	}
	else if (light.GetLightType() == Light::POINT_LIGHT)
	{
		Vector3 pos = light.GetPosition();
		_positionParam->SetValue(pos);
		_radiusParam->SetValue(light.GetRadius());
	}
}



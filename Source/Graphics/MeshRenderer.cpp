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

	_modelParam = shader->GetParameter("u_model");
	_textureParam = shader->GetParameter("u_texture");
	_modelViewParam = shader->GetParameter("u_modelView");
	_modelViewProjParam = shader->GetParameter("u_modelViewProjection");
	_diffuseParam = shader->GetParameter("u_diffuse");
	_ambientParam = shader->GetParameter("u_ambient");

	_positionAttrib = shader->GetAttribute("a_position");
	_normalAttrib = shader->GetAttribute("a_normal");
	_textureAttrib = shader->GetAttribute("a_texture");

	const auto program = _shader->GetProgram();
	const auto index = glGetUniformBlockIndex(program, "ShaderActivationUniforms");
	if(index != GL_INVALID_INDEX)
		glUniformBlockBinding(program, index, 3);

	if(_mesh)
		CreateVAO();
}

void MeshRenderer::ActivateShader(	Camera* camera,
					const vector<Light*> lights)
{	
	_shader->Activate();	
	_viewMatrix = camera->GetView();
	_projectionMatrix = camera->Projection();
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



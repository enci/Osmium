#pragma once

#include <Graphics/Render.h>
#include <Graphics/Mesh.h>

struct ShaderActivationUniforms;

namespace Osm
{
///
/// MeshRenderer
///
class MeshRenderer : public Renderable
{
public:
	const int kMaxDirecationalLights = 5;
	const int kMaxPointLights = 10;

public:

	MeshRenderer(Entity& entity);

	virtual ~MeshRenderer();

	Mesh* GetMesh() { return _mesh; }

	void SetMesh(Mesh* mesh);

	virtual void SetShader(Shader* shader) override;

	GLuint GetVAO() { return _vao; }

	Texture* GetTexture() { return _texture; }

	void SetTexture(Texture* texture) { _texture = texture; }

	Color GetDiffuse() const { return _diffuse; }

	void SetDiffuse(Color diffuse) { _diffuse = diffuse; }

	Color GetAmbient() const { return _ambient; }

	void SetAmbient(Color ambient) { _ambient = ambient; }

	void ActivateShader(
		Camera* camera,
		const std::vector<Light*> lights) override;

	void Draw() override;

	void DrawDepth(Matrix44 viewProjection) override;

#ifdef INSPECTOR
	void Inspect() override;
#endif#endif


protected:

	bool CreateVAO();

	GLuint	_vao = 0;
	Mesh*	_mesh = nullptr;
	Texture* _texture = nullptr;
	ShaderParameter* _modelParam = nullptr;
	ShaderParameter* _textureParam = nullptr;	
	ShaderParameter* _modelViewProjParam = nullptr;
	ShaderParameter* _modelViewParam = nullptr;
	ShaderParameter* _diffuseParam = nullptr;
	ShaderParameter* _ambientParam = nullptr;
	ShaderAttribute* _positionAttrib = nullptr;
	ShaderAttribute* _normalAttrib = nullptr;
	ShaderAttribute* _textureAttrib = nullptr;
	
	std::vector<ShaderParameter*> _shadowParams;

	Transform* _transform = nullptr;
	Matrix44 _viewMatrix;
	Matrix44 _projectionMatrix;
	Color _diffuse;
	Color _ambient;
	GLuint _ubo = 4294967295;	
	ShaderActivationUniforms* _uniforms;
};

}
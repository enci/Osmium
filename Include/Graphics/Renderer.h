#pragma once

#include <Math/Matrix44.h>
#include <Graphics/Mesh.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <unordered_map>
#include <Core/Component.h>
#include <Core/World.h>

namespace igad
{

class MeshRenderer : public Component<Entity>
{
public:
	MeshRenderer(Entity& entity) : Component(entity) {}
	void Init(Mesh*	mesh, Texture* texture) { _mesh = mesh, _texture = texture; }
	Mesh* GetMesh() { return _mesh; }
	Texture* GetTexture() { return _texture; }

private:
	Mesh*	_mesh = nullptr;
	Texture* _texture = nullptr;
};

class Renderer : public Component<World>
{
public:
	Renderer(World& world) : Component<World>(world) {}
	~Renderer();

	void Init(Shader* shader);
	
	void Begin(const Matrix44& view, const Matrix44& projection);

	void Render(const Matrix44& model,
				const Texture* texture,
				const Mesh* mesh);
	void End();

protected:
	Shader*			 _shader = nullptr;
	ShaderParameter* _projParam	= nullptr;
	ShaderParameter* _modelParam = nullptr;
	ShaderParameter* _viewParam	= nullptr;
	ShaderParameter* _textureParam = nullptr;
	ShaderAttribute* _positionAttrib = nullptr;
	ShaderAttribute* _normalAttrib = nullptr;
	ShaderAttribute* _textureAttrib = nullptr;

	struct VAOEntry
	{
		GLuint vao;
		uint count;
	};

	std::unordered_map<const Mesh*, VAOEntry> _vaos;
};

}
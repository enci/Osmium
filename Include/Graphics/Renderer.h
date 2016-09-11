#pragma once

#include <Math/Matrix44.h>
#include <Graphics/Mesh.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <unordered_map>

namespace igad
{

class Renderer
{
public:
	Renderer(Shader* shader);
	~Renderer();
	
	void Begin(const Matrix44& view, const Matrix44& projection);

	void Render(const Matrix44& model,
				const Texture* texture,
				const Mesh* mesh);
	void End();

protected:
	Shader*	_shader;
	ShaderParameter* _projParam;
	ShaderParameter* _modelParam;
	ShaderParameter* _viewParam;
	ShaderParameter* _textureParam;
	ShaderAttribute* _positionAttrib;
	ShaderAttribute* _normalAttrib;
	ShaderAttribute* _textureAttrib;


	struct VAOEntry
	{
		GLuint vao;
		uint count;
	};

	std::unordered_map<const Mesh*, VAOEntry> _vaos;
};

}
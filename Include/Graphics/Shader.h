#pragma once

#include <string>
#include <map>
#include <memory>

#include <Graphics/OpenGL.h>
#include <Math/Matrix44.h>
#include <Math/Vector3.h>
#include <Math/Vector2.h>

namespace Osm
{

class Shader;
class Texture;
class Color;

///
/// ShaderParameter is a representation of an shader parameter.
/// It has a type and it will complain if the type declared in the
/// shader program is different.
///
class ShaderParameter
{
	friend class Shader;

public:

	/// Check if this parameter object is valid
	bool IsValid() const { return _location != -1; }

	/// Gets the type of this parameter
	GLenum GetType() const { return _type; }

	/// Location of this parameter
	GLint GetLocation() const { return _location; }

	/// Set the value. Parameter should be float.
	void SetValue(float val);

	/// Set the value. Parameter should be int.
	void SetValue(int val);

	/// Set the value. Parameter should be bool.
	void SetValue(bool val);

	/// Set the value. Parameter should be Vector2.
	void SetValue(const Vector2& vec);

	/// Set the value. Parameter should be Vector3.
	void SetValue(const Vector3& vec);

	/// Set the value. Parameter should be Vector4.
	void SetValue(const Vector4& vec);

	/// Set the value. Parameter should be Color.
	void SetValue(const Color& color);

	/// Set the value. Parameter should be Matrix33.
	void SetValue(const Matrix44& mtx, bool transpose = false);

	/// Set the value. Parameter should be Texture.
	void SetValue(const Texture& texture);

protected:

	/// The shader creates a parameter.
	ShaderParameter(Shader* shader, std::string name, GLenum type, GLint location, GLint sampler = -1)
		: _shader(shader)
		, _name(name)
		, _type(type)
		, _location(location)
		, _sampler(sampler)
	{}

	/// Constructor for an invalid ShaderParameter
	ShaderParameter() : _shader(nullptr), _name(""), _type(0), _location(-1), _sampler(-1) {}


	/// The shader can reset the parameter after a reload
	void Reset(Shader* shader, std::string name, GLenum type, GLint location, GLint sampler = -1)
	{
		this->_shader = shader;
		this->_type = type;
		this->_location = location;
		this->_name = name;
		this->_sampler = sampler;
	}

	/// The shader should invalidate when reloading a new shader file
	/// a some information can be old
	void Invalidate()
	{
		_shader = nullptr;
		_name = "";
		_type = 0;
		_location = -1;
		_sampler = -1;
	}

	/// The effect this parameter belongs to. Unused (beyond debugging)
	Shader*			_shader;

	/// Name as defined in the shader file.
	std::string		_name;

	/// Type as defined in the shader file.
	GLenum			_type;

	/// Location for the setting for this.
	GLint			_location;

	/// Only valid for type sampler (GL_SAMPLER_2D)
	GLint			_sampler;
};


///
/// A vertex attribute
///
class ShaderAttribute
{
	friend class Shader;

public:

	/// Check if this parameter object is valid
	bool IsValid() const { return _location != -1; }

	/// Gets the type of this parameter
	GLenum GetType() const { return _type; }

	/// Location of this parameter
	GLint GetLocation() const { return _location; }

	/// Check documentation for glVertexAttribPointer
	void SetAttributePointer(
		GLint size,
		GLenum type,
		GLboolean normalized,
		GLsizei stride,
		const GLvoid * pointer);

	/// Check documentation for glVertexAttribPointer
	void DisableAttributePointer();

protected:

	/// The shader creates a parameter.
	ShaderAttribute(Shader* shader, std::string name, GLenum type, GLint location) :
		_shader(shader),
		_type(type),
		_location(location),
		_name(name)
	{}

	/// Constructor for an invalid ShaderParameter
	ShaderAttribute() : _shader(nullptr), _name(""), _type(0), _location(-1) {}


	/// The shader can reset the parameter after a reload
	void Reset(Shader* shader, std::string name, GLenum type, GLint location)
	{
		this->_shader = shader;
		this->_type = type;
		this->_location = location;
		this->_name = name;
	}

	/// The shader should invalidate when reloading a new shader file
	/// as some information can be old
	void Invalidate()
	{
		_shader = nullptr;
		_name = "";
		_type = 0;
		_location = -1;
	}


protected:
	/// The effect this parameter belongs to. Unused (beyond debugging)
	Shader*		_shader;

	/// Name as defined in the shader file.
	std::string  _name;

	/// Type as defined in the shader file.
	GLenum		_type;

	/// Location for the setting for this.
	GLint		_location;

};

///
/// Shader is a class representing a compiled GPU program
///
class Shader
{
public:
	/// Create an empty shader. You will need to provide the source with LoadSource() before 
	/// you can use the shader.
	Shader() {}

	/// Create a shader with vertex and fragment programs 
	Shader(	const std::string& vertexFilename,
			const std::string& fragmentFilename);

	/// Create a shader with vertex geometry and fragment programs 
	Shader(	const std::string& vertexFilename,
			const std::string& geometryFilename,
			const std::string& fragmentFilename);

	Shader(const Shader& other) = delete;

	~Shader();

	/// Load with source (as oposed to files)
	bool LoadSource(const std::string& vertexShader,
					const std::string& geometryShader,
					const std::string& fragmentShader);

	/// Get active parameter by name. If the parameter is not present/active
	/// you will get an invalid one.
	ShaderParameter* GetParameter(const std::string& name);

	/// Get active attribute by name. If the attribute is not present/active
	/// you will get an invalid one.
	ShaderAttribute* GetAttribute(const std::string& name);

	/// Get the program in case you want to do everyting manually
	GLuint GetProgram() const;

	/// Call before drawing 
	void Activate();

	/// Call after drawing
	void Deactivate();

private:

	void LoadParameters();	

	bool Validate();

	/// Just a helper for the constructors
	bool Load(	const std::string& vertexFilename,
				const std::string& fragmentFilename,
				const std::string& geometryFilename = "");

private:

	/// Store all the parameters
	std::map<std::string, std::unique_ptr<ShaderParameter>> _parameters;

	/// Store all the attributes
	std::map<std::string, std::unique_ptr<ShaderAttribute>> _attributes;

	/// GL id (name) of the compiled program
	GLuint _program = 0;
};

}
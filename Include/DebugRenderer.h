#pragma once

#include <Color.h>
#include <Shader.h>
#include <Vector3.h>
#include <Matrix44.h>

namespace igad
{

///
/// Debug drawing in 3d space (good for testing ideas and stuff). Once a drawing
/// max has been reached, all subsequent draw calls are silently ignored.
///
class DebugRenderer
{
	struct VertexPosition3DColor
	{
		Vector3 Position;
		Color Color;
	};
        
public:
    /// No args, for static creation
    DebugRenderer();
        
    /// Does nothing
    virtual ~DebugRenderer();

	/// Call to make debug draw, gets the _shader program id
	void Initialize();
                
    /// Queues a line segment for drawing
	void AddLine(	const Vector3& from,
                    const Vector3& to,
                    const Color& color = igad::Color::Red);
        
	/// Queues a circle for drawing
	void AddCircle(const Vector3& center,
                    float radius         = 1.0f,
                    const Color& color   = igad::Color::Red,
                    int divs             = 12);
        
    /// Queues a sphere for drawing
	void AddAxisSphere(const Vector3& center,
                        float radius         = 1.0f,
                        const Color& xzColor = igad::Color::Green,
                        const Color& xyColor = igad::Color::Blue,
                        const Color& yzColor = igad::Color::Red,
                        int divs             = 12);

    /// Queues a sphere for drawing
	void AddSphere(const Vector3& center,
                    float radius         = 1.0f,
                    const Color& color   = igad::Color::Red,
                    int divs             = 12)
    { AddAxisSphere(center, radius, color, color, color,divs); }
        
    /// Queues an axis display
    void AddAxis(const Matrix44& trans, float size);
        
    /// Clears queued actions
	void Clear();

	/// Draw queue with a view projection matrix
	void Draw(Matrix44& vp);

protected:

	static int const _maxLines = 4096;

	int                     _linesCount;
	VertexPosition3DColor   _vertexArray[_maxLines * 2];

	unique_ptr<Shader>	_shader;
	GLuint				_vao			= 0;
	GLuint				_linesVBO		= 0;
	ShaderAttribute*	_attribColor	= nullptr;
	ShaderAttribute*	_attribVertex	= nullptr;
	ShaderParameter*	_paramCamera	= nullptr;
};
    
    /// Manager global
    extern DebugRenderer gDebugRenderer;
}

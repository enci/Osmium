
#include "BlasteroidsCamera.h"
#include <Core/Device.h>
#include "Spaceship.h"
#include <Utils.h>
#include <Math/Vector4.h>
#include <Graphics/DebugRenderer.h>

using namespace igad;

Vector3 Ray3D::Evaluate(float t) const
{
	return Origin + Direction * t;
}

BlasteroidsCamera::BlasteroidsCamera(World& world)
	: Entity(world)
	, _shakeIntensity(0.0f)
{
	_transform = CreateComponent<Transform>();
	_camera = CreateComponent<Camera>();

	float fov = 60.0f;
	float dist = 15.0f;
	_averagePosition = Vector3(0, dist, 0);
	int screenWidth = igad::pDevice->GetScreenWidth();
	int screenHeight = igad::pDevice->GetScreenHeight();
	float ratio = float(screenWidth) / float(screenHeight);

	Matrix44 view = Matrix44::CreateLookAt(_averagePosition, Vector3(), Vector3(0, 0, 1));
	Matrix44 projection = Matrix44::CreatePerspective(DegToRad(fov), ratio, 0.1f, 2000.0f);

	_camera->SetView(view);
	_camera->SetProjection(projection);
}

void BlasteroidsCamera::Update(float dt)
{
	static float dist = 140;

	auto spaceshpis = GetWorld().GetEntitiesByType<SpaceShip>();
	Vector3 average(0.0f, 0.0f, 0.0f);
	for (auto s : spaceshpis)
	{
		average += s->GetComponent<Transform>()->GetPosition();
	}
	average.x *= -0.25f / spaceshpis.size();
	average.z *= -0.35f / spaceshpis.size();
	average.y = dist;
	average.Normalize();
	average *= dist;

	_averagePosition = Lerp(_averagePosition, average, dt * 2);

	Vector3 offset = RandomOnUnitSphere() * _shakeIntensity;
	_shakeIntensity *= 0.85;
	
	Matrix44 view = Matrix44::CreateLookAt(	_averagePosition + offset,
											Vector3(),
											Vector3(0, 0, 1));
	_camera->SetView(view);

	/*
	// Debug camera
	static float fi = 0;
	static float theta = HalfPi;
	static float r = 100.0f;

	if (glfwGetKey(pDevice->GetWindow(), GLFW_KEY_UP))
		theta += 0.01;
	if (glfwGetKey(pDevice->GetWindow(), GLFW_KEY_DOWN))
		theta -= 0.01;
	if (glfwGetKey(pDevice->GetWindow(), GLFW_KEY_LEFT))
		fi -= 0.01f;
	if (glfwGetKey(pDevice->GetWindow(), GLFW_KEY_RIGHT))
		fi += 0.01f;
	
	float x = r * sin(theta) * cos(fi);
	float y = r * sin(theta) * sin(fi);
	float z = r * cos(theta);

	view = Matrix44::CreateLookAt(Vector3(x, z, y), Vector3(), Vector3(0, 1, 0));
	_camera->SetView(view);
	*/

	/*
	Ray3D topRightRay = Unproject(Vector2(1, 1));
	float t = -topRightRay.Origin.y / topRightRay.Direction.y;
	Vector3 point = topRightRay.Evaluate(t);
	gDebugRenderer.AddCircle(point, 10, Color::Magenta);
	*/

	/*
	Ray3D bottomLeftRay = Unproject(Vector2(-1, -1));
	t = -bottomLeftRay.Origin.z / bottomLeftRay.Direction.z;
	point = bottomLeftRay.Evaluate(t);
	gDebugRenderer.AddCircle(point, 1, Color::Magenta);
	*/
}

igad::Vector3 BlasteroidsCamera::Unproject(const igad::Vector3& v) const
{
	Vector4 vec(v.x, v.y, v.z);
	Matrix44 vpinv = _camera->Projection() * _camera->GetView();
	vpinv.Invert();

	Vector4 unproj = vpinv * vec;
	unproj.NormalizeW();

	return Vector3(unproj.x, unproj.y, unproj.z);
}

Ray3D BlasteroidsCamera::Unproject(const igad::Vector2& v) const
{
	// Unproject a vector at the near plane
	Vector3 from = Unproject(Vector3(v.x, v.y, 0.0f));
	// Unproject a vector at the far plane
	Vector3 to = Unproject(Vector3(v.x, v.y, 1.0f));

	// Make a ray out of the two vectors
	Ray3D ray;
	ray.Origin = from;
	ray.Direction = to - from;
	ray.Direction.Normalize();
	return ray;
}

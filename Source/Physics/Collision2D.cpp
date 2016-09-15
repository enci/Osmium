#include <Physics/Collision2D.h>
#include <Physics/Physics2D.h>

using namespace Osm;

float Collision2D::SeparatingVelocity() const
{
	Vector2 relativeVelocity = FirstBody->GetVelocity() - SecondBody->GetVelocity();
	return relativeVelocity.Dot(Normal);
}

float Collision2D::TotalImpluse() const
{
	return SeparatingVelocity() * (FirstBody->GetMass() + FirstBody->GetMass());
}

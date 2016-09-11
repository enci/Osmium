#include "stdafx.h"
#include <Collision2D.h>
#include <Physics2D.h>

using namespace igad;

float Collision2D::SeparatingVelocity() const
{
	Vector2 relativeVelocity = FirstBody->GetVelocity() - SecondBody->GetVelocity();
	return relativeVelocity.Dot(Normal);
}

float Collision2D::TotalImpluse() const
{
	return SeparatingVelocity() * (FirstBody->GetMass() + FirstBody->GetMass());
}

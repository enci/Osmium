
#include <Math/Vector2.h>
#include <Defines.h>

using namespace Osm;

Vector2 Vector2::RandomUnit()
{
    float rad = RandInRange(-Pi, Pi);
    return Vector2( cosf(rad), sinf(rad));
}

Vector2 Vector2::Zero(0, 0);

// end

#include "Weapon.h"

Weapon::Weapon()
{
	damage = 10.0f;
}

void Weapon::Shoot(glm::vec3 direction)
{
	if (!reloading)
	{

	}
}

void Weapon::Reload()
{
	reloading = true;

	currentAmmo = maxAmmo;
	reloading = false;
}
#pragma once

#include "main.h"

class Weapon {
	public:
		Weapon();
		void Shoot(glm::vec3 direction);

		void Reload();

	private:
		float damage;
		float reloadTime;

		float maxAmmo;
		float currentAmmo;

		bool reloading;
};
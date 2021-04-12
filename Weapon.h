#pragma once

#include "main.h"
#include<vector>

#include <glm/glm.hpp>
class Weapon {
	public:
		Weapon();
		void Shoot(glm::vec3 origin, glm::vec3 direction);

		void Reload();

	private:
		float damage;
		float reloadTime;

		float maxAmmo;
		float currentAmmo;

		bool reloading;
};
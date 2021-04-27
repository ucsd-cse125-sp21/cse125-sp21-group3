#pragma once

#include "main.h"
#include "Player.h"

class Ability {
public:
	Ability(Player* player);

	void setAbility(int ability);

	void useAbility();



private:
	int currentAbility;

	Player* currentPlayer;

};


#pragma once
#include "UnitEntity.h"

class GameplayManager
{
public:
/*
* Note - 
* Unit 1 and 2 can be the same unit
* i.e. unit 1 wants to heal themselves, when selecting heal target they select
* themselves which makes them both unit 1 and 2
*/
	void BasicAttack(UnitEntity* AttPlayer, UnitEntity* DefPlayer);
	void BasicHeal(UnitEntity* unit1, UnitEntity* unit2);
	/*
	void PhysicalAbility(unit 1, unit 2)
	{
		
	}

	void MagicAbility(unit 1, unit 2)
	{

	}

	void BuffAbility (unit 1, unit 2)
	{

	}
	*/
private:
	//Hit chance generator (rand num between 0-100)
	int HitPercentage();
	int CritHitPercentage();
};

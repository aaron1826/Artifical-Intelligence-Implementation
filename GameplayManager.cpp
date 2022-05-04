#include "GameplayManager.h"

void GameplayManager::BasicAttack(UnitEntity* AttPlayer, UnitEntity* DefPlayer)
{
	int damage = AttPlayer->GetClassTotals().TotalPhysAttack -
		DefPlayer->GetClassTotals().TotalPhysArmour;

	int hit = AttPlayer->GetClassTotals().TotalPhysHit -
		DefPlayer->GetClassTotals().TotalEvasion;

	if (HitPercentage() <= hit)
	{
		if (CritHitPercentage() <= AttPlayer->GetClassTotals().TotalCritHit)
		{
			DefPlayer->GetClassTotals().CurrentHP -= damage *
				AttPlayer->GetClassTotals().TotalCritMultiplier;
		}
		else
		{
			DefPlayer->GetClassTotals().CurrentHP -= damage;
		}
	}
	AttPlayer->GetClassTotals().CurrentMP -= 10;
}

void GameplayManager::BasicHeal(UnitEntity* unit1, UnitEntity* unit2)
{
	unit2->GetClassTotals().CurrentHP += unit1->GetClassTotals().TotalMagAttack / 2;

	if (unit2->GetClassTotals().CurrentHP > unit2->GetClassTotals().MaxHP)
	{
		unit2->GetClassTotals().CurrentHP = unit2->GetClassTotals().MaxHP;
	}
}

int GameplayManager::HitPercentage()
{
	srand((unsigned)time(0));

	int hitchance = rand() % 100 + 1;
	return hitchance;
}

int GameplayManager::CritHitPercentage()
{
	srand((unsigned)time(0));

	int critChance = rand() % 100 + 1;
	return critChance;
}


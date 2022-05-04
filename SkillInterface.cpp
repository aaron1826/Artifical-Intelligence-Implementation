#include "SkillInterface.h"
#include "UnitEntity.h"

int PhysicalAttack::DoAction(UnitEntity* AttPlayer, UnitEntity* DefPlayer)
{
	m_Damage = AttPlayer->GetClassTotals().TotalPhysAttack -
		DefPlayer->GetClassTotals().TotalPhysArmour;
	AttPlayer->GetClassTotals().CurrentMP -= m_Data.ManaCost;

	DefPlayer->GetClassTotals().CurrentHP -= m_Damage;
	return 0;
}

int PhysicalAttack::DoCrit(UnitEntity* AttPlayer, UnitEntity* DefPlayer)
{
	m_Damage = (AttPlayer->GetClassTotals().TotalPhysAttack -
		DefPlayer->GetClassTotals().TotalPhysArmour) * 
		AttPlayer->GetClassTotals().TotalCritMultiplier;

	AttPlayer->GetClassTotals().CurrentMP -= m_Data.ManaCost;

	DefPlayer->GetClassTotals().CurrentHP -= m_Damage;
	return 0;
}

int MagicalAttack::DoAction(UnitEntity* AttPlayer, UnitEntity* DefPlayer)
{
	m_Damage = AttPlayer->GetClassTotals().TotalMagAttack -
		DefPlayer->GetClassTotals().TotalMagArmour;
	AttPlayer->GetClassTotals().CurrentMP -= m_Data.ManaCost;

	DefPlayer->GetClassTotals().CurrentHP -= m_Damage;
	return 0;
}

int MagicalAttack::DoCrit(UnitEntity* AttPlayer, UnitEntity* DefPlayer)
{
	m_Damage = (AttPlayer->GetClassTotals().TotalMagAttack -
		DefPlayer->GetClassTotals().TotalMagArmour) *
		AttPlayer->GetClassTotals().TotalCritMultiplier;

	AttPlayer->GetClassTotals().CurrentMP -= m_Data.ManaCost;

	DefPlayer->GetClassTotals().CurrentHP -= m_Damage;
	return 0;
}

int Heal::DoAction(UnitEntity* AttPlayer, UnitEntity* DefPlayer)
{
	DefPlayer->GetClassTotals().CurrentHP +=
		AttPlayer->GetClassTotals().MaxHP * m_Data.HealPercent;
	AttPlayer->GetClassTotals().CurrentMP -= m_Data.ManaCost;

	return 0;
}

int Buff::DoAction(UnitEntity* AttPlayer, UnitEntity* DefPlayer)
{
	DefPlayer->GetBuffs().CritDamageBuff = m_Data.CritDamageBuff;
	DefPlayer->GetBuffs().CritHitBuff = m_Data.CritHitBuff;
	DefPlayer->GetBuffs().EvasionBuff = m_Data.EvasionBuff;
	DefPlayer->GetBuffs().MagArmourBuff = m_Data.MagArmourBuff;
	DefPlayer->GetBuffs().MagDamageBuff = m_Data.MagDamageBuff;
	DefPlayer->GetBuffs().MagHitBuff = m_Data.MagHitBuff;
	DefPlayer->GetBuffs().MagResistBuff = m_Data.MagResistBuff;
	DefPlayer->GetBuffs().MovespeedBuff = m_Data.MovespeedBuff;
	DefPlayer->GetBuffs().PhysArmourBuff = m_Data.PhysArmourBuff;
	DefPlayer->GetBuffs().PhysDamageBuff = m_Data.PhysDamageBuff;
	DefPlayer->GetBuffs().PhysHitBuff = m_Data.PhysHitBuff;

	DefPlayer->GetClassTotals().CurrentMP -= m_Data.ManaCost;

	return 0;
}

#pragma once

#include "D3DUtils.h"
#include <string>

class UnitEntity;

class SkillInterface
{
public:
	enum SkillType
	{
		PHYS_ATT,
		MAG_ATT,
		HEAL,
		BUFF
	};
	virtual int DoAction(UnitEntity* AttPlayer, UnitEntity* DefPlayer) { return 0; };
	virtual const std::string& GetActionName() = 0;
	virtual int& GetManaCost() = 0;
private:

};

/*
* Four different skill types
*	Physical Attack
*	Magical Attack
*	Heal
*	Buff
*/
class PhysicalAttack : public SkillInterface
{
public:
	//Calculates damage value
	int DoAction(UnitEntity* AttPlayer, UnitEntity* DefPlayer) override;
	int DoCrit(UnitEntity* player1, UnitEntity* player2);
	//Get//
	const std::string& GetActionName() { return m_Data.Name; }
	int& GetManaCost() { return m_Data.ManaCost; }
	DamageSkills& GetData() { return m_Data; }
	
private:
	DamageSkills m_Data;
	float m_Damage;
};

class MagicalAttack : public SkillInterface
{
public:
	//Calculates damage value
	int DoAction(UnitEntity* AttPlayer, UnitEntity* DefPlayer) override;
	int DoCrit(UnitEntity* AttPlayer, UnitEntity* DefPlayer);
	//Get//
	const std::string& GetActionName() { return m_Data.Name; }
	int& GetManaCost() { return m_Data.ManaCost; }
	DamageSkills& GetData() { return m_Data; }
private:
	DamageSkills m_Data;
	float m_Damage;
};

class Heal : public SkillInterface
{
public:
	//Calculates heal value
	int DoAction(UnitEntity* AttPlayer, UnitEntity* DefPlayer) override;
	//Get//
	const std::string& GetActionName() { return m_Data.Name; }
	int& GetManaCost() { return m_Data.ManaCost; }
	HealSkills& GetData() { return m_Data; }
private:
	HealSkills m_Data;

};

class Buff : public SkillInterface
{
public:
	//Applies buffs
	int DoAction(UnitEntity* AttPlayer, UnitEntity* DefPlayer) override;
	 //Get//
	const std::string& GetActionName() { return m_Data.Name; }
	int& GetManaCost() { return m_Data.ManaCost; }
	BuffSkills& GetData() { return m_Data; }
private:
	BuffSkills m_Data;

};
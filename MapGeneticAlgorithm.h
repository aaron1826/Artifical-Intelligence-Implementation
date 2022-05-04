#pragma once
#include <vector>
#include "MapTile.h"
#include "Game.h"
#include <stdlib.h>
#include <string>
using namespace rapidjson;

struct Map
{
	float fitnessValue = 0.0f;
	float probability = 0.0f;
	std::vector<MapTile*> map;
};

class MapGeneticAlgorithm
{
public:

	void GeneticAlgorithm();
	std::vector<MapTile*>& GetFinalMap() { return m_FinalMap.map; }
	DirectX::XMINT2& GetTileDims() { return m_TileDims; }
	DirectX::XMINT2& GetMapLimit() { return m_MapLimit; }

private:
	void ReserverContainerSpace();
	void InitialPopulation();
	void InitPlaceholderMaps();

	void GenerateMap();
	void GenerateBlank(Map& map);
	void CalculateFitness(Map& maps);
	void CalculateProbability();

	int RandTile();
	int RandMapTile();
	int RandNumber();
	float CrossoverChance();
	float MutationChance();
	float SelectionChance();

	void Crossover();
	void Mutate();
	void SelectParents();
	void RemoveLeastFit();

	void FinalMap();
	void AverageFitness();

	//Hold on to the tile dims for setups
	DirectX::XMINT2 m_TileDims = { 0, 0 };
	DirectX::XMINT2 m_MapLimit = { 0, 0 };


	Map m_Maps;
	Map m_Parent1;
	Map m_Parent2;
	Map m_OffSpring;
	Map m_FinalMap;
	Map m_WorstMap;
	std::vector<Map> m_Population;

	int m_TileCount = 0;
	int m_Iterations = 50;
	int m_BasePopulationSize = 100;
	float m_CrossoverRate = 0.8f;
	float m_MutationRate = 0.5f;
	float m_AverageFitness = 0.0f;
	bool parent1Selected = false;
	bool parent2Selected = false;

};


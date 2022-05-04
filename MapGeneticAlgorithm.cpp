#include "MapGeneticAlgorithm.h"
#include "TilemapUtils.h"

using namespace TiledLoaders;

void MapGeneticAlgorithm::ReserverContainerSpace()
{
	m_Population.reserve(10000);
}

void MapGeneticAlgorithm::InitialPopulation()
{
	for (int i(0); i < m_BasePopulationSize; ++i)
	{
		GenerateMap();
		CalculateFitness(m_Maps);
		m_Population.push_back(m_Maps);
		m_Maps.map.clear();
	}
}

int MapGeneticAlgorithm::RandTile()
{
	int randTile = rand() % m_TileCount + 1;

	return randTile;
}

int MapGeneticAlgorithm::RandMapTile()
{
	int randTile = rand() % 2500;
	return randTile;
}

int MapGeneticAlgorithm::RandNumber()
{
	int num = rand() % 2500;
	return num;
}

float MapGeneticAlgorithm::CrossoverChance()
{
	float chance = (float)rand() /RAND_MAX;
	return chance;
}

float MapGeneticAlgorithm::MutationChance()
{
	float chance = (float)rand() / RAND_MAX;
	return chance;
}

float MapGeneticAlgorithm::SelectionChance()
{
	float chance = (float)rand() / RAND_MAX;
	return chance;
}

void MapGeneticAlgorithm::Crossover()
{
	if (parent1Selected && parent2Selected)
	{
		m_OffSpring.map.clear();

		for (int i(0); i < m_Parent1.map.size() / 2; ++i)
		{
			m_OffSpring.map.push_back(m_Parent1.map[i]);
		}

		for (int i(0); i < m_Parent2.map.size() / 2; ++i)
		{
			m_OffSpring.map.push_back(m_Parent2.map[i + m_Parent2.map.size() / 2]);
		}

		m_Parent1.map.clear();
		m_Parent2.map.clear();
		GenerateBlank(m_Parent1);
		GenerateBlank(m_Parent2);
	}

}

void MapGeneticAlgorithm::Mutate()
{
	if (parent1Selected && parent2Selected)
	{
		int NumberOfTilesToChange = RandNumber();

		//JSON Setup

		//Create Document to hold JSON data & parse data
		Document doc;
		ParseNewJSONDocument(doc, std::string("data/sprites/tilemaps/Tilemap_00.json"));

		//Assert all the members we are looking for are present
		AssertBasicTilemapMembers(doc);

		//Grab hold of each sub array
		const Value& tileArr = doc["tilesets"][0];

		// Build Objects 

		//Generate some simple, singular frames for each tile mathematically
		std::vector<RECT> frames;
		frames.reserve(tileArr["tilecount"].GetUint());
		GenerateFrameData(doc, 0, frames);

		//Extract & populate container with tile specific data for assignment
		std::unordered_map<int, TileProperties> mapProperties;
		LoadTilePropertiesData(doc, 0, mapProperties);

		//Get the name for the tileset (This should be the same name that is given at TextureLoad time)
		std::string mapName = tileArr["name"].GetString();
		//Get texture pointers for main texture and grid texture
		std::shared_ptr<SpriteTexture> texPtr = Game::GetGame()->GetAssetManager().GetSpriteTextureData(mapName);
		std::shared_ptr<SpriteTexture> gridPtr = Game::GetGame()->GetAssetManager().GetSpriteTextureData(std::string("UI_Atlas_01"));


		for (int i(0); i < NumberOfTilesToChange; ++i)
		{
			int TileChanging = RandMapTile();

			//Create new object and pass texture
			MapTile* tile = new MapTile(texPtr, gridPtr);

			//Get the appropriate tile ID using the data array value
			unsigned int tileID = RandTile();

			//Use tileID rotation flags to set rotation (if required)
			tile->GetPrimarySprite().SetRotation(GetTileRotationValue(tileID));

			//Iterate through container and find this tiles unique properties (- 1 because tiled offsets awkwardly)
			std::unordered_map<int, TileProperties>::iterator it = mapProperties.find(tileID - 1);
			if (it != mapProperties.end())
				tile->SetTileProperties(it->second);

			//Decrement the ID to align it with frames container
			--tileID;
			//Use ID to set correct RECT
			tile->GetPrimarySprite().SetRect(frames.at(tileID));
			//The tile might need rotating, so center the origin
			tile->GetPrimarySprite().SetOriginToCenter();

			//Set position through row tiling (add an offset to account for origin shift)
			tile->GetPrimarySprite().SetPosition(m_OffSpring.map[TileChanging]->GetPrimarySprite().GetPosition());


			//Set the tiles x/y coordinates in world space
			tile->SetMapCoordinates(m_OffSpring.map[TileChanging]->GetMapCoordinates());
			tile->SetMapCoordinateMax(m_MapLimit);

			//Configure the secondary sprite
			tile->MirrorTileToGridData();
			tile->GetGridSprite().SetFrame(21);
			tile->GetGridSprite().SetOriginToCenter();
			tile->GetGridSprite().SetColour(DirectX::XMVECTOR{ 0.5f, 0.5f, 0.5f, 0.5f });

			m_OffSpring.map[TileChanging] = tile;
		}
	}


}

void MapGeneticAlgorithm::GeneticAlgorithm()
{
	ReserverContainerSpace();
	InitialPopulation();
	InitPlaceholderMaps();

	int count = 0;
	while (count < m_Iterations)
	{

		CalculateProbability();

		if (CrossoverChance() < m_CrossoverRate)
		{
			SelectParents();
			Crossover();
		}

		if (MutationChance() < m_MutationRate)
		{
			Mutate();
		}

		if (parent1Selected && parent2Selected)
		{
			CalculateFitness(m_OffSpring);
			m_Population.push_back(m_OffSpring);
			CalculateProbability();
			m_OffSpring.map.clear();
			parent1Selected = false;
			parent2Selected = false;
		}

		RemoveLeastFit();
		count++;
	}
	AverageFitness();
	FinalMap();
}

void MapGeneticAlgorithm::CalculateProbability()
{
	float totalFitness = 0.0f;

	for (int i(0); i < m_Population.size(); ++i)
	{
		totalFitness += m_Population[i].fitnessValue;
	}

	for (int i(0); i < m_Population.size(); ++i)
	{
		m_Population[i].probability = m_Population[i].fitnessValue / totalFitness;
	}
}

void MapGeneticAlgorithm::GenerateMap()
{
	//JSON Setup
	

	//Create Document to hold JSON data & parse data
	Document doc;
	ParseNewJSONDocument(doc, std::string("data/sprites/tilemaps/Tilemap_00.json"));

	//Assert all the members we are looking for are present
	AssertBasicTilemapMembers(doc);

	//Grab hold of each sub array
	const Value& tileArr = doc["tilesets"][0];

	// Build Objects 

	//Generate some simple, singular frames for each tile mathematically
	std::vector<RECT> frames;
	frames.reserve(tileArr["tilecount"].GetUint());
	GenerateFrameData(doc, 0, frames);

	//Extract & populate container with tile specific data for assignment
	std::unordered_map<int, TileProperties> mapProperties;
	LoadTilePropertiesData(doc, 0, mapProperties);

	//Get the name for the tileset (This should be the same name that is given at TextureLoad time)
	std::string mapName = tileArr["name"].GetString();
	//Get texture pointers for main texture and grid texture
	std::shared_ptr<SpriteTexture> texPtr = Game::GetGame()->GetAssetManager().GetSpriteTextureData(mapName);
	std::shared_ptr<SpriteTexture> gridPtr = Game::GetGame()->GetAssetManager().GetSpriteTextureData(std::string("UI_Atlas_01"));

	//Get the map dimensions to build the map accurately
	int mapRows = doc["height"].GetInt();
	int mapCols = doc["width"].GetInt();
	//Get tile dimensions
	int tileWidth = doc["tilewidth"].GetInt();
	int tileHeight = doc["tileheight"].GetInt();

	//Store map related information for later
	m_TileDims.x = tileWidth;
	m_TileDims.y = tileHeight;
	m_MapLimit.x = mapCols - 1;
	m_MapLimit.y = mapRows - 1;

	//Store the tile count for the random tile generation
	m_TileCount = tileArr["tilecount"].GetUint() - 4;

	//Start building the map (left->right, top->bot order)
	for (int y(0); y < mapRows; ++y)
	{
		for (int x(0); x < mapCols; ++x)
		{
			//Create new object and pass texture
			MapTile* tile = new MapTile(texPtr, gridPtr);

			//Get the appropriate tile ID using the data array value
			unsigned int tileID = RandTile();

			//Use tileID rotation flags to set rotation (if required)
			tile->GetPrimarySprite().SetRotation(GetTileRotationValue(tileID));

			//Iterate through container and find this tiles unique properties (- 1 because tiled offsets awkwardly)
			std::unordered_map<int, TileProperties>::iterator it = mapProperties.find(tileID - 1);
			if (it != mapProperties.end())
				tile->SetTileProperties(it->second);

			//Decrement the ID to align it with frames container
			--tileID;
			//Use ID to set correct RECT
			tile->GetPrimarySprite().SetRect(frames.at(tileID));
			//The tile might need rotating, so center the origin
			tile->GetPrimarySprite().SetOriginToCenter();

			//Set position through row tiling (add an offset to account for origin shift)
			tile->GetPrimarySprite().SetPosition(
				static_cast<float>((tileWidth * x) + tileWidth * 0.5f),
				static_cast<float>((tileWidth * y) + tileHeight * 0.5f)
			);

			//Set the tiles x/y coordinates in world space
			tile->SetMapCoordinates(x, y);
			tile->SetMapCoordinateMax(m_MapLimit);

			//Configure the secondary sprite
			tile->MirrorTileToGridData();
			tile->GetGridSprite().SetFrame(21);
			tile->GetGridSprite().SetOriginToCenter();
			tile->GetGridSprite().SetColour(DirectX::XMVECTOR{ 0.5f, 0.5f, 0.5f, 0.5f });

			m_Maps.map.push_back(tile);
		}
	}

	m_Maps.fitnessValue = 0.0;
	m_Maps.probability = 0.0f;
}

void MapGeneticAlgorithm::GenerateBlank(Map& map)
{
	//
	// JSON Setup
	//

	//Create Document to hold JSON data & parse data
	Document doc;
	ParseNewJSONDocument(doc, std::string("data/sprites/tilemaps/Tilemap_00.json"));

	//Assert all the members we are looking for are present
	AssertBasicTilemapMembers(doc);

	//Grab hold of each sub array
	const Value& tileArr = doc["tilesets"][0];

	//
	// Build Objects 
	//

	//Generate some simple, singular frames for each tile mathematically
	std::vector<RECT> frames;
	frames.reserve(tileArr["tilecount"].GetUint());
	GenerateFrameData(doc, 0, frames);

	//Extract & populate container with tile specific data for assignment
	std::unordered_map<int, TileProperties> mapProperties;
	LoadTilePropertiesData(doc, 0, mapProperties);

	//Get the name for the tileset (This should be the same name that is given at TextureLoad time)
	std::string mapName = tileArr["name"].GetString();
	//Get texture pointers for main texture and grid texture
	std::shared_ptr<SpriteTexture> texPtr = Game::GetGame()->GetAssetManager().GetSpriteTextureData(mapName);
	std::shared_ptr<SpriteTexture> gridPtr = Game::GetGame()->GetAssetManager().GetSpriteTextureData(std::string("UI_Atlas_01"));

	//Get the map dimensions to build the map accurately
	int mapRows = doc["height"].GetInt();
	int mapCols = doc["width"].GetInt();
	//Get tile dimensions
	int tileWidth = doc["tilewidth"].GetInt();
	int tileHeight = doc["tileheight"].GetInt();

	//Store map related information for later
	m_TileDims.x = tileWidth;
	m_TileDims.y = tileHeight;
	m_MapLimit.x = mapCols - 1;
	m_MapLimit.y = mapRows - 1;

	//counter for "data" sub-array
	int count = 0;
	//Start building the map (left->right, top->bot order)
	for (int y(0); y < mapRows; ++y)
	{
		for (int x(0); x < mapCols; ++x)
		{
			//Create new object and pass texture
			MapTile* tile = new MapTile(texPtr, gridPtr);

			//Get the appropriate tile ID using the data array value
			unsigned int tileID = 12;

			//Use tileID rotation flags to set rotation (if required)
			tile->GetPrimarySprite().SetRotation(GetTileRotationValue(tileID));

			//Iterate through container and find this tiles unique properties (- 1 because tiled offsets awkwardly)
			std::unordered_map<int, TileProperties>::iterator it = mapProperties.find(tileID - 1);
			if (it != mapProperties.end())
				tile->SetTileProperties(it->second);

			//Decrement the ID to align it with frames container
			--tileID;
			//Use ID to set correct RECT
			tile->GetPrimarySprite().SetRect(frames.at(tileID));
			//The tile might need rotating, so center the origin
			tile->GetPrimarySprite().SetOriginToCenter();

			//Set position through row tiling (add an offset to account for origin shift)
			tile->GetPrimarySprite().SetPosition(
				static_cast<float>((tileWidth * x) + tileWidth * 0.5f),
				static_cast<float>((tileWidth * y) + tileHeight * 0.5f)
			);

			//Set the tiles x/y coordinates in world space
			tile->SetMapCoordinates(x, y);
			tile->SetMapCoordinateMax(m_MapLimit);

			//Configure the secondary sprite
			tile->MirrorTileToGridData();
			tile->GetGridSprite().SetFrame(21);
			tile->GetGridSprite().SetOriginToCenter();
			tile->GetGridSprite().SetColour(DirectX::XMVECTOR{ 0.5f, 0.5f, 0.5f, 0.5f });

			map.map.push_back(tile);
		}
	}

	map.fitnessValue = 0.0f;
	map.probability = 0.0f;
}

void MapGeneticAlgorithm::FinalMap()
{
	for (int i(0); i < m_Population.size(); ++i)
	{
		if (m_Population[i].fitnessValue > m_FinalMap.fitnessValue)
			m_FinalMap = m_Population[i];
	}

	for (auto& t : m_FinalMap.map)
	{
		//Get coords
		DirectX::XMINT2& coords = t->GetMapCoordinates();

		//Do some checking to verify what neighbours should be added
		if (coords.y > 0)				//North Check
			t->SetNeighbourAtIndex(0, m_FinalMap.map.at((coords.x + (coords.y - 1)) + ((coords.y - 1) * m_MapLimit.y)));
		if (coords.x < m_MapLimit.x)	//East Check
			t->SetNeighbourAtIndex(1, m_FinalMap.map.at((coords.x + 1 + coords.y) + (coords.y * m_MapLimit.y)));
		if (coords.y < m_MapLimit.y)	//South Check
			t->SetNeighbourAtIndex(2, m_FinalMap.map.at((coords.x + (coords.y + 1)) + ((coords.y + 1) * m_MapLimit.y)));
		if (coords.x > 0)				//West Check
			t->SetNeighbourAtIndex(3, m_FinalMap.map.at((coords.x - 1 + coords.y) + (coords.y * m_MapLimit.y)));

	}
}

void MapGeneticAlgorithm::AverageFitness()
{
	float totalFitness = 0.0f;
	for (int i(0); i < m_Population.size(); ++i)
	{
		totalFitness += m_Population[i].fitnessValue;
	}
	 
	m_AverageFitness += totalFitness / m_Population.size();
}

void MapGeneticAlgorithm::InitPlaceholderMaps()
{
	GenerateBlank(m_Parent1);
	GenerateBlank(m_Parent2);
	GenerateBlank(m_OffSpring);
	GenerateBlank(m_FinalMap);
	GenerateBlank(m_WorstMap);
}

void MapGeneticAlgorithm::CalculateFitness(Map& maps)
{
	float fitness = 0;
	for (int i(0); i < maps.map.size(); ++i)
	{
		if (maps.map[i]->GetTileProperties().terrainTypeID == 0 && maps.map[i]->GetTileProperties().impassable == false)
			fitness = fitness + 1.0;
		if (maps.map[i]->GetTileProperties().terrainTypeID == 1 && maps.map[i]->GetTileProperties().impassable == false)
			fitness = fitness + 0.5;
		if (maps.map[i]->GetTileProperties().terrainTypeID == 0 && maps.map[i]->GetTileProperties().impassable == true)
			fitness = fitness - 1.0;
		if (maps.map[i]->GetTileProperties().terrainTypeID == 1 && maps.map[i]->GetTileProperties().impassable == true)
			fitness = fitness - 0.5;
		if (maps.map[i]->GetTileProperties().moveCost < 1)
			fitness = fitness + 0.5;
		if (maps.map[i]->GetTileProperties().moveCost > 1)
			fitness = fitness - 0.5;
	}
	maps.fitnessValue = fitness / maps.map.size();
}

void MapGeneticAlgorithm::SelectParents()
{

	for (int i(0); i < m_Population.size(); ++i)
	{
		if (m_Population[i].probability > m_Parent1.probability)
		{
			m_Parent1 = m_Population[i];
			parent1Selected = true;
		}
	}

	for (int i(0); i < m_Population.size(); ++i)
	{
		if (m_Population[i].probability > m_Parent2.probability && m_Population[i].probability != m_Parent1.probability)
		{
			m_Parent2 = m_Population[i];
			parent2Selected = true;
		}
	}
}

void MapGeneticAlgorithm::RemoveLeastFit()
{
	m_WorstMap.fitnessValue = 1;

	for (int i(0); i < m_Population.size(); ++i)
	{
		if (m_Population[i].fitnessValue < m_WorstMap.fitnessValue)
			m_WorstMap = m_Population[i];
	}

	for (int i(0); i < m_Population.size(); ++i)
	{
		if(m_WorstMap.fitnessValue == m_Population[i].fitnessValue)
			m_Population.erase(m_Population.cbegin() + i);
	}
}

#pragma once
#include "All_Includes.h"
#include "D3DUtils_Debug.h"

#include "MathHelper.h"

//
// Suite of DX & Game related utilities, support types functions etc. for use.
// "All_Includes" or equiv should be placed here, and just include this file where required.
//


//-------------------------------------------------------------------------------
// Type Defininitions
//-------------------------------------------------------------------------------

typedef DirectX::SimpleMath::Vector2 v2;
typedef DirectX::SimpleMath::Vector3 v3;
typedef DirectX::SimpleMath::Vector4 v4;

//-------------------------------------------------------------------------------
// Program Constants
//-------------------------------------------------------------------------------

// Define the maximum about of lights that can be in the scene (mirror in shader)
#define MaxLights 16

//PI is always helpful (and tasty!)
const static float PI = 3.1415926535f;
//Gravity Constant
const static float GRAVITY = 9.81f;
const int NUM_FRAME_RESOURCES = 3;

//Common rotations
const float ROTATION_90 = PI / 2.f;
const float ROTATION_180 = PI;
const float ROTATION_270 = PI * 1.5f;


//-------------------------------------------------------------------------------
// Geometry, Textures & Materials Structures + Functions
//-------------------------------------------------------------------------------

//Describes an animation (or single frame if required) on a spritesheet.
//Values relate to an index in a RECT array.
struct FrameAnimation
{
	FrameAnimation()
		:animName("NONE"), animStart(0), animEnd(0), animSpeed(0)
	{}

	std::string animName;
	short int animStart;
	short int animEnd;
	short int animSpeed;

	//Get the number of frames in the current animation
	short int GetAnimFrameCount() { return animEnd - animStart; }
};

//More specialised Sprite Texture Container. Holds more information that we might typically
//need in sprite work
struct SpriteTexture
{
	std::vector<RECT> spriteFrames;
	std::vector<FrameAnimation> animations;
	std::wstring fileName;
	size_t srvIndex;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = nullptr;
};

//Generates a vector of face normals for an object
void GenerateFaceNormals(std::vector<DirectX::SimpleMath::Vector3>& vertices, std::vector<DirectX::SimpleMath::Vector3>& normals, int numOfHalfQuads);


//-------------------------------------------------------------------------------
// Lighting & Camera Structures + Functions
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
// Supporting Types/Structures
//-------------------------------------------------------------------------------

struct CharacterClass
{
	std::string ClassName;
	int UniqueID;
	int HP;
	int MP;
	int Movespeed;
	int PhysAttack;
	int MagAttack;
	int PhysHit;
	int MagHit;
	int CritHit;
	float CritMultiplier;
	int PhysArmour;
	int MagArmour;
	int Evasion;
	int MagResist;
};

struct Equipment 
{
	std::string EquipmentName;
	int EquipmentTypeID;
	int UniqueEquipmentID;
	int PhysAttack;
	int MagAttack;
	int PhysHit;
	int MagHit;
	int CritHit;
	float CritMultiplier;
	int HP;
	int MP;
	int PhysArmour;
	int MagArmour;
	int Evasion;
	int Movespeed;
	int MagResist;
};

struct DamageSkills
{
	int UniqueID;
	std::string Name;
	int SkillType;
	int DamageType;
	int ManaCost;
	int Range;
	int Radius;
	float PhysDamageScaling;
	float MagDamageScaling;
	float InnateCrit;
	std::string ToolTip;
};

struct HealSkills
{
	int UniqueID;
	std::string Name;
	int SkillType;
	int ManaCost;
	int Range;
	int Radius;
	float HealPercent;
	std::string ToolTip;
};

struct BuffSkills
{
	int UniqueID;
	std::string Name;
	int SkillType;
	int ManaCost;
	int TurnCount;
	int Range;
	int Radius;
	float PhysDamageBuff;
	float MagDamageBuff;
	float PhysHitBuff;
	float MagHitBuff;
	float PhysArmourBuff;
	float MagArmourBuff;
	float MagResistBuff;
	float CritHitBuff;
	float CritDamageBuff;
	float EvasionBuff;
	float MovespeedBuff;
	std::string ToolTip;
};
//-------------------------------------------------------------------------------
// Shader Constant Buffer Containers
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
// Mathematical Helpers/Functions/Types
//-------------------------------------------------------------------------------

//Get random value from range
float GetRandFromRange(float min, float max);

//Get random seed
void SeedRandom(int seed);

//Convert Degrees into Radian
inline float DegreesToRadian(float x)
{
	return (x * PI) / 180.0f;
}

//Convert Radian into Degrees
inline float RadianToDegrees(float x)
{
	return (x * 180.0f) / PI;
}

//Linear Interpolation
float Lerp(float start, float end, float time);
//update the aspect ratio and recompute the projection matrix.
void CreateProjectionMatrix(DirectX::SimpleMath::Matrix& projM, float fieldOfView,
	float aspectRatio, float nearZ, float farZ);

//the view matrix represents the camera
void CreateViewMatrix(DirectX::SimpleMath::Matrix& viewM, const DirectX::SimpleMath::Vector3& camPos,
	const DirectX::SimpleMath::Vector3& camTgt, const DirectX::SimpleMath::Vector3& camUp);

//Invert a matrix
DirectX::SimpleMath::Matrix InverseTranspose(const DirectX::SimpleMath::Matrix& m);



//-------------------------------------------------------------------------------
// Timer Types/Functions
//-------------------------------------------------------------------------------

////////////////////////////
/// Game Timer Version 1 ///
////////////////////////////

//Get the current game run time
float GetClock();
//Get the elapsed frame time
float GetDeltaTime();
//Add time to clock (add dTime).
void AddSecToClock(float sec);
//If we want to reset or manually alter the game time.
void SetClockTime(float value);

////////////////////////////
/// Game Timer Version 2 ///
////////////////////////////



//-------------------------------------------------------------------------------
// D3D Support Types/Functions
//-------------------------------------------------------------------------------

//Simplified structure for 2D Usage (No need to manage Shader Constants for now)
struct FrameResource2D
{
public:

	FrameResource2D(ID3D12Device* device);
	FrameResource2D(const FrameResource2D& rhs) = delete;
	FrameResource2D& operator=(const FrameResource2D& rhs) = delete;
	~FrameResource2D();
	// We cannot reset the allocator until the GPU is done processing the commands.
	// So each frame needs their own allocator.
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	// Fence value to mark commands up to this fence point.  This lets us
	// check if these frame resources are still in use by the GPU.
	UINT64 Fence = 0;

};


//Forward Dec
class Game;

//Wraps up common data for passing to modes or states (expand as required)
struct PassData
{
	std::unique_ptr<DirectX::SpriteBatch> sceneSB;
	std::unique_ptr<DirectX::SpriteBatch> uiSB;
	std::unique_ptr<DirectX::SpriteFont> primarySF;
	std::unique_ptr<DirectX::SpriteFont> secondarySF;
	DirectX::DescriptorHeap* heap = nullptr;
};

/*
	Macro for releasing COM Objects (aka Resources).
	If !null, then release and null. Func/Data that is created by
	another system and will not go till released properly.
	Saves typing and silly bugs.
*/
#define ReleaseCOM(x) \
{	if(x){	x->Release();	x = 0;	} }	



//-------------------------------------------------------------------------------
// Miscellaneous / Undefined Categories
//-------------------------------------------------------------------------------

/*
	We like colour definitions for things, so here are some predefined colours
	for use here (Expand as needed, most are based on webcolours).
*/
namespace Colours
{
	const DirectX::SimpleMath::Vector4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 DimGray = { 0.412f, 0.412f, 0.412f, 1.0f };
	const DirectX::SimpleMath::Vector4 AliceBlue = { 0.941f, 0.972f, 1.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 AntiqueWhite = { 0.98f, 0.922f, 0.843f, 1.0f };
	const DirectX::SimpleMath::Vector4 Aquamarine = { 0.5f, 1.0f, 0.831f, 1.0f };
	const DirectX::SimpleMath::Vector4 Crimson = { 0.863f, 0.078f, 0.235f, 1.0f };
	const DirectX::SimpleMath::Vector4 SaddleBrown = { 0.545f, 0.271f, 0.074f, 1.0f };
}

//Convert from Vec3 to Vec4 (DX Types)
inline DirectX::SimpleMath::Vector4 Vec3To4(const DirectX::SimpleMath::Vector3& src, float w)
{
	return DirectX::SimpleMath::Vector4(src.x, src.y, src.z, w);
}

//Convert from Vec4 to Vec3 (DX Types)
inline DirectX::SimpleMath::Vector3 Vec4To3(const DirectX::SimpleMath::Vector4& src)
{
	return DirectX::SimpleMath::Vector3(src.x, src.y, src.z);
}

/*
	Comparing floating point numbers is tricky. A number is rarely zero or one, it's more common for it
	to be 0.001 or 0.998. So we need a way of comparing numbers that takes this into account,
	this simple approach won't work for everything, but it's useful most of the time.
*/
const float VERY_SMALL = 0.0001f;
inline bool Equals(float a, float b)
{
	return (fabs(a - b) < VERY_SMALL) ? true : false;
}

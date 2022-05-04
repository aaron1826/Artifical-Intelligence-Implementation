#include "D3DUtils.h"
#include "D3D.h"
#include <time.h>       //time function
#include <sstream>      //Drawing FPS

using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

//-------------------------------------------------------------------------------
// Geometry, Textures & Materials Structures + Functions
//-------------------------------------------------------------------------------

void GenerateFaceNormals(std::vector<DirectX::SimpleMath::Vector3>& vertices, std::vector<DirectX::SimpleMath::Vector3>& normals, int numOfHalfQuads)
{
    //make some face normals
    for (int i = 0; i < numOfHalfQuads; ++i)
    {
        int idx = i * 3;
        Vector3 a(vertices[idx] - vertices[idx + 1]), b(vertices[idx + 2] - vertices[idx + 1]);
        a.Normalize();
        b.Normalize();
        normals[idx] = normals[idx + 1] = normals[idx + 2] = b.Cross(a);
    }
}

//-------------------------------------------------------------------------------
// Lighting & Camera Structures + Functions
//-------------------------------------------------------------------------------



//-------------------------------------------------------------------------------
// Supporting Types/Structures
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
// Mathematical Helpers/Functions/Types
//-------------------------------------------------------------------------------


float GetRandFromRange(float min, float max)
{
    float res = (float)rand() / RAND_MAX;
    res = min + res * (max - min);
    return res;
}

void SeedRandom(int seed /*= -1*/)
{
    if (seed == -1)
        seed = (int)time(nullptr);

    srand(seed);
}

float Lerp(float start, float end, float time)
{
    return start + (end - start) * time;
}

void CreateProjectionMatrix(DirectX::SimpleMath::Matrix& projM, float fieldOfView, float aspectRatio, float nearZ, float farZ)
{
    projM = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearZ, farZ);
}


void CreateViewMatrix(DirectX::SimpleMath::Matrix& viewM, const DirectX::SimpleMath::Vector3& camPos, const DirectX::SimpleMath::Vector3& camTgt, const DirectX::SimpleMath::Vector3& camUp)
{
    viewM = XMMatrixLookAtLH(camPos, camTgt, camUp);
}

Matrix InverseTranspose(const Matrix& m)
{
    // Inverse-transpose is just applied to normals.  So zero out 
    // translation row so that it doesn't get into our inverse-transpose
    // calculation--we don't want the inverse-transpose of the translation.
    XMMATRIX A = m;
    A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    XMVECTOR det = XMMatrixDeterminant(A);
    return XMMatrixTranspose(XMMatrixInverse(&det, A));
}



//-------------------------------------------------------------------------------
// Timer Functions
//-------------------------------------------------------------------------------

static float gameTime = 0, frameElapsed = 0;

//Get the current game run time
float GetClock()
{
    return gameTime;
}
//Get the elapsed frame time
float GetDeltaTime()
{
    return frameElapsed;
}
//Add time to clock (add dTime).
void AddSecToClock(float sec)
{
    gameTime += sec;
    frameElapsed = sec;
}
//If we want to reset or manually alter the game time.
void SetClockTime(float value)
{
    gameTime = value;
}



//-------------------------------------------------------------------------------
// Miscellaneous / Undefined Categories
//-------------------------------------------------------------------------------


FrameResource2D::FrameResource2D(ID3D12Device* device)
{
    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
}

FrameResource2D::~FrameResource2D()
{

}

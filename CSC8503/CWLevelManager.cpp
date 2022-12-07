#include "RenderObject.h"
#include "TextureLoader.h"
#include "CWLevelManager.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

NCL::CSC8503::CWLevelManager::CWLevelManager(GameWorld& gWorld, GameTechRenderer& gRenderer) : world(gWorld), renderer(gRenderer)
{
	InitAssets();
	InitGoatWorld();
}

NCL::CSC8503::CWLevelManager::~CWLevelManager()
{
	delete cubeMesh;
	delete highRise3;
	delete highRise9;
	delete highRise12;
	delete highRise13;
	delete highRise18;

	delete basicTex;
	delete whiteTex;
	delete conceptTex;
	delete basicShader;
}

void NCL::CSC8503::CWLevelManager::InitGoatWorld()
{
	InitBaseObjects();
	InitSideWalks();
	InitBuildings();
}

void NCL::CSC8503::CWLevelManager::InitAssets()
{
	cubeMesh = renderer.LoadMesh("cube.msh");
	highRise3 = renderer.LoadMesh("Highrise_3.msh");
	highRise9 = renderer.LoadMesh("Highrise_9.msh");
	highRise12 = renderer.LoadMesh("Highrise_12.msh");
	highRise13 = renderer.LoadMesh("Highrise_13.msh");
	highRise18 = renderer.LoadMesh("Highrise_18.msh");

	basicTex = renderer.LoadTexture("checkerboard.png");
	whiteTex = renderer.LoadTexture("WhiteTex.png");
	conceptTex = renderer.LoadTexture("ConceptTexture.jpg");

	basicShader = renderer.LoadShader("scene.vert", "scene.frag");
}

void NCL::CSC8503::CWLevelManager::InitBaseObjects()
{
	AddCube(Vector3(0, 0, 0), Vector3(512.0f, 2.0f, 512.0f), Vector3(0, 0, 0), 0, Vector4(0.2f, 0.2f, 0.2f, 1.0f), "Floor", whiteTex);

	AddCube(Vector3(0, 64.0f, -512.0f), Vector3(512.0f, 64.0f, 2.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.8f, 0.6f, 1.0f), "Wall.B", whiteTex);
	AddCube(Vector3(0, 64.0f, 512.0f), Vector3(512.0f, 64.0f, 2.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.8f, 0.6f, 1.0f), "Wall.F", whiteTex);
	AddCube(Vector3(-512.0f, 64.0f, 0.0f), Vector3(2.0f, 64.0f, 512.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.8f, 0.6f, 1.0f), "Wall.L", whiteTex);
	AddCube(Vector3(512.0f, 64.0f, 0.0f), Vector3(2.0f, 64.0f, 512.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.8f, 0.6f, 1.0f), "Wall.R", whiteTex);
}

void NCL::CSC8503::CWLevelManager::InitSideWalks()
{
	AddCube(Vector3(0.0, 1.8f, -482.0f), Vector3(512.0f, 2.0f, 32.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk01", whiteTex);
	AddCube(Vector3(0.0, 1.8f, 482.0f), Vector3(512.0f, 2.0f, 32.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk02", whiteTex);
	AddCube(Vector3(482.0, 1.8f, 0.0f), Vector3(32.0f, 2.0f, 450.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk03", whiteTex);
	AddCube(Vector3(-482.0, 1.8f, 0.0f), Vector3(32.0f, 2.0f, 450.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);

	AddCube(Vector3(-256.0, 1.8f, 0.0f), Vector3(100.0f, 2.0f, 352.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);
	AddCube(Vector3(256.0, 1.8f, 256.0f), Vector3(100.0f, 2.0f, 100.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);
	AddCube(Vector3(256.0, 1.8f, 0.0f), Vector3(100.0f, 2.0f, 100.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);
	AddCube(Vector3(256.0, 1.8f, -256.0f), Vector3(100.0f, 2.0f, 100.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);

	AddCube(Vector3(0.0, 1.8f, -200.0f), Vector3(100.0f, 2.0f, 150.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);
}

void NCL::CSC8503::CWLevelManager::InitBuildings()
{
	AddBuilding(Vector3(256.0f, 2.5f, -300.0f), Vector3(17.0f, 17.0f, 17.0f), Vector3(0, -90.0f, 0), 0, highRise13, conceptTex);
	AddBuilding(Vector3(300.0f, 2.5f, -200.0f), Vector3(16.0f, 16.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);
	AddBuilding(Vector3(216.0f, 2.5f, -200.0f), Vector3(16.0f, 16.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);

	AddBuilding(Vector3(300.0f, 2.5f, 0.0f), Vector3(25.0f, 25.0f, 25.0f), Vector3(0, 0.0f, 0), 0, highRise12, conceptTex);
	AddBuilding(Vector3(200.0f, 2.5f, 50.0f), Vector3(15.0f, 15.0f, 15.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddBuilding(Vector3(200.0f, 2.5f, -50.0f), Vector3(30.0f, 30.0f, 30.0f), Vector3(0, 0.0f, 0), 0, highRise3, conceptTex);

	AddBuilding(Vector3(-300.0f, 2.5f, 0.0f), Vector3(25.0f, 25.0f, 25.0f), Vector3(0, 0.0f, 0), 0, highRise12, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, -120.0f), Vector3(30.0f, 20.0f, 30.0f), Vector3(0, 0.0f, 0), 0, highRise3, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, -200.0f), Vector3(30.0f, 30.0f, 30.0f), Vector3(0, 0.0f, 0), 0, highRise3, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, -280.0f), Vector3(15.0f, 15.0f, 15.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, 0.0f), Vector3(15.0f, 20.0f, 15.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, 180.0f), Vector3(18.0f, 18.0f, 18.0f), Vector3(0, 0.0f, 0), 0, highRise13, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, 300.0f), Vector3(16.0f, 16.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);
	AddBuilding(Vector3(-300.0f, 2.5f, 300.0f), Vector3(16.0f, 10.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);
	AddBuilding(Vector3(-300.0f, 2.5f, 180.0f), Vector3(16.0f, 15.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddBuilding(Vector3(-300.0f, 2.5f, -200.0f), Vector3(18.0f, 18.0f, 18.0f), Vector3(0, 0.0f, 0), 0, highRise13, conceptTex);

	//Petrol Pump
	AddCube(Vector3(0.0, 15.0f, -300.0f), Vector3(64.0f, 24.0f, 32.0f), Vector3(0, 0, 0), 0, Debug::WHITE, "PumpBuildingBase", whiteTex);
	AddCube(Vector3(0.0, 40.0f, -300.0f), Vector3(75.0f, 2.5f, 40.0f), Vector3(0, 0, 0), 0, Debug::RED, "PumpBuildingRoof", whiteTex);

	AddCube(Vector3(0.0, 4.5f, -200.0f), Vector3(40.0f, 1.0f, 15.0f), Vector3(0, 0, 0), 0, Debug::WHITE, "PumpLBase", whiteTex);
	AddCube(Vector3(0.0, 4.5f, -100.0f), Vector3(40.0f, 1.0f, 15.0f), Vector3(0, 0, 0), 0, Debug::WHITE, "PumpRBase", whiteTex);

	AddCube(Vector3(30.0, 26.0f, -200.0f), Vector3(3.0f, 24.0f, 3.0f), Vector3(0, 0, 0), 0, Debug::WHITE, "PumpL_PillarL", whiteTex);
	AddCube(Vector3(-30.0, 26.0f, -200.0f), Vector3(3.0f, 24.0f, 3.0f), Vector3(0, 0, 0), 0, Debug::WHITE, "PumpL_PillarR", whiteTex);

	AddCube(Vector3(30.0, 26.0f, -100.0f), Vector3(3.0f, 24.0f, 3.0f), Vector3(0, 0, 0), 0, Debug::WHITE, "PumpR_PillarL", whiteTex);
	AddCube(Vector3(-30.0, 26.0f, -100.0f), Vector3(3.0f, 24.0f, 3.0f), Vector3(0, 0, 0), 0, Debug::WHITE, "PumpR_PillarR", whiteTex);

	AddCube(Vector3(0.0, 50.0f, -150.0f), Vector3(40.0f, 2.5f, 75.0f), Vector3(0, 0, 0), 0, Debug::RED, "PumpRoof", whiteTex);
}

void NCL::CSC8503::CWLevelManager::AddCube(const Vector3& cubePos, const Vector3& cubeSize, const Vector3& cubeRot, const float& cubeMass, const Vector4& cubeColour, const std::string& cubeName, TextureBase* cubeTex)
{
	GameObject* cube = new GameObject(1, cubeName);

	AABBVolume* volume = new AABBVolume(cubeSize);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(cubePos)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(cubeRot.x, cubeRot.y, cubeRot.z))
		.SetScale(cubeSize * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, cubeTex, basicShader));
	cube->GetRenderObject()->SetColour(cubeColour);
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(cubeMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(cube);
}

void NCL::CSC8503::CWLevelManager::AddBuilding(const Vector3& buildingPos, const Vector3& buildingSize, const Vector3& buildingRot, const float& buildingMass, MeshGeometry* buildingMesh, TextureBase* buildingTex)
{
	GameObject* building = new GameObject(1, "Building");

	AABBVolume* volume = new AABBVolume(buildingSize);
	building->SetBoundingVolume((CollisionVolume*)volume);

	building->GetTransform()
		.SetPosition(buildingPos)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(buildingRot.x, buildingRot.y, buildingRot.z))
		.SetScale(buildingSize * 2);

	building->SetRenderObject(new RenderObject(&building->GetTransform(), buildingMesh, buildingTex, basicShader));
	building->GetRenderObject()->SetColour(Debug::WHITE);
	building->SetPhysicsObject(new PhysicsObject(&building->GetTransform(), building->GetBoundingVolume()));

	building->GetPhysicsObject()->SetInverseMass(buildingMass);
	building->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(building);
}
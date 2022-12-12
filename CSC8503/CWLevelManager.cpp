#include "RenderObject.h"
#include "TextureLoader.h"
#include "CWLevelManager.h"
#include "CWJumpPad.h"
#include "CWPropDestroy.h"
#include "CWMazeTrigger.h"
#include "CWPawn.h"
#include "CWDude.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

NCL::CSC8503::CWLevelManager::CWLevelManager(GameWorld& gWorld, CWGoatGame& game, GameTechRenderer& gRenderer) : world(gWorld), goatGame(game), renderer(gRenderer)
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

	delete envStreetLight;
	delete envTree2;
	delete envTree3;
	delete envCar;
	delete envBench;

	delete goatMesh;
	delete dudeMesh;
	delete enemyMesh;

	delete basicTex;
	delete propTex;
	delete whiteTex;
	delete conceptTex;
	delete basicShader;
}

void NCL::CSC8503::CWLevelManager::InitGoatWorld()
{
	InitBaseObjects();
	InitSideWalks();
	InitBuildings();
	InitJumpPads();
	InitMaze();
	InitDestroyableProps();
	InitDudeNPC();
}

void NCL::CSC8503::CWLevelManager::InitAssets()
{
	cubeMesh = renderer.LoadMesh("cube.msh");
	highRise3 = renderer.LoadMesh("Highrise_3.msh");
	highRise9 = renderer.LoadMesh("Highrise_9.msh");
	highRise12 = renderer.LoadMesh("Highrise_12.msh");
	highRise13 = renderer.LoadMesh("Highrise_13.msh");
	highRise18 = renderer.LoadMesh("Highrise_18.msh");

	envStreetLight = renderer.LoadMesh("Env_Road_Column.msh");
	envTree2 = renderer.LoadMesh("Env_Type2_Tree_2.msh");
	envTree3 = renderer.LoadMesh("Env_Type3_Tree_2.msh");
	envCar = renderer.LoadMesh("Env_Car_Hatch.msh");
	envBench = renderer.LoadMesh("Env_Bench.msh");

	goatMesh = renderer.LoadMesh("Goat.msh");
	dudeMesh = renderer.LoadMesh("Keeper.msh");
	enemyMesh = renderer.LoadMesh("goose.msh");

	basicTex = renderer.LoadTexture("checkerboard.png");
	whiteTex = renderer.LoadTexture("WhiteTex.png");
	propTex = renderer.LoadTexture("PropTexture.png");
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

	AddCube(Vector3(-256.0, 1.8f, 0.0f), Vector3(100.0f, 2.0f, 352.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk05", whiteTex);
	AddCube(Vector3(300.0, 1.8f, 300.0f), Vector3(150.0f, 2.0f, 150.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk06", whiteTex);		//Maze Walk
	AddCube(Vector3(256.0, 1.8f, 0.0f), Vector3(100.0f, 2.0f, 100.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk07", whiteTex);
	AddCube(Vector3(256.0, 1.8f, -256.0f), Vector3(100.0f, 2.0f, 100.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk08", whiteTex);

	AddCube(Vector3(0.0, 1.8f, -200.0f), Vector3(100.0f, 2.0f, 150.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk09", whiteTex);
}

void NCL::CSC8503::CWLevelManager::InitBuildings()
{
	AddBuilding(Vector3(256.0f, 2.5f, -300.0f), Vector3(17.0f, 17.0f, 17.0f), Vector3(0, -90.0f, 0), 0, highRise13, conceptTex);
	AddInvisibleWall(Vector3(300.0f, 85.0f, -300.0f), Vector3(23.0f, 82.0f, 23.0f));
	AddInvisibleWall(Vector3(210.0f, 85.0f, -300.0f), Vector3(23.0f, 82.0f, 23.0f));

	AddBuilding(Vector3(300.0f, 2.5f, -200.0f), Vector3(16.0f, 16.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);
	AddInvisibleWall(Vector3(300.0f, 100.0f, -200.0f), Vector3(25.0f, 98.0f, 25.0f));

	AddBuilding(Vector3(216.0f, 2.5f, -200.0f), Vector3(16.0f, 16.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);
	AddInvisibleWall(Vector3(216.0f, 100.0f, -200.0f), Vector3(25.0f, 98.0f, 25.0f));

	AddBuilding(Vector3(300.0f, 2.5f, 0.0f), Vector3(25.0f, 25.0f, 25.0f), Vector3(0, 0.0f, 0), 0, highRise12, conceptTex);
	AddInvisibleWall(Vector3(300.0f, 116.0f, 0.0f), Vector3(38.0f, 112.0f, 85.0f));

	AddBuilding(Vector3(215.0f, 2.5f, 50.0f), Vector3(15.0f, 15.0f, 15.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddInvisibleWall(Vector3(215.0f, 72.0f, 50.0f), Vector3(32.0f, 70.0f, 32.0f));

	AddBuilding(Vector3(215.0f, 2.5f, -50.0f), Vector3(30.0f, 30.0f, 30.0f), Vector3(0, 0.0f, 0), 0, highRise3, conceptTex);
	AddInvisibleWall(Vector3(215.0f, 114.0f, -50.0f), Vector3(32.0f, 110.0f, 32.0f));

	AddBuilding(Vector3(-300.0f, 2.5f, 0.0f), Vector3(25.0f, 25.0f, 25.0f), Vector3(0, 0.0f, 0), 0, highRise12, conceptTex);
	AddInvisibleWall(Vector3(-300.0f, 116.0f, 0.0f), Vector3(38.0f, 112.0f, 85.0f));

	AddBuilding(Vector3(-220.0f, 2.5f, -130.0f), Vector3(30.0f, 20.0f, 30.0f), Vector3(0, 0.0f, 0), 0, highRise3, conceptTex);
	AddInvisibleWall(Vector3(-220.0f, 76.0f, -130.0f), Vector3(32.0f, 72.0f, 32.0f));
	AddBuilding(Vector3(-220.0f, 2.5f, -200.0f), Vector3(30.0f, 30.0f, 30.0f), Vector3(0, 0.0f, 0), 0, highRise3, conceptTex);
	AddInvisibleWall(Vector3(-220.0f, 114.0f, -200.0f), Vector3(32.0f, 110.0f, 32.0f));
	
	AddBuilding(Vector3(-220.0f, 2.5f, -280.0f), Vector3(15.0f, 15.0f, 15.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddInvisibleWall(Vector3(-220.0f, 72.0f, -280.0f), Vector3(32.0f, 70.0f, 32.0f));
	
	AddBuilding(Vector3(-220.0f, 2.5f, 0.0f), Vector3(15.0f, 20.0f, 15.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddInvisibleWall(Vector3(-220.0f, 72.0f, 0.0f), Vector3(32.0f, 70.0f, 32.0f));

	AddBuilding(Vector3(-220.0f, 2.5f, 165.0f), Vector3(18.0f, 18.0f, 18.0f), Vector3(0, 0.0f, 0), 0, highRise13, conceptTex);
	AddInvisibleWall(Vector3(-220.0f, 87.0f, 212.5f), Vector3(24.0f, 89.0f, 24.0f));
	AddInvisibleWall(Vector3(-218.0f, 87.0f, 119.0f), Vector3(24.0f, 85.0f, 24.0f));

	AddBuilding(Vector3(-220.0f, 2.5f, 300.0f), Vector3(16.0f, 16.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);
	AddInvisibleWall(Vector3(-220.0f, 100.0f, 300.0f), Vector3(25.0f, 98.0f, 25.0f));
	AddBuilding(Vector3(-300.0f, 2.5f, 300.0f), Vector3(16.0f, 10.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);
	AddInvisibleWall(Vector3(-300.0f, 60.0f, 300.0f), Vector3(25.0f, 60.0f, 25.0f));
	
	AddBuilding(Vector3(-300.0f, 2.5f, 156.0f), Vector3(16.0f, 15.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddInvisibleWall(Vector3(-300.0f, 72.0f, 156.0f), Vector3(32.0f, 70.0f, 32.0f));
	
	AddBuilding(Vector3(-300.0f, 2.5f, -240.0f), Vector3(18.0f, 18.0f, 18.0f), Vector3(0, 0.0f, 0), 0, highRise13, conceptTex);
	AddInvisibleWall(Vector3(-300.0f, 87.0f, -288.0f), Vector3(24.0f, 89.0f, 24.0f));
	AddInvisibleWall(Vector3(-300.0f, 87.0f, -192.0f), Vector3(24.0f, 85.0f, 24.0f));

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

void NCL::CSC8503::CWLevelManager::InitJumpPads()
{
	AddJumpPad(Vector3(0, 2.5, 128.0f), Vector3(32.0f, 2.0f, 32.0f), Vector3(-45, 180, 0), 15.0f, Debug::RED);
}

void NCL::CSC8503::CWLevelManager::InitMaze()
{
	Vector4 mazeWallColor = Vector4(0.13f, 0.47f, 0.0f, 1.0f);
	float wallThickness = 5.0f;
	AddCube(Vector3(288.0, 20.0f, 174.0f), Vector3(125.0f, 18.0f, wallThickness), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
	AddCube(Vector3(288.0, 20.0f, 414.0f), Vector3(125.0f, 18.0f, wallThickness), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
	
	AddCube(Vector3(405.0, 20.0f, 280.0f), Vector3(wallThickness, 18.0f, 108.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
	AddCube(Vector3(172.0, 20.0f, 306.0f), Vector3(wallThickness, 18.0f, 108.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
												   
	AddCube(Vector3(235.0, 20.0f, 350.0f), Vector3(wallThickness, 18.0f, 60.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
	AddCube(Vector3(350.0, 20.0f, 235.0f), Vector3(wallThickness, 18.0f, 60.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
												   
	AddCube(Vector3(294.0, 20.0f, 355.0f), Vector3(wallThickness, 18.0f, 60.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
	AddCube(Vector3(294.0, 20.0f, 210.0f), Vector3(wallThickness, 18.0f, 30.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);

	AddCube(Vector3(200.0, 20.0f, 206.0f), Vector3(22.0f, 18.0f, wallThickness), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
	AddCube(Vector3(391.0, 20.0f, 382.0f), Vector3(22.0f, 18.0f, wallThickness), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
																 
	AddCube(Vector3(230.0, 20.0f, 294.0f), Vector3(29.0f, 18.0f, wallThickness), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
	AddCube(Vector3(365.0, 20.0f, 294.0f), Vector3(22.0f, 18.0f, wallThickness), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
																 
	AddCube(Vector3(295.0, 20.0f, 360.0f), Vector3(22.0f, 18.0f, wallThickness), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
	AddCube(Vector3(295.0, 20.0f, 230.0f), Vector3(22.0f, 18.0f, wallThickness), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);

	AddCube(Vector3(338.0, 20.0f, 380.0f), Vector3(wallThickness, 18.0f, 36.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
	AddCube(Vector3(246.0, 20.0f, 215.0f), Vector3(wallThickness, 18.0f, 36.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);

	AddCube(Vector3(315.0, 20.0f, 262.0f), Vector3(36.0f, 18.0f, wallThickness), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
	
	AddCube(Vector3(380.0, 20.0f, 318.0f), Vector3(wallThickness, 18.0f, 22.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);

	AddCube(Vector3(232.0, 20.0f, 247.0f), Vector3(18.0f, 18.0f, wallThickness), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);


	AddInvisibleWallTrigger({ 300.0f, 20.0f, 300.0f }, { 130.0f, 20.0f, 130.0f });
	/*AddCube(Vector3(420.0, 20.0f, 270.0f), Vector3(8.0f, 18.0f, 90.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);
	AddCube(Vector3(180.0, 20.0f, 310.0f), Vector3(8.0f, 18.0f, 90.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);

	AddCube(Vector3(235.0, 20.0f, 340.0f), Vector3(8.0f, 18.0f, 55.0f), Vector3(0, 0, 0), 0, mazeWallColor, "MazeWall01", whiteTex);*/
}

void NCL::CSC8503::CWLevelManager::InitDudeNPC()
{
	AddNPCDude(Vector3(30.0f, 4.5f, 0.0f), Vector3(0, 0, 0), Vector3(5, 5, 5), 6.8f);
}

void NCL::CSC8503::CWLevelManager::InitDestroyableProps()
{
	//StreetLights
	Vector3 streetLightScale = Vector3(4, 4, 4);
	Vector3 streetLightBoxScale = Vector3(4, 32, 4);
	Vector3 streetLightBasePos = Vector3(0, -24.0f, 0);
	AddDestroyableProp(Vector3(-170.0f, 36.0f, 0), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-170.0f, 36.0f, 100.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-170.0f, 36.0f, 200.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-170.0f, 36.0f, 300.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 90, 0), envStreetLight, whiteTex, basicShader);
	
	AddDestroyableProp(Vector3(-170.0f, 36.0f, -100.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-170.0f, 36.0f, -200.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-170.0f, 36.0f, -300.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 90, 0), envStreetLight, whiteTex, basicShader);
	
	AddDestroyableProp(Vector3(-350.0f, 36.0f, -300.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-350.0f, 36.0f, -200.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-350.0f, 36.0f, -100.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-350.0f, 36.0f, 0.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-350.0f, 36.0f, 100.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-350.0f, 36.0f, 200.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-350.0f, 36.0f, 300.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	
	AddDestroyableProp(Vector3(-300.0f, 36.0f, 340.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 0, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-210.0f, 36.0f, 340.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 0, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-210.0f, 36.0f, -340.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 180, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-300.0f, 36.0f, -340.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 180, 0), envStreetLight, whiteTex, basicShader);
	
	AddDestroyableProp(Vector3(-90.0f, 36.0f, -300.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-90.0f, 36.0f, -200.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(-90.0f, 36.0f, -100.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	
	AddDestroyableProp(Vector3(90.0f, 36.0f, -100.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(90.0f, 36.0f, -200.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(90.0f, 36.0f, -300.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, 90, 0), envStreetLight, whiteTex, basicShader);
	
	AddDestroyableProp(Vector3(170.0f, 36.0f, -300.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(170.0f, 36.0f, -200.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	
	AddDestroyableProp(Vector3(170.0f, 36.0f, -50.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(170.0f, 36.0f, 50.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(340.0f, 36.0f, -200.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);
	AddDestroyableProp(Vector3(340.0f, 36.0f, -300.0f), streetLightScale, streetLightBoxScale, streetLightBasePos, Vector3(0, -90, 0), envStreetLight, whiteTex, basicShader);

	Vector3 tree2Scale = Vector3(6, 6, 6);
	Vector3 tree2BoxScale = Vector3(10, 28, 10);
	Vector3 tree2BasePos = Vector3(0, -24.0f, 0);
	Vector4 tree2Color = Vector4(0.13f, 0.47f, 0.0f, 1.0f);
	AddDestroyableProp(Vector3(-224.0f, 34.0f, 70.0f), tree2Scale, tree2BoxScale, tree2BasePos, Vector3(0, 0, 0), envTree3, whiteTex, basicShader, tree2Color);
	AddDestroyableProp(Vector3(-224.0f, 34.0f, 174.0f), tree2Scale, tree2BoxScale, tree2BasePos, Vector3(0, 0, 0), envTree3, whiteTex, basicShader, tree2Color);
	AddDestroyableProp(Vector3(-300.0f, 34.0f, 236.0f), tree2Scale, tree2BoxScale, tree2BasePos, Vector3(0, 0, 0), envTree3, whiteTex, basicShader, tree2Color);
	AddDestroyableProp(Vector3(-64.0f, 34.0f, -72.0f), tree2Scale, tree2BoxScale, tree2BasePos, Vector3(0, 0, 0), envTree3, whiteTex, basicShader, tree2Color);
	AddDestroyableProp(Vector3(64.0f, 34.0f, -72.0f), tree2Scale, tree2BoxScale, tree2BasePos, Vector3(0, 0, 0), envTree3, whiteTex, basicShader, tree2Color);

	Vector3 tree3Scale = Vector3(3, 3, 3);
	Vector3 tree3BoxScale = Vector3(5, 30, 5);
	Vector3 tree3BasePos = Vector3(0, -24.0f, 0);
	Vector4 tree3Color = Vector4(0.3f, 0.4f, 0.2f, 1.0f);
	AddDestroyableProp(Vector3(180.0f, 34.0f, 0.0f), tree3Scale, tree3BoxScale, tree3BasePos, Vector3(0, 0, 0), envTree2, whiteTex, basicShader, tree3Color);
	AddDestroyableProp(Vector3(180.0f, 34.0f, -240.0f), tree3Scale, tree3BoxScale, tree3BasePos, Vector3(0, 0, 0), envTree2, whiteTex, basicShader, tree3Color);
	AddDestroyableProp(Vector3(320.0f, 34.0f, -240.0f), tree3Scale, tree3BoxScale, tree3BasePos, Vector3(0, 0, 0), envTree2, whiteTex, basicShader, tree3Color);
	AddDestroyableProp(Vector3(340.0f, 34.0f, -256.0f), tree3Scale, tree3BoxScale, tree3BasePos, Vector3(0, 0, 0), envTree2, whiteTex, basicShader, tree3Color);
	AddDestroyableProp(Vector3(84.0f, 34.0f, -240.0f), tree3Scale, tree3BoxScale, tree3BasePos, Vector3(0, 0, 0), envTree2, whiteTex, basicShader, tree3Color);

	Vector3 carScale = Vector3(6, 6, 6);
	Vector3 carBoxScale = Vector3(20, 12, 12);
	Vector3 carBasePos = Vector3(0, 0.0f, 0);
	Vector4 carColor = Vector4(0.75f, 0.05f, 0.04f, 1.0f);
	AddDestroyableProp(Vector3(-210.0f, 8.0f, -70.0f), carScale, carBoxScale, carBasePos, Vector3(0, 90, 0), envCar, whiteTex, basicShader, carColor);
	AddDestroyableProp(Vector3(0.0f, 8.0f, -240.0f), carScale, carBoxScale, carBasePos, Vector3(0, 90, 0), envCar, whiteTex, basicShader, carColor);
	AddDestroyableProp(Vector3(206.0f, 8.0f, 480.0f), carScale, carBoxScale, carBasePos, Vector3(0, -90, 0), envCar, whiteTex, basicShader, carColor);
	AddDestroyableProp(Vector3(5.0f, 8.0f, -140.0f), carScale, carBoxScale, carBasePos, Vector3(0, -90, 0), envCar, whiteTex, basicShader, carColor);
	AddDestroyableProp(Vector3(410.0f, 8.0f, 130.0f), carScale, carBoxScale, carBasePos, Vector3(0, -90, 0), envCar, whiteTex, basicShader, carColor);
	AddDestroyableProp(Vector3(410.0f, 8.0f, 90.0f), carScale, carBoxScale, carBasePos, Vector3(0, -90, 0), envCar, whiteTex, basicShader, carColor);
	AddDestroyableProp(Vector3(410.0f, 8.0f, 50.0f), carScale, carBoxScale, carBasePos, Vector3(0, -90, 0), envCar, whiteTex, basicShader, carColor);

	Vector3 benchScale = Vector3(3, 3, 3);
	Vector3 benchBoxScale = Vector3(3.8f, 1.85f, 9.8f);
	Vector3 benchBasePos = Vector3(0, 0.0f, 0);
	Vector4 benchColor = Vector4(1.0f, 0.52f, 0.35f, 1.0f);
	AddDestroyableProp(Vector3(-190.0f, 8.0f, 60.0f), benchScale, benchBoxScale, benchBasePos, Vector3(0, 90, 0), envBench, whiteTex, basicShader, benchColor);
	AddDestroyableProp(Vector3(-190.0f, 8.0f, 90.0f), benchScale, benchBoxScale, benchBasePos, Vector3(0, 90, 0), envBench, whiteTex, basicShader, benchColor);
	AddDestroyableProp(Vector3(-320.0f, 8.0f, 256.0f), benchScale, benchBoxScale, benchBasePos, Vector3(0, 90, 0), envBench, whiteTex, basicShader, benchColor);
	AddDestroyableProp(Vector3(-320.0f, 8.0f, 210.0f), benchScale, benchBoxScale, benchBasePos, Vector3(0, 90, 0), envBench, whiteTex, basicShader, benchColor);
	AddDestroyableProp(Vector3(-320.0f, 8.0f, -104.0f), benchScale, benchBoxScale, benchBasePos, Vector3(0, 90, 0), envBench, whiteTex, basicShader, benchColor);
	AddDestroyableProp(Vector3(-320.0f, 8.0f, -150.0f), benchScale, benchBoxScale, benchBasePos, Vector3(0, 90, 0), envBench, whiteTex, basicShader, benchColor);
	AddDestroyableProp(Vector3(256.0f, 8.0f, -320.0f), benchScale, Vector3(9.8f, 1.85f, 3.8f), benchBasePos, Vector3(0, 0, 0), envBench, whiteTex, basicShader, benchColor);
	AddDestroyableProp(Vector3(256.0f, 8.0f, -180.0f), benchScale, Vector3(9.8f, 1.85f, 3.8f), benchBasePos, Vector3(0, 0, 0), envBench, whiteTex, basicShader, benchColor);
}

void NCL::CSC8503::CWLevelManager::AddDestroyableProp(const Vector3& pos, const Vector3& scale, const Vector3& boxSize, const Vector3& baseYPos, const Vector3& rot, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader, const Vector4& color)
{
	CWPropDestroy* prop = new CWPropDestroy(goatGame, pos + Vector3(xOffset, 0, zOffset), baseYPos, scale, boxSize, mesh, texture, shader);
	prop->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(rot.x, rot.y, rot.z));
	prop->GetRenderObject()->SetColour(color);

	//Debug::DrawBox(pos + Vector3(xOffset, 0, zOffset), boxSize, Debug::GREEN, 1000.0f);
	
	world.AddGameObject(prop);
}

void NCL::CSC8503::CWLevelManager::AddCube(const Vector3& cubePos, const Vector3& cubeSize, const Vector3& cubeRot, const float& cubeMass, const Vector4& cubeColour, const std::string& cubeName, TextureBase* cubeTex)
{
	GameObject* cube = new GameObject(1, cubeName);

	AABBVolume* volume = new AABBVolume(cubeSize);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(cubePos + Vector3(xOffset, 0, zOffset))
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(cubeRot.x, cubeRot.y, cubeRot.z))
		.SetScale(cubeSize * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, cubeTex, basicShader));
	cube->GetRenderObject()->SetColour(cubeColour);
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(cubeMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(cube);

	//Debug::DrawBox(cubePos + Vector3(xOffset, 0, zOffset), cubeSize, Debug::GREEN, 1000.0f);
	//Debug::DrawAxisLines(cube->GetTransform().GetMatrix(), 1.0f, 1000.0f);
}

void NCL::CSC8503::CWLevelManager::AddBuilding(const Vector3& buildingPos, const Vector3& buildingSize, const Vector3& buildingRot, const float& buildingMass, MeshGeometry* buildingMesh, TextureBase* buildingTex)
{
	GameObject* building = new GameObject(1, "Building");

	AABBVolume* volume = new AABBVolume(Vector3());
	building->SetBoundingVolume((CollisionVolume*)volume);

	building->GetTransform()
		.SetPosition(buildingPos + Vector3(xOffset, 0, zOffset))
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(buildingRot.x, buildingRot.y, buildingRot.z))
		.SetScale(buildingSize * 2);

	building->SetRenderObject(new RenderObject(&building->GetTransform(), buildingMesh, buildingTex, basicShader));
	building->GetRenderObject()->SetColour(Debug::WHITE);
	building->SetPhysicsObject(new PhysicsObject(&building->GetTransform(), building->GetBoundingVolume()));

	building->GetPhysicsObject()->SetInverseMass(buildingMass);
	building->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(building);

	//Debug::DrawBox(buildingPos + Vector3(xOffset, 0, zOffset), buildingSize, Debug::RED, 1000.0f);
}

void NCL::CSC8503::CWLevelManager::AddInvisibleWall(const Vector3& wallPos, const Vector3 wallSize)
{
	GameObject* wall = new GameObject(1, "BuildingWall");

	AABBVolume* volume = new AABBVolume(wallSize);
	wall->SetBoundingVolume((CollisionVolume*)volume);

	wall->GetTransform()
		.SetPosition(wallPos + Vector3(xOffset, 0, zOffset))
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0))
		.SetScale(wallSize * 2);

	//wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, whiteTex, basicShader));
	//wall->GetRenderObject()->SetColour(Debug::WHITE);

	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(wall);

	//Debug::DrawBox(wallPos + Vector3(xOffset, 0, zOffset), wallSize, Debug::YELLOW, 1000.0f);
}

void NCL::CSC8503::CWLevelManager::AddInvisibleWallTrigger(const Vector3& wallPos, const Vector3 wallSize)
{
	CWMazeTrigger* mazeTrigger = new CWMazeTrigger(goatGame, wallSize);
	mazeTrigger->GetTransform().SetPosition(wallPos + Vector3(xOffset, 0, zOffset)).SetScale(wallSize * 2);

	Debug::DrawBox(wallPos + Vector3(xOffset, 0, zOffset), wallSize, Debug::YELLOW, 1000.0f);

	world.AddGameObject(mazeTrigger);
}

void NCL::CSC8503::CWLevelManager::AddJumpPad(const Vector3& padPos, const Vector3& padSize, const Vector3& padRotation, const float& padForce, const Vector4& padColor)
{
	CWJumpPad* jumpPad = new CWJumpPad(goatGame, padPos + Vector3(xOffset, 0, zOffset), padSize, padForce, padColor, cubeMesh, whiteTex, basicShader);
	jumpPad->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(padRotation.x, padRotation.y, padRotation.z));
	world.AddGameObject(jumpPad);
}

void NCL::CSC8503::CWLevelManager::AddNPCDude(const Vector3& pos, const Vector3& rot, const Vector3& scale, const float& radius, const Vector4& color)
{
	CWDude* dude = new CWDude(goatGame, world, pos + Vector3(xOffset, 0, zOffset), rot, scale, radius, dudeMesh, whiteTex, basicShader, color);
	world.AddGameObject(dude);
}

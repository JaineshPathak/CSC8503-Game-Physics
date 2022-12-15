#pragma once
#include "GameTechRenderer.h"
#include "PhysicsSystem.h"
#include <random>

namespace NCL
{
	namespace CSC8503
	{
		class CWJumpPad;
		class CWPropDestroy;
		class CWDoorTrigger;
		class CWGoatGame;
		class CWPawn;
		class CWDude;
		class CWEvilGoose;
		class CWGrapplePowerup;
		class CWDoorKey;
		class CWLevelManager
		{
		public:
			CWLevelManager(GameWorld& world, CWGoatGame& game, GameTechRenderer& renderer);
			~CWLevelManager();

			MeshGeometry* GetGoatMesh() { return goatMesh; }
			MeshGeometry* GetDudeMesh() { return dudeMesh; }
			MeshGeometry* GetEnemyMesh() { return enemyMesh; }

			ShaderBase* GetBasicShader() { return basicShader; }
			TextureBase* GetWhiteTex() { return whiteTex; }

			CWDoorTrigger* GetDoorTrigger() { return warehouseDoorTrigger; }

			Vector3 GetRandomRoamPoint() 
			{
				if ((int)roamPoints.size() <= 0)
					return Vector3(0, 0, 0);

				std::mt19937 generator(std::random_device{}());
				std::uniform_int_distribution<std::size_t> distribution(0, roamPoints.size() - 1);

				int rndIndex = (int)distribution(generator);
				return roamPoints[rndIndex];
			}

			Vector3 GetRandomMazePoint()
			{
				if ((int)mazePoints.size() <= 0)
					return Vector3(0, 0, 0);

				std::mt19937 generator(std::random_device{}());
				std::uniform_int_distribution<std::size_t> distribution(0, mazePoints.size() - 1);

				int rndIndex = (int)distribution(generator);
				return mazePoints[rndIndex];
			}
			//void Update(float dt);

		protected:

			void InitAssets();
			void InitGoatWorld();
			void InitRoamPoints();
			void InitMazePoints();

			void InitBaseObjects();
			void InitSideWalks();
			void InitBuildings();
			void InitJumpPads();
			void InitMaze();
			void InitWarehouse();
			void InitDudeNPC();
			void InitGooseNPC();
			void InitPowerups();

			void InitDestroyableProps();
			void AddDestroyableProp(const Vector3& pos, const Vector3& size, const Vector3& boxSize, const Vector3& baseYPos, const Vector3& rot, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader, const Vector4& color = Debug::WHITE);

			GameObject* AddCube(const Vector3& cubePos, const Vector3& cubeSize, const Vector3& cubeRot, const float& cubeMass, const Vector4& cubeColour = Vector4(1.0f, 1.0f, 1.0f, 1.0f), const std::string& cubeName = "DefaultCube", TextureBase* cubeTex = nullptr);
			void AddBuilding(const Vector3& buildingPos, const Vector3& buildingSize, const Vector3& buildingRot, const float& buildingMass, MeshGeometry* buildingMesh, TextureBase* buildingTex = nullptr);
			void AddInvisibleWall(const Vector3& wallPos, const Vector3 wallSize);
			CWDoorTrigger* AddInvisibleWallTrigger(const Vector3& wallPos, const Vector3 wallSize);
			void AddJumpPad(const Vector3& padPos, const Vector3& padSize, const Vector3& padRotation, const float& padForce, const Vector4& padColor);
			void AddGrapplePowerup(const Vector3& pos);
			void AddKeyPowerup(const Vector3& pos);

			void AddNPCDude(const Vector3& pos,
				const Vector3& rot,
				const Vector3& scale,
				const float& radius,
				const Vector4& color = Debug::BLUE);

			void AddNPCEvil(const Vector3& pos,
				const Vector3& rot,
				const Vector3& scale,
				const float& radius,
				const Vector4& color = Debug::RED);

			std::vector<Vector3> roamPoints;
			std::vector<Vector3> mazePoints;

			//Meshes
			MeshGeometry* cubeMesh = nullptr;

			//Buildings
			MeshGeometry* highRise3 = nullptr;
			MeshGeometry* highRise9 = nullptr;
			MeshGeometry* highRise12 = nullptr;
			MeshGeometry* highRise13 = nullptr;
			MeshGeometry* highRise18 = nullptr;

			//Props
			MeshGeometry* envStreetLight = nullptr;
			MeshGeometry* envTree2 = nullptr;
			MeshGeometry* envTree3 = nullptr;
			MeshGeometry* envCar = nullptr;
			MeshGeometry* envBench = nullptr;

			TextureBase* basicTex = nullptr;
			TextureBase* propTex = nullptr;
			TextureBase* whiteTex = nullptr;
			TextureBase* conceptTex = nullptr;
			ShaderBase* basicShader = nullptr;

			MeshGeometry* goatMesh = nullptr;
			MeshGeometry* dudeMesh = nullptr;
			MeshGeometry* enemyMesh = nullptr;

			GameObject* warehouseDoor = nullptr;
			CWDoorTrigger* warehouseDoorTrigger = nullptr;

			GameWorld& world;
			CWGoatGame& goatGame;
			GameTechRenderer& renderer;

			float xOffset = 510.0f;
			float zOffset = 510.0f;
		};
	}
}
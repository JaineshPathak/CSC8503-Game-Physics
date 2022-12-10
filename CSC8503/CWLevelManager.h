#pragma once
#include "GameTechRenderer.h"
#include "PhysicsSystem.h"

namespace NCL
{
	namespace CSC8503
	{
		class CWJumpPad;
		class CWPropDestroy;
		class CWGoatGame;
		class CWLevelManager
		{
		public:
			CWLevelManager(GameWorld& world, CWGoatGame& game, GameTechRenderer& renderer);
			~CWLevelManager();

			//void Update(float dt);

		protected:

			void InitAssets();
			void InitGoatWorld();

			void InitBaseObjects();
			void InitSideWalks();
			void InitBuildings();
			void InitJumpPads();
			void InitMaze();

			void InitDestroyableProps();
			void AddDestroyableProp(const Vector3& pos, const Vector3& size, const Vector3& boxSize, const Vector3& rot, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader);

			void AddCube(const Vector3& cubePos, const Vector3& cubeSize, const Vector3& cubeRot, const float& cubeMass, const Vector4& cubeColour = Vector4(1.0f, 1.0f, 1.0f, 1.0f), const std::string& cubeName = "DefaultCube", TextureBase* cubeTex = nullptr);
			void AddBuilding(const Vector3& buildingPos, const Vector3& buildingSize, const Vector3& buildingRot, const float& buildingMass, MeshGeometry* buildingMesh, TextureBase* buildingTex = nullptr);
			void AddInvisibleWall(const Vector3& wallPos, const Vector3 wallSize);
			void AddJumpPad(const Vector3& padPos, const Vector3& padSize, const Vector3& padRotation, const float& padForce, const Vector4& padColor);

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

			GameWorld& world;
			CWGoatGame& goatGame;
			GameTechRenderer& renderer;

			float xOffset = 510.0f;
			float zOffset = 510.0f;
		};
	}
}
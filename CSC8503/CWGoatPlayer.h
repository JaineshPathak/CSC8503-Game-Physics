#pragma once
#include "GameObject.h"
#include "MeshGeometry.h"
#include "GameTechRenderer.h"

namespace NCL
{
    namespace CSC8503
    {
        class CWGoatPlayer : public GameObject
        {
        public:
            CWGoatPlayer(GameWorld& gWorld, GameTechRenderer& gRenderer);
            ~CWGoatPlayer();

            virtual void Update(float dt);

        protected:
            TextureBase* whiteTex = nullptr;
            ShaderBase* basicShader = nullptr;

            MeshGeometry* goatMesh = nullptr;

            GameWorld& world;
            GameTechRenderer& renderer;

        private:
            Vector3 startPos = Vector3(0, 10, 0);
            Vector3 startRot = Vector3(0, 0, 0);
            Vector3 startScale = Vector3(2, 3, 2);

            float moveSpeed = 15.0f;
            float mass = 7.5f;
        };
    }
}
#pragma once
#include "GameObject.h"
#include "MeshGeometry.h"
#include "GameTechRenderer.h"

namespace NCL
{
    namespace CSC8503
    {
        class CWGoatGame;

        class CWGoatPlayer : public GameObject
        {
        public:
            CWGoatPlayer(CWGoatGame& gGame, GameWorld& gWorld, GameTechRenderer& gRenderer);
            ~CWGoatPlayer();

            virtual void Update(float dt);

        protected:
            TextureBase* whiteTex = nullptr;
            ShaderBase* basicShader = nullptr;

            MeshGeometry* goatMesh = nullptr;

            CWGoatGame& game;
            GameWorld& world;
            GameTechRenderer& renderer;

        private:
            Vector3 startPos = Vector3(512.0f, 10.0f, 512.0f);
            Vector3 startRot = Vector3(0, 0, 0);
            Vector3 startScale = Vector3(3.0f, 3.0f, 3.0f);

            Vector3 collisionScale = Vector3(4.0f, 5.0f, 4.0f);

            float moveSpeed = 3550.0f;
            float rotationSpeed = 5.0f;
            float mass = 7.5f;
            float jumpForce = 400.0f;
            float airControl = 1.0f;

            bool isOnGround = true;
        };
    }
}
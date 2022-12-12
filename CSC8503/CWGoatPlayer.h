#pragma once
#include "GameObject.h"
#include "GameTechRenderer.h"

namespace NCL
{
    namespace CSC8503
    {
        class CWGoatGame;
        class CWSpringConstraint;
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
            
            Vector3 velocityRef;
            float velocityDampSmoothness = 0.25f;

            float moveSpeed = 15.0f;
            float rotationSpeed = 5.0f;
            float mass = 7.5f;
            float jumpForce = 7.0f;
            float airControl = 0.5f;

            bool isOnGround = true;
            bool isHooked = false;

            bool enableRope = true;
            CWSpringConstraint* springRope = nullptr;
            Vector3 ropeAnchorPoint;
            float ropeForce = 0.1f;
            float ropeDamping = 0.1f;
        };
    }
}
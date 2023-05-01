#pragma once
#include "GameObject.h"
#include "GameTechRenderer.h"

namespace NCL
{
    namespace CSC8503
    {
        enum PowerUpState
        {
            Active = 0,
            Cooldown = 1,
            Disabled = 2
        };

        class CWGoatPlayer;
        class CWGoatGame;
        class GameWorld;
        class CWGrapplePowerup : public GameObject
        {
        public:
            CWGrapplePowerup() {}
            CWGrapplePowerup(CWGoatGame& gGame, GameWorld& gWorld, const Vector3& pos,
                            MeshGeometry* mesh,
                            TextureBase* tex,
                            ShaderBase* shader,
                            const Vector4& color = Debug::BLACK);

            virtual void Update(float dt);
            virtual void OnTriggerBegin(GameObject* otherObject) override;
            virtual void ResetPowerup();

        protected:
            float elaspedTime = 0.0f;
            float yRot = 0.0f;
            float rotationSpeed = 50.0f;

            float powerupTime = 30.0f;

            float respawnTime = 35.0f;
            float respawnTimeCurrent = 45.0f;
            PowerUpState currentState = PowerUpState::Active;

            Vector3 powerupScale = Vector3(3.0f, 3.0f, 3.0f);
            Vector3 defaultPos;
        };
    }
}
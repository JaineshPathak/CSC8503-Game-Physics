#pragma once
#include "CWGrapplePowerup.h"

namespace NCL
{
    namespace CSC8503
    {
        class CWGoatPlayer;
        class CWGoatGame;
        class GameWorld;
        class CWDoorKey : public CWGrapplePowerup
        {
        public:
            CWDoorKey(CWGoatGame& gGame, GameWorld& gWorld, const Vector3& pos,
                MeshGeometry* mesh,
                TextureBase* tex,
                ShaderBase* shader,
                const Vector4& color = Debug::CYAN);

            void Update(float dt);
            virtual void OnTriggerBegin(GameObject* otherObject) override;
        };
    }
}
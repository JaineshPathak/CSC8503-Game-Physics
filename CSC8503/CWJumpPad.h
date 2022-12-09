#pragma once

#include "GameObject.h"
#include "GameTechRenderer.h"

namespace NCL
{
    namespace CSC8503
    {
        class CWGoatGame;
        class CWJumpPad : public GameObject
        {
        public:
            CWJumpPad(CWGoatGame& gGame, Vector3 position, Vector3 size, float jumpForce, Vector4 color, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader);

            void OnCollisionBegin(GameObject* otherObject) override;

        protected:
            CWGoatGame& goatGame;

            Vector3 jumpDir;
            float jumpForce;
        };
    }
}
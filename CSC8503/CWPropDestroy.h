#pragma once

#include "GameObject.h"
#include "GameTechRenderer.h"

namespace NCL
{
    namespace CSC8503
    {
        class CWGoatGame;
        class CWPropDestroy : public GameObject
        {
        public:
            CWPropDestroy(CWGoatGame& gGame, const Vector3& pos, const Vector3& size, const Vector3& boxSize, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader);
            virtual void OnCollisionBegin(GameObject* otherObject) override;

        protected:
            CWGoatGame& goatGame;

            bool isDestroyed = false;
            int scoreAdd = 2;
        };
    }
}
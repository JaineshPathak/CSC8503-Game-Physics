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
            CWPropDestroy(CWGoatGame& gGame, const Vector3& pos, const Vector3& baseYPos, const Vector3& scale, const Vector3& boxSize, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader);
            virtual void OnCollisionBegin(GameObject* otherObject) override;

            void DestroyProp(GameObject* playerObject, int scoreBonus = 0);
            void ResetProp();

        protected:
            CWGoatGame& goatGame;

            Vector3 defaultPos;
            Vector3 defaultRot;
            Vector4 defaultColor;
            float defaultInvMass;

            Vector3 basePos;
            bool isDestroyed = false;
            bool firstTime = true;
            int scoreAdd = 2;
            float upForceModifier = 5.0f;
        };
    }
}
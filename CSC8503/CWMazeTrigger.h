#pragma once
#include "GameObject.h"
#include "GameTechRenderer.h"

namespace NCL
{
    namespace CSC8503
    {
        class CWGoatGame;
        class CWMazeTrigger : public GameObject
        {
        public:
            CWMazeTrigger(CWGoatGame& gGame, const Vector3& wallSize);
            ~CWMazeTrigger() {}

            virtual void OnTriggerBegin(GameObject* otherObject) override;
            virtual void OnTriggerEnd(GameObject* otherObject) override;

        protected:
            CWGoatGame& goatGame;
            bool playerEntered = false;
        };
    }
}
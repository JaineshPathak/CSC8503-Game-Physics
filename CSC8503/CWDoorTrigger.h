#pragma once
#include "GameObject.h"
#include "GameTechRenderer.h"

namespace NCL
{
    namespace CSC8503
    {
        class CWGoatGame;
        class CWDoorTrigger : public GameObject
        {
        public:
            CWDoorTrigger(CWGoatGame& gGame, const Vector3& wallSize);
            ~CWDoorTrigger() {}

            void SetDoorObject(GameObject* door) { warehouseDoor = door; }
            virtual void OnTriggerBegin(GameObject* otherObject) override;

        protected:
            CWGoatGame& goatGame;
            bool isTriggered = false;

            GameObject* warehouseDoor;
        };
    }
}
#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject() 
{
	counter = 0.0f;
	stateMachine = new StateMachine();

	State* stateLeft = new State([&](float dt)-> void
		{
			this->MoveLeft(dt);
		}
	);

	State* stateRight = new State([&](float dt)-> void
		{
			this->MoveRight(dt);
		}
	);

	stateMachine->AddState(stateLeft);
	stateMachine->AddState(stateRight);

	stateMachine->AddTransition(new StateTransition(stateLeft, stateRight, [&]()->bool
		{
			return this->counter > 3.0f;
		}
	));

	stateMachine->AddTransition(new StateTransition(stateRight, stateLeft, [&]()->bool
		{
			return this->counter < 0.0f;
		}
	));
}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) 
{
	stateMachine->Update(dt);
}

void StateGameObject::MoveLeft(float dt) 
{
	physicsObject->ApplyLinearImpulse({ -2, 0, 0 });
	counter += dt;
}

void StateGameObject::MoveRight(float dt) 
{
	physicsObject->ApplyLinearImpulse({ 2, 0, 0 });
	counter -= dt;
}
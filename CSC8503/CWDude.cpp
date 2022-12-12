#include "CWDude.h"
#include "RenderObject.h"
#include "PhysicsObject.h"

NCL::CSC8503::CWDude::CWDude(CWGoatGame& gGame, GameWorld& gWorld, 
							const Vector3& pos,
							const Vector3& rot, 
							const Vector3& scale, 
							const float& radius, 
							MeshGeometry* mesh,
							TextureBase* tex,
							ShaderBase* shader,
							const Vector4& color) : CWPawn(gGame, gWorld)
{
	SphereVolume* volume = new SphereVolume(radius);
	SetBoundingVolume((CollisionVolume*)volume);

	transform.SetPosition(pos).
		SetOrientation(Quaternion::EulerAnglesToQuaternion(rot.x, rot.y, rot.z)).
		SetScale(scale * 2);

	renderObject = new RenderObject(&transform, mesh, tex, shader);
	renderObject->SetColour(color);

	physicsObject = new PhysicsObject(&transform, boundingVolume);
	physicsObject->SetInverseMass(5.5f);
	physicsObject->InitCubeInertia();
	physicsObject->SetLinearDamping(0.3f);
	physicsObject->SetGravityMultiplier(5.0f);
	physicsObject->SetRestitution(0);

	gGame.AddPawnToList(this);

	patrolPoints.push_back(Vector3(-120.0f + 512.0f, 0.0f, 0.0f + 512.0f));
	patrolPoints.push_back(Vector3(-120.0f + 512.0f, 0.0f, -400.0f + 512.0f));
	patrolPoints.push_back(Vector3(120.0f + 512.0f, 0.0f, -400.0f + 512.0f));
	patrolPoints.push_back(Vector3(120.0f + 512.0f, 0.0f, 0.0f + 512.0f));
	for (size_t i = 0; i < patrolPoints.size(); i++)
		Debug::DrawBox(patrolPoints[i], Vector3(1, 1, 1), Debug::RED, 1000.0f);

	stateMachine = new StateMachine();
	State* Idle = new State([&](float dt)->void
		{
		}
	);

	State* Roaming = new State([&](float dt)->void
		{
			NavigationPath path;
			bool found = goatGame.GetNavGrid()->FindPath(transform.GetPosition(), patrolPoints[currentPatrolPointIndex], path);
			if (found)
			{
				pathList.clear();
				Vector3 pos;
				while (path.PopWaypoint(pos))
					pathList.push_back(pos);

				for (int i = 1; i < pathList.size(); ++i)
				{
					Vector3 a = pathList[i - 1];
					Vector3 b = pathList[i];

					Debug::DrawLine(a, b, Debug::GREEN);
				}
				Vector3 wp = pathList[wpIndex];
				if ( (transform.GetPosition() - wp).Length() <= 4.0f)
				{
					wpIndex++;
					if (wpIndex >= pathList.size())
					{
						return;
					}
				}
				transform.SetPosition(Vector3::MoveTowards(transform.GetPosition(), pathList[wpIndex], moveSpeed * dt));
			}
		}
	);

	State* Running = new State([&](float dt)->void
		{
		}
	);

	StateTransition* stateIdleToRoaming = new StateTransition(Idle, Roaming, [&](void)->bool
		{
			return true;
		}
	);

	stateMachine->AddState(Idle);
	stateMachine->AddState(Roaming);
	stateMachine->AddTransition(stateIdleToRoaming);
	//stateMachine->AddTransition(stateBA);

	/*StateTransition* Roaming = new StateTransition(B, A, [&](void)->bool
		{
			return data < 0;
		}
	);*/
}

void NCL::CSC8503::CWDude::Update(float dt)
{
	stateMachine->Update(dt);
}

void NCL::CSC8503::CWDude::OnCollisionBegin(GameObject* otherObject)
{
}

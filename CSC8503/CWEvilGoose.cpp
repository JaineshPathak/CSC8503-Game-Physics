#include "CWEvilGoose.h"
#include "RenderObject.h"
#include "PhysicsObject.h"

NCL::CSC8503::CWEvilGoose::CWEvilGoose(CWGoatGame& gGame, GameWorld& gWorld,
	const Vector3& pos,
	const Vector3& rot,
	const Vector3& scale,
	const float& radius,
	MeshGeometry* mesh,
	TextureBase* tex,
	ShaderBase* shader,
	const Vector4& color) : CWPawn(gGame, gWorld)
{
	//currentRoamDestination = goatGame.GetRandomRoamPoint();

	SphereVolume* volume = new SphereVolume(radius);
	SetBoundingVolume((CollisionVolume*)volume);

	tag = "NPC_Enemy";
	transform.SetPosition(pos).
		SetOrientation(Quaternion::EulerAnglesToQuaternion(rot.x, rot.y, rot.z)).
		SetScale(scale * 2);

	renderObject = new RenderObject(&transform, mesh, tex, shader);
	renderObject->SetColour(color);

	physicsObject = new PhysicsObject(&transform, boundingVolume);
	physicsObject->SetInverseMass(5.5f);
	physicsObject->InitCubeInertia();
	physicsObject->SetLinearDamping(4.0f);
	physicsObject->SetGravityMultiplier(5.0f);
	physicsObject->SetRestitution(0);

	mazePos = Vector3(300.0f + 512.0f, 0.0f, 300.0f + 512.0f);
	//Debug::DrawBox(mazePos, Vector3(radius, 10.0f, radius), Debug::WHITE, 1000.0f);

	gGame.AddPawnToList(this);

	stateMachine = new StateMachine();
	Idle = new State([&](float dt)->void
		{
		}
	);

	Roaming = new State([&](float dt)->void
		{
			//renderObject->SetColour(Debug::RED);

			/*pathTimerCurrent = 0.0f;
			float patrolDistance = (currentDestination - (transform.GetPosition() + basePos)).Length();
			if (patrolDistance <= distanceThreshold)
			{
				
			}*/
			
			if (pathList.size() > 0)
			{
				//DebugDisplayPath(pathList);
				float nodeDistance = (pathList[currentDestinationIndex] - (transform.GetPosition() + basePos)).Length();
				if (nodeDistance <= distanceThreshold)
				{
					currentDestinationIndex++;
					if (currentDestinationIndex >= (int)pathList.size() - 1)
						FindRandomPatrolPoint();
				}

				MoveTowards(pathList[currentDestinationIndex], dt, true);
				RotateTowards(pathList[currentDestinationIndex], rotationSpeed, dt);
			}
		}
	);

	Chasing = new State([&](float dt)->void
		{
			if (goatGame.GetPlayer() == nullptr)
				return;

			currentDestination = goatGame.GetPlayer()->GetTransform().GetPosition();
			if (LookingAtPlayer())
			{
				MoveTowards(currentDestination, dt, true);
				RotateTowards(currentDestination, rotationSpeed, dt);

				pathList.clear();
			}
			else
			{
				pathTimerCurrent += dt;
				if (pathTimerCurrent >= pathTimer)
				{
					FindPath(currentDestination, pathList);
					currentDestinationIndex = 0;
					pathTimerCurrent = 0.0f;
				}
			}

			//DebugDisplayPath(pathList);

			if (pathList.size() > 0)
			{
				float nodeDistance = (pathList[currentDestinationIndex] - (transform.GetPosition() + basePos)).Length();
				if (nodeDistance <= distanceThreshold)
					currentDestinationIndex = (currentDestinationIndex + 1) % pathList.size();
				
				MoveTowards(pathList[currentDestinationIndex], dt, true);
				RotateTowards(pathList[currentDestinationIndex], rotationSpeed, dt);
			}
			/*if (pathList.size() > 2)
			{
				Vector3 wayPoint = pathList[1];
				float nodeDistance = (wayPoint - (transform.GetPosition() + basePos)).Length();
				if (nodeDistance <= distanceThreshold)
				{
					path.PopWaypoint(wayPoint);
					Debug::DrawBox(wayPoint, Vector3(1, 1, 1), Debug::MAGENTA);
				}
				//MoveTowards(wayPoint, dt, false);
				MoveTowards(pathList[0], wayPoint, dt);
				RotateTowards(pathList[2], rotationSpeed, dt);
			}*/
		}
	);

	StateTransition* stateIdleToRoaming = new StateTransition(Idle, Roaming, [&](void)->bool
		{
			FindRandomPatrolPoint();
			return true;
		}
	);

	StateTransition* stateRoamingToChasing = new StateTransition(Roaming, Chasing, [&](void)->bool
		{
			float distance = (mazePos - goatGame.GetPlayer()->GetTransform().GetPosition()).LengthSquared();
			return distance < (mazeRadius * mazeRadius);
		}
	);

	StateTransition* stateChasingToRoaming = new StateTransition(Chasing, Roaming, [&](void)->bool
		{
			float distance = (mazePos - goatGame.GetPlayer()->GetTransform().GetPosition()).LengthSquared();
			if (distance > (mazeRadius * mazeRadius))
			{
				FindRandomPatrolPoint();
				return true;
			}
			else
				return false;
		}
	);

	stateMachine->AddState(Idle);
	stateMachine->AddState(Roaming);
	stateMachine->AddState(Chasing);
	stateMachine->AddTransition(stateIdleToRoaming);
	stateMachine->AddTransition(stateRoamingToChasing);
	stateMachine->AddTransition(stateChasingToRoaming);
	//stateMachine->AddTransition(stateBA);

	/*StateTransition* Roaming = new StateTransition(B, A, [&](void)->bool
		{
			return data < 0;
		}
	);*/
}

void NCL::CSC8503::CWEvilGoose::Update(float dt)
{
	//std::cout << transform.GetPosition().y << std::endl;
	stateMachine->Update(dt);
}

void NCL::CSC8503::CWEvilGoose::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetTag() == "Prop")
	{
		if (stateMachine->GetActiveState() == Roaming)
			currentDestination = goatGame.GetRandomMazePoint();
	}

	if (otherObject->GetTag() == "NPC_Padestrian")
	{
		if (stateMachine->GetActiveState() == Roaming)
			currentDestination = goatGame.GetRandomMazePoint();
	}

	if (otherObject->GetTag() == "Player")
	{
		if (stateMachine->GetActiveState() == Chasing)
		{
			if (goatGame.GetGameState() == GameState::GameMenu || goatGame.GetGameState() == GameState::GameEnded) return;

			//goatGame.GetPlayer()->ResetPlayer();
			goatGame.GetPlayer()->TakeDamage(meleeDamage);
			if(goatGame.GetPlayer()->GetHealth() <= 0.0f)
				FindRandomPatrolPoint();		//Go Back to Patrol State
		}
	}
}

void NCL::CSC8503::CWEvilGoose::DebugDisplayPath(std::vector<Vector3> paths)
{
	for (int i = 1; i < paths.size(); ++i)
	{
		Vector3 a = paths[i - 1];
		Vector3 b = paths[i];

		Debug::DrawLine(a, b, Debug::GREEN);
	}
}

bool NCL::CSC8503::CWEvilGoose::LookingAtPlayer()
{
	bool status = false;

	Vector3 playerDir = currentDestination - transform.GetPosition();
	Ray ray = Ray(transform.GetPosition(), playerDir.Normalised());
	RayCollision rayCol;

	if (world.Raycast(ray, rayCol, true, this))
		status = rayCol.node == goatGame.GetPlayer();

	return status;
}

void NCL::CSC8503::CWEvilGoose::FindRandomPatrolPoint()
{
	currentDestination = goatGame.GetRandomMazePoint();
	FindPath(currentDestination, pathList);
	currentDestinationIndex = 0;
}
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
	//currentRoamDestination = goatGame.GetRandomRoamPoint();

	SphereVolume* volume = new SphereVolume(radius);
	SetBoundingVolume((CollisionVolume*)volume);


	name = "Lebowski";
	tag = "NPC_Padestrian";
	transform.SetPosition(pos).
		SetOrientation(Quaternion::EulerAnglesToQuaternion(rot.x, rot.y, rot.z)).
		SetScale(scale * 2);

	renderObject = new RenderObject(&transform, mesh, tex, shader);
	renderObject->SetColour(color);

	physicsObject = new PhysicsObject(&transform, boundingVolume);
	physicsObject->SetInverseMass(5.5f);
	physicsObject->InitCubeInertia();
	physicsObject->SetLinearDamping(4.0f);	//0.3f
	physicsObject->SetGravityMultiplier(5.0f);
	physicsObject->SetRestitution(0);

	gGame.AddPawnToList(this);

	stateMachine = new StateMachine();
	Idle = new State([&](float dt)->void
		{
		}
	);

	Roaming = new State([&](float dt)->void
		{
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
			#pragma region OLD CODE
			/*roamTimerCurrent += dt;
			if (roamTimerCurrent >= roamTimer)
			{
				roamTimerCurrent = 0.0f;
				currentDestination = goatGame.GetRandomRoamPoint();
			}

			float patrolDistance = ((transform.GetPosition() + basePos) - currentDestination).Length();
			if (patrolDistance <= distanceThreshold)
				currentRoamDestination = goatGame.GetRandomRoamPoint();



			NavigationPath path;
			bool found = goatGame.GetNavGrid()->FindPath(transform.GetPosition(), currentRoamDestination, path) && (patrolDistance > distanceThreshold);
			if (found)
			{
				pathList.clear();
				Vector3 pos;
				while (path.PopWaypoint(pos))
					pathList.push_back(pos);

				//DebugDisplayPath(pathList);

				if (pathList.size() > 2)
				{
					Vector3 wayPoint = pathList[1];
					float nodeDistance = (wayPoint - (transform.GetPosition() + basePos)).Length();
					if (nodeDistance <= distanceThreshold)
					{
						path.PopWaypoint(wayPoint);
						//Debug::DrawBox(wayPoint, Vector3(1, 1, 1), Debug::MAGENTA);
					}
					//MoveTowards(wayPoint, dt, false);
					MoveTowards(pathList[0], wayPoint, dt);
					RotateTowards(wayPoint, rotationSpeed, dt);
				}
				else
				{
					MoveTowards(currentRoamDestination, dt, false);
					RotateTowards(currentRoamDestination, rotationSpeed, dt);
				}
			}
			else
			{
				MoveTowards(currentRoamDestination, dt, false);
				RotateTowards(currentRoamDestination, rotationSpeed, dt);
			}*/
			#pragma endregion
			#pragma region Helped by Mr Giacomo Bergami
			/*
			NavigationPath path;
			bool found = goatGame.GetNavGrid()->FindPath(transform.GetPosition(), patrolPoints[currentPatrolPointIndex], path);
			if (found)
			{
				pathList.clear();
				Vector3 pos;
				while (path.PopWaypoint(pos))
					pathList.push_back(pos);

				DebugDisplayPath(pathList);

				float patrolDistance = ( (transform.GetPosition() + basePos) - pos).Length();
				if (patrolDistance <= distanceThreshold)
					currentPatrolPointIndex = (currentPatrolPointIndex + 1) % patrolPoints.size();

				if (pathList.size() > 2)
				{
					Vector3 wayPoint = pathList[1];
					float nodeDistance = ((transform.GetPosition() + basePos) - wayPoint).Length();
					if(nodeDistance <= distanceThreshold)
					{
						path.PopWaypoint(wayPoint);
						Debug::DrawBox(wayPoint, Vector3(1, 1, 1), Debug::MAGENTA);
					}
					MoveTowards(pathList[0], wayPoint, dt);
				}
				else
				{
					MoveTowards(transform.GetPosition(), patrolPoints[currentPatrolPointIndex], dt);
				}
			}*/
			#pragma endregion
		}
	);

	Running = new State([&](float dt)->void
		{
			if (goatGame.GetPlayer() == nullptr)
				return;

			Vector3 dir = (goatGame.GetPlayer()->GetTransform().GetPosition() - transform.GetPosition()).Normalised();
			dir.y = 0;
			physicsObject->SetLinearVelocity(-dir * moveSpeed);
			RotateAway(goatGame.GetPlayer()->GetTransform().GetPosition(), rotationSpeed * 1.5f, dt);

			chaseTimerCurrent += dt;
		}
	);

	StateTransition* stateIdleToRoaming = new StateTransition(Idle, Roaming, [&](void)->bool
		{
			FindRandomPatrolPoint();
			return true;
		}
	);

	StateTransition* stateRoamingToRunning = new StateTransition(Roaming, Running, [&](void)->bool
		{
			if (goatGame.GetPlayer() == nullptr)
				return false;

			float distance = (goatGame.GetPlayer()->GetTransform().GetPosition() - transform.GetPosition()).LengthSquared();
			chaseTimerCurrent = 0.0f;
			return distance <= (chaseRadius * chaseRadius);
		}
	);

	StateTransition* stateRunningToRoaming = new StateTransition(Running, Roaming, [&](void)->bool
		{
			if (goatGame.GetPlayer() == nullptr)
				return false;

			float distance = (goatGame.GetPlayer()->GetTransform().GetPosition() - transform.GetPosition()).LengthSquared();
			//return distance >= chaseRadius;

			if (chaseTimerCurrent >= chaseTimer && distance > (chaseRadius * chaseRadius))
			{
				chaseTimerCurrent = 0.0f;
				FindRandomPatrolPoint();
				return true;
			}
			
			return false;
		}
	);

	stateMachine->AddState(Idle);
	stateMachine->AddState(Roaming);
	stateMachine->AddState(Running);
	stateMachine->AddTransition(stateIdleToRoaming);
	stateMachine->AddTransition(stateRoamingToRunning);
	stateMachine->AddTransition(stateRunningToRoaming);
	//stateMachine->AddTransition(stateBA);

	/*StateTransition* Roaming = new StateTransition(B, A, [&](void)->bool
		{
			return data < 0;
		}
	);*/
}

void NCL::CSC8503::CWDude::Update(float dt)
{
	//std::cout << transform.GetPosition().y << std::endl;
	stateMachine->Update(dt);
}

void NCL::CSC8503::CWDude::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetTag() == "Prop")
	{
		if (stateMachine->GetActiveState() == Roaming)
		{
			roamTimerCurrent = 0.0f;
			FindRandomPatrolPoint();
		}
		else if (stateMachine->GetActiveState() == Running)
		{
			CWPropDestroy* prop = static_cast<CWPropDestroy*>(otherObject);
			if (prop != nullptr)
				prop->DestroyProp(goatGame.GetPlayer(), (int)scoreBonus);
		}
	}

	if (otherObject->GetTag() == "NPC_Padestrian")
	{
		if (stateMachine->GetActiveState() == Roaming)
		{
			roamTimerCurrent = 0.0f;
			FindRandomPatrolPoint();
		}
	}
}

void NCL::CSC8503::CWDude::DebugDisplayPath(std::vector<Vector3> paths)
{
	for (int i = 1; i < paths.size(); ++i)
	{
		Vector3 a = paths[i - 1];
		Vector3 b = paths[i];

		Debug::DrawLine(a, b, Debug::GREEN);
	}
}

void NCL::CSC8503::CWDude::MoveTowards(const Vector3& pos, float dt, bool useForce)
{
	if (useForce)
	{
		auto v = (pos - transform.GetPosition()).Normalised();
		v.y = 0.0f;
		physicsObject->AddForce(v * moveSpeed);
	}
	else
		transform.SetPosition(Vector3::MoveTowards(transform.GetPosition(), pos, moveSpeed * dt));
}

void NCL::CSC8503::CWDude::MoveTowards(Vector3 src, const Vector3& pos, float dt)
{
	auto color = Debug::BLUE;
	if ((previousPosition - transform.GetPosition()).Length() < (distanceThreshold * 0.01)) {
		src = transform.GetPosition();
		color = Debug::MAGENTA;
	}

	//Debug::DrawLine(pos, src, color);
	auto v = (pos - src).Normalised();
	v.y = 0.0f;
	physicsObject->AddForce(v * moveSpeed);
	//v = (v).Normalised() * moveSpeed;
	//physicsObject->SetLinearVelocity(v);

	previousPosition = transform.GetPosition();

	//transform.SetPosition(Vector3::MoveTowards(transform.GetPosition(), pos, moveSpeed * dt));
}

void NCL::CSC8503::CWDude::RotateTowards(const Vector3& pos, float rotSpeed, float dt)
{
	Quaternion ogRot = Quaternion::RotateTowards(transform.GetPosition(), pos, Vector3(0, 1, 0));
	Vector3 ogRotEuler = ogRot.ToEuler();
	ogRotEuler.x = 0;
	ogRotEuler.z = 0;
	Quaternion finalRot = Quaternion::EulerAnglesToQuaternion(ogRotEuler.x, ogRotEuler.y, ogRotEuler.z);
	transform.SetOrientation(Quaternion::Slerp(transform.GetOrientation(), finalRot, rotSpeed * dt));
}

void NCL::CSC8503::CWDude::RotateAway(const Vector3& pos, float rotSpeed, float dt)
{
	Quaternion ogRot = Quaternion::RotateTowards(transform.GetPosition(), pos, Vector3(0, 1, 0));
	Vector3 ogRotEuler = ogRot.ToEuler();
	ogRotEuler.x = 0;
	ogRotEuler.y -= 180.0f;
	ogRotEuler.z = 0;
	Quaternion finalRot = Quaternion::EulerAnglesToQuaternion(ogRotEuler.x, ogRotEuler.y, ogRotEuler.z);
	transform.SetOrientation(Quaternion::Slerp(transform.GetOrientation(), finalRot, rotSpeed * dt));
}

void NCL::CSC8503::CWDude::FindRandomPatrolPoint()
{
	currentDestination = goatGame.GetRandomRoamPoint();
	FindPath(currentDestination, pathList);
	currentDestinationIndex = 0;
}

void NCL::CSC8503::CWDude::FindPath(const Vector3& destination, std::vector<Vector3>& _pathList)
{
	if (goatGame.GetNavGrid()->FindPath(transform.GetPosition(), destination, path))
	{
		if (_pathList.size() > 0) _pathList.clear();

		Vector3 pos;
		while (path.PopWaypoint(pos))
			_pathList.push_back(pos);
	}
}

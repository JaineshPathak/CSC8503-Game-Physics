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


	name = "Lebowski";
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

	//transform.SetPosition(Vector3(patrolPoints[1].x, 3.0f, patrolPoints[1].z));
	//currentPatrolPointIndex = 1;

	for (size_t i = 0; i < patrolPoints.size(); i++)
		Debug::DrawBox(patrolPoints[i], Vector3(1, 1, 1), Debug::GREEN, 1000.0f);

	stateMachine = new StateMachine();
	State* Idle = new State([&](float dt)->void
		{
		}
	);

	State* Roaming = new State([&](float dt)->void
		{
			float patrolDistance = ((transform.GetPosition() + basePos) - patrolPoints[currentPatrolPointIndex]).Length();
			if (patrolDistance <= distanceThreshold)
				currentPatrolPointIndex = (currentPatrolPointIndex + 1) % patrolPoints.size();

			NavigationPath path;
			bool found = goatGame.GetNavGrid()->FindPath(transform.GetPosition(), patrolPoints[currentPatrolPointIndex], path) && (patrolDistance > distanceThreshold);
			if (found && !foundPathPrev)
			{
				foundPathPrev = true;
				pathList.clear();
				Vector3 pos;
				while (path.PopWaypoint(pos))
					pathList.push_back(pos);

				DebugDisplayPath(pathList);

				if (pathList.size() > 2)
				{
					Vector3 wayPoint = pathList[1];
					float nodeDistance = ((transform.GetPosition() + basePos) - wayPoint).Length();
					if (nodeDistance <= distanceThreshold)
					{
						path.PopWaypoint(wayPoint);
						Debug::DrawBox(wayPoint, Vector3(1, 1, 1), Debug::MAGENTA);
					}
					MoveTowards(wayPoint, dt, false);
					RotateTowards(wayPoint, rotationSpeed, dt);
				}
			}
			else
			{
				MoveTowards(patrolPoints[currentPatrolPointIndex], dt, false);
				RotateTowards(patrolPoints[currentPatrolPointIndex], rotationSpeed, dt);
			}

			#pragma region Helped by Giacomo
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

	State* Running = new State([&](float dt)->void
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
			return true;
		}
	);

	StateTransition* stateRoamingToRunning = new StateTransition(Roaming, Running, [&](void)->bool
		{
			if (goatGame.GetPlayer() == nullptr)
				return false;

			float distance = (goatGame.GetPlayer()->GetTransform().GetPosition() - transform.GetPosition()).Length();
			chaseTimerCurrent = 0.0f;
			return distance <= chaseRadius;
		}
	);

	StateTransition* stateRunningToRoaming = new StateTransition(Running, Roaming, [&](void)->bool
		{
			if (goatGame.GetPlayer() == nullptr)
				return false;

			float distance = (goatGame.GetPlayer()->GetTransform().GetPosition() - transform.GetPosition()).Length();
			//return distance >= chaseRadius;

			if (chaseTimerCurrent >= chaseTimer && distance > chaseRadius)
			{
				chaseTimerCurrent = 0.0f;
				return true;
			}
			
			return false;
		}
	);

	stateMachine->AddState(Idle);
	stateMachine->AddState(Roaming);
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
		physicsObject->SetLinearVelocity(v * moveSpeed);
	}
	else
		transform.SetPosition(Vector3::MoveTowards(transform.GetPosition(), pos, moveSpeed * dt));
}

void NCL::CSC8503::CWDude::MoveTowards(Vector3 src, const Vector3& pos, float dt)
{
	auto color = Debug::BLUE;
	if ((previousPosition - transform.GetPosition()).Length() < (distanceThreshold * 0.01)) {
		src = transform.GetPosition(); color = Debug::MAGENTA;
	}
	Quaternion ogRot = Quaternion::RotateTowards(transform.GetPosition(), pos, Vector3(0, 1, 0));
	Vector3 ogRotEuler = ogRot.ToEuler();
	ogRotEuler.x = 0;
	ogRotEuler.z = 0;
	Quaternion finalRot = Quaternion::EulerAnglesToQuaternion(ogRotEuler.x, ogRotEuler.y, ogRotEuler.z);
	transform.SetOrientation(finalRot);
	auto v = (pos - src);

	Debug::DrawLine(pos, src, color);
	v.y = 0;
	v = (v).Normalised() * moveSpeed;
	physicsObject->SetLinearVelocity(v);
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
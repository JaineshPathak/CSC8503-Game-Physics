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


	tag = "NPC_Evil";
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

			float patrolDistance = ((transform.GetPosition() + basePos) - currentRoamDestination).Length();
			if (patrolDistance <= distanceThreshold)
				currentRoamDestination = goatGame.GetRandomMazePoint();

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
			}
		}
	);

	Chasing = new State([&](float dt)->void
		{
			//renderObject->SetColour(Debug::YELLOW);

			if (goatGame.GetPlayer() == nullptr)
				return;

			currentRoamDestination = goatGame.GetPlayer()->GetTransform().GetPosition();
			NavigationPath path;
			bool found = goatGame.GetNavGrid()->FindPath(transform.GetPosition(), currentRoamDestination, path);
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
						Debug::DrawBox(wayPoint, Vector3(1, 1, 1), Debug::MAGENTA);
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
			}
		}
	);

	StateTransition* stateIdleToRoaming = new StateTransition(Idle, Roaming, [&](void)->bool
		{
			currentRoamDestination = goatGame.GetRandomMazePoint();
			return true;
		}
	);

	StateTransition* stateRoamingToChasing = new StateTransition(Roaming, Chasing, [&](void)->bool
		{
			if (goatGame.GetPlayer() == nullptr)
				return false;

			float distance = (goatGame.GetPlayer()->GetTransform().GetPosition() - mazePos).Length();
			return distance <= mazeRadius;
		}
	);

	StateTransition* stateChasingToRoaming = new StateTransition(Chasing, Roaming, [&](void)->bool
		{
			if (goatGame.GetPlayer() == nullptr)
				return false;

			float distance = (goatGame.GetPlayer()->GetTransform().GetPosition() - transform.GetPosition()).Length();
			return distance >= mazeRadius;

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
			currentRoamDestination = goatGame.GetRandomMazePoint();
	}

	if (otherObject->GetTag() == "NPC_Padestrian")
	{
		if (stateMachine->GetActiveState() == Roaming)
			currentRoamDestination = goatGame.GetRandomMazePoint();
	}

	if (otherObject->GetTag() == "Player")
	{
		if (stateMachine->GetActiveState() == Chasing)
		{
			goatGame.GetPlayer()->ResetPlayer();
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

void NCL::CSC8503::CWEvilGoose::MoveTowards(const Vector3& pos, float dt, bool useForce)
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

void NCL::CSC8503::CWEvilGoose::MoveTowards(Vector3 src, const Vector3& pos, float dt)
{
	auto color = Debug::BLUE;
	if ((previousPosition - transform.GetPosition()).Length() < (distanceThreshold * 0.01)) {
		src = transform.GetPosition();
		color = Debug::MAGENTA;
	}

	//Debug::DrawLine(pos, src, color);
	auto v = (pos - src);
	v.y = 0;
	v = (v).Normalised() * moveSpeed;
	physicsObject->SetLinearVelocity(v);
	previousPosition = transform.GetPosition();

	//transform.SetPosition(Vector3::MoveTowards(transform.GetPosition(), pos, moveSpeed * dt));
}

void NCL::CSC8503::CWEvilGoose::RotateTowards(const Vector3& pos, float rotSpeed, float dt)
{
	Quaternion ogRot = Quaternion::RotateTowards(transform.GetPosition(), pos, Vector3(0, 1, 0));
	Vector3 ogRotEuler = ogRot.ToEuler();
	ogRotEuler.x = 0;
	ogRotEuler.z = 0;
	Quaternion finalRot = Quaternion::EulerAnglesToQuaternion(ogRotEuler.x, ogRotEuler.y, ogRotEuler.z);
	transform.SetOrientation(Quaternion::Slerp(transform.GetOrientation(), finalRot, rotSpeed * dt));
}

void NCL::CSC8503::CWEvilGoose::RotateAway(const Vector3& pos, float rotSpeed, float dt)
{
	Quaternion ogRot = Quaternion::RotateTowards(transform.GetPosition(), pos, Vector3(0, 1, 0));
	Vector3 ogRotEuler = ogRot.ToEuler();
	ogRotEuler.x = 0;
	ogRotEuler.y -= 180.0f;
	ogRotEuler.z = 0;
	Quaternion finalRot = Quaternion::EulerAnglesToQuaternion(ogRotEuler.x, ogRotEuler.y, ogRotEuler.z);
	transform.SetOrientation(Quaternion::Slerp(transform.GetOrientation(), finalRot, rotSpeed * dt));
}
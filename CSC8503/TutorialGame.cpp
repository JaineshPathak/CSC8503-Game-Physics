#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"



using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);
	//physics->SetGravity(Vector3(0, 9.81f, 0));

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	cubeMesh	= renderer->LoadMesh("cube.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	charMesh	= renderer->LoadMesh("goat.msh");
	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	bonusMesh	= renderer->LoadMesh("apple.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt) 
{
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0,1,0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}

	if (cubeLooker != nullptr && selectionObject != nullptr)
	{
		Vector3 cubePos = cubeLooker->GetTransform().GetPosition();
		//Vector3 camPos = world->GetMainCamera()->GetPosition();

		Matrix4 temp = Matrix4::BuildViewMatrix(cubePos, selectionObject->GetTransform().GetPosition(), Vector3(0, 1, 0));
		Matrix4 modelMat = temp.Inverse();		//Gives World Position

		Quaternion q(modelMat);
		cubeLooker->GetTransform().SetOrientation(Quaternion::Slerp(cubeLooker->GetTransform().GetOrientation(), q, dt));
	}

	UpdateKeys();

	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(5, 95), Debug::RED);
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
	}

	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);	//Forward Direction

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}

	//Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));

	if (testStateObject)
		testStateObject->Update(dt);

	SelectObject();
	MoveSelectedObject();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(15000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	//InitMixedGridWorld(15, 15, 3.5f, 3.5f);
	//AddSphereToWorld(Vector3(2, 0, 0), 1.0f, 3.5f);
	testStateObject = AddStateObjectToWorld(Vector3(0, 25, 0));

	AddSphereToWorld(Vector3(0, 0, 15), 1.0f, 7.5f);
	AddCubeToWorld(Vector3(15, 0, 0), Vector3(1, 1, 1), 3.5f);
	AddCubeToWorld(Vector3(-15, 0, 0), Vector3(1, 1, 1), 3.5f);

	InitGameExamples();
	InitDefaultFloor();
	BridgeConstraintTest();

}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject(2, "Floor");		//Ignored layer

	Vector3 floorSize = Vector3(200, 2, 200);
	OBBVolume* volume = new OBBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2.0f)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0))
		.SetPosition(position);

	PhysicsObject* floorPhys = new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume());
	floorPhys->SetRestitution(1.0f);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(floorPhys);

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	//Debug::DrawBox(volume->GetCenter(), volume->GetHalfDimensions(), Debug::WHITE, 1000.0f);
	Debug::DrawBox(floor->GetTransform().GetPosition(), floor->GetTransform().GetOrientation(), volume->GetHalfDimensions(), Debug::GREEN, 1000.0f);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject(1, "Sphere01");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);

	if(rand() % 2)
		sphere->GetPhysicsObject()->InitSphereInertia();
	else
	{
		sphere->GetPhysicsObject()->InitHollowSphereInertia();
		sphere->GetRenderObject()->SetColour(Debug::CYAN);
	}

	world->AddGameObject(sphere);

	return sphere;
}

//http://thunderfist-podium.blogspot.com/2012/02/capsule-capsule-collision-in-games.html
GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject(1, "Cube01");

	OBBVolume* volume = new OBBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0))
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	//Debug::DrawBox(cube->GetTransform().GetMatrix(), cube->GetTransform().GetOrientation(), volume->GetHalfDimensions(), Debug::WHITE, 1000.0f);

	return cube;
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();
	SphereVolume* volume  = new SphereVolume(1.0f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position)
{
	StateGameObject* stateObj = new StateGameObject();

	SphereVolume* volume = new SphereVolume(4.0f);
	stateObj->SetBoundingVolume((CollisionVolume*)volume);
	stateObj->GetTransform()
		.SetScale(Vector3(4, 4, 4))
		.SetPosition(position);

	stateObj->SetRenderObject(new RenderObject(&stateObj->GetTransform(), sphereMesh, basicTex, basicShader));
	stateObj->SetPhysicsObject(new PhysicsObject(&stateObj->GetTransform(), stateObj->GetBoundingVolume()));

	stateObj->GetPhysicsObject()->SetInverseMass(1.0f);
	stateObj->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(stateObj);

	return stateObj;
}

void TutorialGame::BridgeConstraintTest()
{
	Vector3 cubeSize = Vector3(10, 1, 4);

	float invCubeMass = 2;
	int numLinks = 10;
	float maxDistance = 15.0f;
	float cubeDistance = 15.0f;

	Vector3 startPos = Vector3(200, 200, 200);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	start->GetRenderObject()->SetColour(Debug::RED);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);
	end->GetRenderObject()->SetColour(Debug::GREEN);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; i++)
	{
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* distanceConstraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(distanceConstraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -20, 0));

	cubeLooker = AddEnemyToWorld(Vector3(25, 0, 0));
	cubeLooker->SetLayer(2);
	cubeLooker->GetRenderObject()->SetColour(Debug::RED);
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}

	//AddCubeToWorld(Vector3(0, 0, 0), Vector3(3, 1, 60), 0.1f);
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}

	if (selectionObject != nullptr)
	{
		//Debug::DrawLine(selectionObject->GetTransform().GetPosition(), selectionObject->GetTransform().GetPosition() + selectionObject->GetTransform().GetForward(), Debug::BLUE);
		//Debug::DrawAxisLines(selectionObject->GetTransform().GetMatrix(), 2.0f);
	}

	if (inSelectionMode) {
		Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;

				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				Debug::DrawLine(ray.GetPosition(), closestCollision.collidedAt, Debug::WHITE, 5.0f);
				Debug::DrawLine(closestCollision.collidedAt, closestCollision.collidedAt + closestCollision.collidedNormal, Debug::YELLOW, 5.0f);

				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------

#pragma region CW_GOAT_GAME
NCL::CSC8503::CWGoatGame::CWGoatGame()
{
	world = new GameWorld();
#ifdef USEVULKAN
	renderer = new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics = new PhysicsSystem(*world);
	//physics->SetGravity(Vector3(0, 9.81f, 0));

	useGravity = true;

	InitAssets();
	InitCamera();
	InitGoatWorld();
}

NCL::CSC8503::CWGoatGame::~CWGoatGame()
{
	delete world;
	delete renderer;
	delete physics;

	delete cubeMesh;
	delete basicTex;
	delete whiteTex;
	delete conceptTex;
	delete basicShader;
}

void NCL::CSC8503::CWGoatGame::UpdateGame(float dt)
{
	world->UpdateWorld(dt);
	world->GetMainCamera()->UpdateCamera(dt);

	renderer->Update(dt);
	physics->Update(dt);

	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void NCL::CSC8503::CWGoatGame::InitAssets()
{
	cubeMesh = renderer->LoadMesh("cube.msh");
	highRise3 = renderer->LoadMesh("Highrise_3.msh");
	highRise9 = renderer->LoadMesh("Highrise_9.msh");
	highRise12 = renderer->LoadMesh("Highrise_12.msh");
	highRise13 = renderer->LoadMesh("Highrise_13.msh");
	highRise18 = renderer->LoadMesh("Highrise_18.msh");

	basicTex = renderer->LoadTexture("checkerboard.png");
	whiteTex = renderer->LoadTexture("WhiteTex.png");
	conceptTex = renderer->LoadTexture("ConceptTexture.jpg");

	basicShader = renderer->LoadShader("scene.vert", "scene.frag");
}

void NCL::CSC8503::CWGoatGame::InitCamera()
{
	if (world == nullptr || world->GetMainCamera() == nullptr)
		return;

	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(15000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
}

void NCL::CSC8503::CWGoatGame::InitGoatWorld()
{
	world->ClearAndErase();
	physics->Clear();
	
	InitBaseObjects();
	InitSideWalks();
	InitBuildings();
}

void NCL::CSC8503::CWGoatGame::InitBaseObjects()
{
	AddCube(Vector3(0, 0, 0), Vector3(512.0f, 2.0f, 512.0f), Vector3(0, 0, 0), 0, Vector4(0.2f, 0.2f, 0.2f, 1.0f), "Floor", whiteTex);
	
	AddCube(Vector3(0, 64.0f, -512.0f), Vector3(512.0f, 64.0f, 2.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.8f, 0.6f, 1.0f), "Wall.B", whiteTex);
	AddCube(Vector3(0, 64.0f, 512.0f), Vector3(512.0f, 64.0f, 2.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.8f, 0.6f, 1.0f), "Wall.F", whiteTex);
	AddCube(Vector3(-512.0f, 64.0f, 0.0f), Vector3(2.0f, 64.0f, 512.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.8f, 0.6f, 1.0f), "Wall.L", whiteTex);
	AddCube(Vector3(512.0f, 64.0f, 0.0f), Vector3(2.0f, 64.0f, 512.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.8f, 0.6f, 1.0f), "Wall.R", whiteTex);
}

void NCL::CSC8503::CWGoatGame::InitSideWalks()
{
	AddCube(Vector3(0.0, 1.8f, -482.0f), Vector3(512.0f, 2.0f, 32.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk01", whiteTex);
	AddCube(Vector3(0.0, 1.8f, 482.0f), Vector3(512.0f, 2.0f, 32.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk02", whiteTex);
	AddCube(Vector3(482.0, 1.8f, 0.0f), Vector3(32.0f, 2.0f, 450.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk03", whiteTex);
	AddCube(Vector3(-482.0, 1.8f, 0.0f), Vector3(32.0f, 2.0f, 450.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);
	
	AddCube(Vector3(-256.0, 1.8f, 0.0f), Vector3(100.0f, 2.0f, 352.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);
	AddCube(Vector3(256.0, 1.8f, 256.0f), Vector3(100.0f, 2.0f, 100.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);
	AddCube(Vector3(256.0, 1.8f, 0.0f), Vector3(100.0f, 2.0f, 100.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);
	AddCube(Vector3(256.0, 1.8f, -256.0f), Vector3(100.0f, 2.0f, 100.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);
	
	AddCube(Vector3(0.0, 1.8f, -200.0f), Vector3(100.0f, 2.0f, 150.0f), Vector3(0, 0, 0), 0, Vector4(1.0f, 0.6f, 0.6f, 1.0f), "SideWalk04", whiteTex);
}

void NCL::CSC8503::CWGoatGame::InitBuildings()
{
	AddBuilding(Vector3(256.0f, 2.5f, -300.0f), Vector3(17.0f, 17.0f, 17.0f), Vector3(0, -90.0f, 0), 0, highRise13, conceptTex);
	AddBuilding(Vector3(300.0f, 2.5f, -200.0f), Vector3(16.0f, 16.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);
	AddBuilding(Vector3(216.0f, 2.5f, -200.0f), Vector3(16.0f, 16.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);
	
	AddBuilding(Vector3(300.0f, 2.5f, 0.0f), Vector3(25.0f, 25.0f, 25.0f), Vector3(0, 0.0f, 0), 0, highRise12, conceptTex);
	AddBuilding(Vector3(200.0f, 2.5f, 50.0f), Vector3(15.0f, 15.0f, 15.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddBuilding(Vector3(200.0f, 2.5f, -50.0f), Vector3(30.0f, 30.0f, 30.0f), Vector3(0, 0.0f, 0), 0, highRise3, conceptTex);

	AddBuilding(Vector3(-300.0f, 2.5f, 0.0f), Vector3(25.0f, 25.0f, 25.0f), Vector3(0, 0.0f, 0), 0, highRise12, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, -120.0f), Vector3(30.0f, 20.0f, 30.0f), Vector3(0, 0.0f, 0), 0, highRise3, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, -200.0f), Vector3(30.0f, 30.0f, 30.0f), Vector3(0, 0.0f, 0), 0, highRise3, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, -280.0f), Vector3(15.0f, 15.0f, 15.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, 0.0f), Vector3(15.0f, 20.0f, 15.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, 180.0f), Vector3(18.0f, 18.0f, 18.0f), Vector3(0, 0.0f, 0), 0, highRise13, conceptTex);
	AddBuilding(Vector3(-200.0f, 2.5f, 300.0f), Vector3(16.0f, 16.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);
	AddBuilding(Vector3(-300.0f, 2.5f, 300.0f), Vector3(16.0f, 10.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise18, conceptTex);
	AddBuilding(Vector3(-300.0f, 2.5f, 180.0f), Vector3(16.0f, 15.0f, 16.0f), Vector3(0, 0.0f, 0), 0, highRise9, conceptTex);
	AddBuilding(Vector3(-300.0f, 2.5f, -200.0f), Vector3(18.0f, 18.0f, 18.0f), Vector3(0, 0.0f, 0), 0, highRise13, conceptTex);
}

void NCL::CSC8503::CWGoatGame::AddCube(const Vector3& cubePos, const Vector3& cubeSize, const Vector3& cubeRot, const float& cubeMass, const Vector4& cubeColour, const std::string& cubeName, TextureBase* cubeTex)
{
	GameObject* cube = new GameObject(1, cubeName);

	AABBVolume* volume = new AABBVolume(cubeSize);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(cubePos)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(cubeRot.x, cubeRot.y, cubeRot.z))
		.SetScale(cubeSize * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, cubeTex, basicShader));
	cube->GetRenderObject()->SetColour(cubeColour);
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(cubeMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);
}

void NCL::CSC8503::CWGoatGame::AddBuilding(const Vector3& buildingPos, const Vector3& buildingSize, const Vector3& buildingRot, const float& buildingMass, MeshGeometry* buildingMesh, TextureBase* buildingTex)
{
	GameObject* building = new GameObject(1, "Building");

	AABBVolume* volume = new AABBVolume(buildingSize);
	building->SetBoundingVolume((CollisionVolume*)volume);

	building->GetTransform()
		.SetPosition(buildingPos)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(buildingRot.x, buildingRot.y, buildingRot.z))
		.SetScale(buildingSize * 2);

	building->SetRenderObject(new RenderObject(&building->GetTransform(), buildingMesh, buildingTex, basicShader));
	building->GetRenderObject()->SetColour(Debug::WHITE);
	building->SetPhysicsObject(new PhysicsObject(&building->GetTransform(), building->GetBoundingVolume()));

	building->GetPhysicsObject()->SetInverseMass(buildingMass);
	building->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(building);
}
#pragma endregion


//--------------------------------------------------------------------------------------------------------------
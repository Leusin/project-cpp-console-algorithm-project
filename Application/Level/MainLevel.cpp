#include "MainLevel.h"

#include "Actor/Unit/Unit.h"
#include "Math/Vector2I.h"

MainLevel::MainLevel()
{
	AddActor(new Unit({0, 0}));
	AddActor(new Unit({1, 5}));
	AddActor(new Unit({4, 10}));
	AddActor(new Unit({30, 15}));

}

MainLevel::~MainLevel()
{
	quradTree.Clear();
}

void MainLevel::BeginPlay()
{
	super::BeginPlay();
}

void MainLevel::Tick(float deltaTime)
{
	quradTree.Clear();
	auto actors = GetActors();
	for (const Actor* actor : actors)
	{
		if (actor->As<Unit>())
		{
			quradTree.Insert((Unit*)actor);
		}
	}
}

void MainLevel::Render()
{
	super::Render();

	quradTree.DrawBounds();
}

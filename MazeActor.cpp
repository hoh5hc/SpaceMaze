// Fill out your copyright notice in the Description page of Project Settings.

#include "MazeActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Components/BoxComponent.h"
#include "Materials/MaterialInterface.h"
#include "PickUpItem.h"
#include "Components/StaticMeshComponent.h"
#include "ObstacleItem.h"

// Sets default values
AMazeActor::AMazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent=Scene;

	ProceduralMaze = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMaze"));
	ProceduralMaze->SetupAttachment(Scene);
	ProceduralMaze->bUseAsyncCooking = true;
	//example task

}
void AMazeActor::PostActorCreated()
{
	Super::PostActorCreated();
	
}

void AMazeActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();


}


// Called when the game starts or when spawned
void AMazeActor::BeginPlay()
{
	Super::BeginPlay();

	//init grid data arcording to size
	MazeData.InitGrid(GridCol, GridRow);
	//init cell neighbor of grid
	MazeData.InitCellNeighbor();
	//choose algo to generate maze
	MazeData.AlgoRecursiveBacktrack();
	//init variable to output for generate mesh
	TArray<FVector> gridbase;
	TArray<FVector> gridvertical;
	TArray<FVector> gridhorizon;

	//init variable to paste to procedural mesh component
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;

	//calculate wall position
	MazeData.CalculateWallPosition(CellSize, gridbase, gridvertical, gridhorizon);


	GenerateProceduralBase(gridbase, Vertices, Triangles, Normals, UVs, Tangents, VertexColors);
	ProceduralMaze->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	ProceduralMaze->SetMaterial(0, ProceduralBaseMaterial);

	GenerateProceduralFloorStraight(Vertices, Triangles, Normals, UVs, Tangents, VertexColors);
	ProceduralMaze->CreateMeshSection(1, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	ProceduralMaze->SetMaterial(1, ProceduralFloorStraightMaterial);

	GenerateProceduralRoof(Vertices, Triangles, Normals, UVs, Tangents, VertexColors);
	ProceduralMaze->CreateMeshSection(2, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	ProceduralMaze->SetMaterial(2, ProceduralRoofMaterial);

	GenerateProceduralVertWall(gridvertical, Vertices, Triangles, Normals, UVs, Tangents, VertexColors);
	ProceduralMaze->CreateMeshSection(3, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	ProceduralMaze->SetMaterial(3, ProceduralVertWallMaterial);

	GenerateProceduralHoriWall(gridhorizon, Vertices, Triangles, Normals, UVs, Tangents, VertexColors);
	ProceduralMaze->CreateMeshSection(4, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	ProceduralMaze->SetMaterial(4, ProceduralHoriWallMaterial);

	GenerateProceduralFloorCorner(Vertices, Triangles, Normals, UVs, Tangents, VertexColors);
	ProceduralMaze->CreateMeshSection(5, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	ProceduralMaze->SetMaterial(5, ProceduralFloorCornerMaterial);

	GenerateProceduralFloorInter(Vertices, Triangles, Normals, UVs, Tangents, VertexColors);
	ProceduralMaze->CreateMeshSection(6, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	ProceduralMaze->SetMaterial(6, ProceduralFloorInterMaterial);

	ProceduralMaze->ContainsPhysicsTriMeshData(true);

	PlaceItem();


}

// Called every frame
void AMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMazeActor::PlaceItem()
{
	FActorSpawnParameters Spawninfo;
	Spawninfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	TArray<FVector> DeadEndCellPosition;
	FRotator rotate = FRotator(0);
	for (auto &cell : MazeData.GridCell)
	{
		if (cell.Link.Num() == 1) // dead end cell
		{
			FVector sth;
			CellPosition(cell.X, cell.Y, 0.5, 0.5, 0.5, sth);
			DeadEndCellPosition.Emplace(sth);
			//about to spawn award or money here
		}
		else if (cell.Link.Num() >= 3) // intersection cell
		{
			//some special effect or ability is allow to use here
		}
		else if (cell.Link.Num() == 2) //hallway cell
		{		
			FSquareCell *link1 = cell.Link[0];
			FSquareCell *link2 = cell.Link[1];
			int celldirection=2;
			if (link1->X == link2->X) //along x
			{
				celldirection = 0;
			}
			else if (link1->Y == link2->Y) //along y
			{
				celldirection = 1;
			}
			else //corner
			{
				celldirection = 2;
			}

			FVector sth;
			
			CellPosition(cell.X, cell.Y, 0.5, 0.5, 0.5, sth);
			int SpawnRand = FMath::RandRange(0, 100);
			if (SpawnRand < StaticObstacleSpawnPercent)
			{
				AObstacleItem* staticobstacle = GetWorld()->SpawnActor<AObstacleItem>(StaticObstacle[FMath::RandRange(0, StaticObstacle.Num() - 1)], sth, rotate, Spawninfo);
				staticobstacle->ActorSpawned(this, celldirection,0, cell.X, cell.Y);
			}
			else if (SpawnRand < ContObstacleSpawnPercent + StaticObstacleSpawnPercent)
			{
				for (int i = 0; i < ContSpawnNum; i++)
				{
					AObstacleItem* contobstacle = GetWorld()->SpawnActor<AObstacleItem>(ContObstacle[FMath::RandRange(0, ContObstacle.Num() - 1)], sth, rotate, Spawninfo);
					contobstacle->ActorSpawned(this, celldirection,i, cell.X, cell.Y);
				}
				
			}
			else
			{

			}
		}
		else  
		{
			//nothing to do here
			//never happen this
			// if it come to here
			//you are fucked
		}

		FVector sth;
		
		CellPosition(cell.X, cell.Y, 0.5, 0.5, 0.5, sth);
		APickUpItem* Light = GetWorld()->SpawnActor<APickUpItem>(LightSpawn, sth, rotate, Spawninfo);
		Light->ActorSpawned(this);

	}

	if (DeadEndCellPosition.Num()<3)
	{
		// not enough room to spawn all things
		//ther is at least 2 dead end so the other option was to remove key or spawn it somewhere else
		int indexlocation = FMath::RandRange(0, DeadEndCellPosition.Num() - 1);
		APickUpItem* startspawn = GetWorld()->SpawnActor<APickUpItem>(StartSpawn, DeadEndCellPosition[indexlocation], rotate, Spawninfo);
		DeadEndCellPosition.RemoveAt(indexlocation);
		startspawn->ActorSpawned(this);


		indexlocation = FMath::RandRange(0, DeadEndCellPosition.Num() - 1);
		APickUpItem* endspawn = GetWorld()->SpawnActor<APickUpItem>(EndSpawn, DeadEndCellPosition[indexlocation], rotate, Spawninfo);
		DeadEndCellPosition.RemoveAt(indexlocation);
		endspawn->ActorSpawned(this);
	}
	else
	{
		int indexlocation = FMath::RandRange(0, DeadEndCellPosition.Num() - 1);
		APickUpItem* startspawn = GetWorld()->SpawnActor<APickUpItem>(StartSpawn, DeadEndCellPosition[indexlocation], rotate, Spawninfo);
		DeadEndCellPosition.RemoveAt(indexlocation);
		startspawn->ActorSpawned(this);


		indexlocation = FMath::RandRange(0, DeadEndCellPosition.Num() - 1);
		APickUpItem* endspawn = GetWorld()->SpawnActor<APickUpItem>(EndSpawn, DeadEndCellPosition[indexlocation], rotate, Spawninfo);
		DeadEndCellPosition.RemoveAt(indexlocation);
		endspawn->ActorSpawned(this);


		indexlocation = FMath::RandRange(0, DeadEndCellPosition.Num() - 1);
		APickUpItem* keyspawn = GetWorld()->SpawnActor<APickUpItem>(KeySpawn, DeadEndCellPosition[indexlocation], rotate, Spawninfo);
		DeadEndCellPosition.RemoveAt(indexlocation);
		keyspawn->ActorSpawned(this);
	}


	for (auto &next : DeadEndCellPosition)
	{
		
		APickUpItem* rewardspawn = GetWorld()->SpawnActor<APickUpItem>(RewardSpawn[FMath::RandRange(0, RewardSpawn.Num() - 1)], next, rotate, Spawninfo);
		rewardspawn->ActorSpawned(this);
	}

}

void AMazeActor::GenerateProceduralRoof(TArray<FVector> &Vertices, TArray<int32> &Triangles, TArray<FVector> &Normals, TArray<FVector2D> &UVs, TArray<struct FProcMeshTangent> &Tangents, TArray<FColor> &VertexColors)
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	Tangents.Empty();
	VertexColors.Empty();

	int currentver = 0;

	TArray<FSquareCell> celldata = MazeData.GridCell.Array();
	for (int cell = 0; cell < celldata.Num(); cell++)
	{

		Vertices.Emplace(FVector(celldata[cell].X, celldata[cell].Y, 0) * UnitSize * CellSize + FVector(0, 0, WallHeight)*UnitSize);
		Vertices.Emplace(FVector(celldata[cell].X + 1, celldata[cell].Y, 0) * UnitSize * CellSize + FVector(0, 0, WallHeight)*UnitSize);
		Vertices.Emplace(FVector(celldata[cell].X + 1, celldata[cell].Y + 1, 0) * UnitSize * CellSize + FVector(0, 0, WallHeight)*UnitSize);
		Vertices.Emplace(FVector(celldata[cell].X, celldata[cell].Y + 1, 0) * UnitSize * CellSize + FVector(0, 0, WallHeight)*UnitSize);


		Triangles.Emplace(cell * 4 + 0 + currentver);// loop up hand note for explanation
		Triangles.Emplace(cell * 4 + 1 + currentver);
		Triangles.Emplace(cell * 4 + 3 + currentver);

		Triangles.Emplace(cell * 4 + 1 + currentver);// loop up hand note for explanation
		Triangles.Emplace(cell * 4 + 2 + currentver);
		Triangles.Emplace(cell * 4 + 3 + currentver);

		FProcMeshTangent tangent = FProcMeshTangent(Vertices[cell * 4 + 0 + currentver] - Vertices[cell * 4 + 1 + currentver], true);
		Tangents.Emplace(tangent);
		Tangents.Emplace(tangent);
		Tangents.Emplace(tangent);
		Tangents.Emplace(tangent);

		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 0));


		FVector normal = FVector::CrossProduct(Vertices[cell * 4 + 2 + currentver] - Vertices[cell * 4 + 1 + currentver], Vertices[cell * 4 + 2 + currentver] - Vertices[cell * 4 + 3 + currentver]).GetSafeNormal();
		Normals.Emplace(normal);
		Normals.Emplace(normal);
		Normals.Emplace(normal);
		Normals.Emplace(normal);
	}
}

void AMazeActor::GenerateProceduralFloorStraight(TArray<FVector> &Vertices, TArray<int32> &Triangles, TArray<FVector> &Normals, TArray<FVector2D> &UVs, TArray<struct FProcMeshTangent> &Tangents, TArray<FColor> &VertexColors)
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	Tangents.Empty();
	VertexColors.Empty();

	int currentver = 0;
	for (auto &cell:MazeData.GridCell)
	{

		if (cell.Link.Num() == 2)
		{


			FSquareCell *link1 = cell.Link[0];
			FSquareCell *link2 = cell.Link[1];
			if (link1->X == link2->X)
			{
				Vertices.Emplace(FVector(cell.X, cell.Y, 0) * UnitSize * CellSize);
				Vertices.Emplace(FVector(cell.X + 1, cell.Y, 0) * UnitSize * CellSize);
				Vertices.Emplace(FVector(cell.X + 1, cell.Y + 1, 0) * UnitSize * CellSize);
				Vertices.Emplace(FVector(cell.X, cell.Y + 1, 0) * UnitSize * CellSize);

				int verticenum = Vertices.Num();

				Triangles.Emplace(verticenum - 4 + 3 + currentver);// loop up hand note for explanation
				Triangles.Emplace(verticenum - 4 + 1 + currentver);
				Triangles.Emplace(verticenum - 4 + 0 + currentver);

				Triangles.Emplace(verticenum - 4 + 2 + currentver);// loop up hand note for explanation
				Triangles.Emplace(verticenum - 4 + 1 + currentver);
				Triangles.Emplace(verticenum - 4 + 3 + currentver);


				FProcMeshTangent tangent = FProcMeshTangent(Vertices[verticenum - 4 + 1 + currentver] - Vertices[verticenum - 4 + 0 + currentver], true);
				Tangents.Emplace(tangent);
				Tangents.Emplace(tangent);
				Tangents.Emplace(tangent);
				Tangents.Emplace(tangent);

				FVector normal = FVector::CrossProduct(Vertices[verticenum - 4 + 2 + currentver] - Vertices[verticenum - 4 + 3 + currentver], Vertices[verticenum - 4 + 2 + currentver] - Vertices[verticenum - 4 + 1 + currentver]).GetSafeNormal();
				Normals.Emplace(normal);
				Normals.Emplace(normal);
				Normals.Emplace(normal);
				Normals.Emplace(normal);

				UVs.Emplace(FVector2D(1, 0));
				UVs.Emplace(FVector2D(0, 0));
				UVs.Emplace(FVector2D(0, 1));
				UVs.Emplace(FVector2D(1, 1));
				
			}
			else if  (link1->Y == link2->Y)
			{
				Vertices.Emplace(FVector(cell.X, cell.Y, 0) * UnitSize * CellSize);
				Vertices.Emplace(FVector(cell.X + 1, cell.Y, 0) * UnitSize * CellSize);
				Vertices.Emplace(FVector(cell.X + 1, cell.Y + 1, 0) * UnitSize * CellSize);
				Vertices.Emplace(FVector(cell.X, cell.Y + 1, 0) * UnitSize * CellSize);

				int verticenum = Vertices.Num();

				Triangles.Emplace(verticenum - 4 + 3 + currentver);// loop up hand note for explanation
				Triangles.Emplace(verticenum - 4 + 1 + currentver);
				Triangles.Emplace(verticenum - 4 + 0 + currentver);

				Triangles.Emplace(verticenum - 4 + 2 + currentver);// loop up hand note for explanation
				Triangles.Emplace(verticenum - 4 + 1 + currentver);
				Triangles.Emplace(verticenum - 4 + 3 + currentver);


				FProcMeshTangent tangent = FProcMeshTangent(Vertices[verticenum - 4 + 1 + currentver] - Vertices[verticenum - 4 + 0 + currentver], true);
				Tangents.Emplace(tangent);
				Tangents.Emplace(tangent);
				Tangents.Emplace(tangent);
				Tangents.Emplace(tangent);

				FVector normal = FVector::CrossProduct(Vertices[verticenum - 4 + 2 + currentver] - Vertices[verticenum - 4 + 3 + currentver], Vertices[verticenum - 4 + 2 + currentver] - Vertices[verticenum - 4 + 1 + currentver]).GetSafeNormal();
				Normals.Emplace(normal);
				Normals.Emplace(normal);
				Normals.Emplace(normal);
				Normals.Emplace(normal);

				UVs.Emplace(FVector2D(0, 0));
				UVs.Emplace(FVector2D(0, 1));
				UVs.Emplace(FVector2D(1, 1));
				UVs.Emplace(FVector2D(1, 0));
				


			}


		}
	}

}

void AMazeActor::GenerateProceduralFloorCorner(TArray<FVector> &Vertices, TArray<int32> &Triangles, TArray<FVector> &Normals, TArray<FVector2D> &UVs, TArray<struct FProcMeshTangent> &Tangents, TArray<FColor> &VertexColors)
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	Tangents.Empty();
	VertexColors.Empty();

	int currentver = 0;
	for (auto &cell : MazeData.GridCell)
	{

		if (cell.Link.Num() == 2)

		{
			FSquareCell *link1 = cell.Link[0];
			FSquareCell *link2 = cell.Link[1];
			if (link1->X == link2->X)
			{
			}
			else if (link1->Y == link2->Y)
			{
				
			}
			else
			{
				int x = 2 * cell.X - link1->X - link2->X;
				int y = 2 * cell.Y - link1->Y - link2->Y;

				Vertices.Emplace(FVector(cell.X, cell.Y, 0) * UnitSize * CellSize);
				Vertices.Emplace(FVector(cell.X + 1, cell.Y, 0) * UnitSize * CellSize);
				Vertices.Emplace(FVector(cell.X + 1, cell.Y + 1, 0) * UnitSize * CellSize);
				Vertices.Emplace(FVector(cell.X, cell.Y + 1, 0) * UnitSize * CellSize);

				int verticenum = Vertices.Num();

				Triangles.Emplace(verticenum - 4 + 3 + currentver);// loop up hand note for explanation
				Triangles.Emplace(verticenum - 4 + 1 + currentver);
				Triangles.Emplace(verticenum - 4 + 0 + currentver);

				Triangles.Emplace(verticenum - 4 + 2 + currentver);// loop up hand note for explanation
				Triangles.Emplace(verticenum - 4 + 1 + currentver);
				Triangles.Emplace(verticenum - 4 + 3 + currentver);


				FProcMeshTangent tangent = FProcMeshTangent(Vertices[verticenum - 4 + 1 + currentver] - Vertices[verticenum - 4 + 0 + currentver], true);
				Tangents.Emplace(tangent);
				Tangents.Emplace(tangent);
				Tangents.Emplace(tangent);
				Tangents.Emplace(tangent);

				FVector normal = FVector::CrossProduct(Vertices[verticenum - 4 + 2 + currentver] - Vertices[verticenum - 4 + 3 + currentver], Vertices[verticenum - 4 + 2 + currentver] - Vertices[verticenum - 4 + 1 + currentver]).GetSafeNormal();
				Normals.Emplace(normal);
				Normals.Emplace(normal);
				Normals.Emplace(normal);
				Normals.Emplace(normal);
				if (x == -1 && y == -1)
				{
					UVs.Emplace(FVector2D(1, 1));
					UVs.Emplace(FVector2D(1, 0));
					UVs.Emplace(FVector2D(0, 0));
					UVs.Emplace(FVector2D(0, 1));

				}
				else if (x == -1 && y == 1)
				{

					UVs.Emplace(FVector2D(1, 0));
					UVs.Emplace(FVector2D(0, 0));
					UVs.Emplace(FVector2D(0, 1));
					UVs.Emplace(FVector2D(1, 1));

				}
				else if (x == 1 && y == 1)
				{

					UVs.Emplace(FVector2D(0, 0));
					UVs.Emplace(FVector2D(0, 1));
					UVs.Emplace(FVector2D(1, 1));
					UVs.Emplace(FVector2D(1, 0));
				}
				else if (x == 1 && y == -1)
				{
					
					UVs.Emplace(FVector2D(0, 1));
					UVs.Emplace(FVector2D(1, 1));
					UVs.Emplace(FVector2D(1, 0));
					UVs.Emplace(FVector2D(0, 0));
				}


			}


		}
	}

}

void AMazeActor::GenerateProceduralFloorInter(TArray<FVector> &Vertices, TArray<int32> &Triangles, TArray<FVector> &Normals, TArray<FVector2D> &UVs, TArray<struct FProcMeshTangent> &Tangents, TArray<FColor> &VertexColors)
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	Tangents.Empty();
	VertexColors.Empty();

	int currentver = 0;
	for (auto &cell : MazeData.GridCell)
	{

		if (cell.Link.Num() == 2)
		{

		}
		else
		{

			Vertices.Emplace(FVector(cell.X, cell.Y, 0) * UnitSize * CellSize);
			Vertices.Emplace(FVector(cell.X + 1, cell.Y, 0) * UnitSize * CellSize);
			Vertices.Emplace(FVector(cell.X + 1, cell.Y + 1, 0) * UnitSize * CellSize);
			Vertices.Emplace(FVector(cell.X, cell.Y + 1, 0) * UnitSize * CellSize);

			int verticenum = Vertices.Num();

			Triangles.Emplace(verticenum - 4 + 3 + currentver);// loop up hand note for explanation
			Triangles.Emplace(verticenum - 4 + 1 + currentver);
			Triangles.Emplace(verticenum - 4 + 0 + currentver);

			Triangles.Emplace(verticenum - 4 + 2 + currentver);// loop up hand note for explanation
			Triangles.Emplace(verticenum - 4 + 1 + currentver);
			Triangles.Emplace(verticenum - 4 + 3 + currentver);


			FProcMeshTangent tangent = FProcMeshTangent(Vertices[verticenum - 4 + 1 + currentver] - Vertices[verticenum - 4 + 0 + currentver], true);
			Tangents.Emplace(tangent);
			Tangents.Emplace(tangent);
			Tangents.Emplace(tangent);
			Tangents.Emplace(tangent);

			FVector normal = FVector::CrossProduct(Vertices[verticenum - 4 + 2 + currentver] - Vertices[verticenum - 4 + 3 + currentver], Vertices[verticenum - 4 + 2 + currentver] - Vertices[verticenum - 4 + 1 + currentver]).GetSafeNormal();
			Normals.Emplace(normal);
			Normals.Emplace(normal);
			Normals.Emplace(normal);
			Normals.Emplace(normal);

			UVs.Emplace(FVector2D(0, 0));
			UVs.Emplace(FVector2D(0, 1));
			UVs.Emplace(FVector2D(1, 1));
			UVs.Emplace(FVector2D(1, 0));

		}

	}
}



void AMazeActor::GenerateProceduralBase(TArray<FVector> gridbase, TArray<FVector> &Vertices, TArray<int32> &Triangles, TArray<FVector> &Normals, TArray<FVector2D> &UVs, TArray<struct FProcMeshTangent> &Tangents, TArray<FColor> &VertexColors)
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	Tangents.Empty();
	VertexColors.Empty();

	for (int cell = 0; cell < gridbase.Num(); cell++)
	{
		for (auto &direction : VerticeDirection)
		{
			Vertices.Emplace((gridbase[cell] + FVector(0, 0, +WallHeight) + direction / (IsBigCollumn ? 1 : 2))*UnitSize);
			Vertices.Emplace((gridbase[cell] + FVector(0, 0, +WallHeight) + direction / (IsBigCollumn ? 1 : 2))*UnitSize);
			Vertices.Emplace((gridbase[cell] + FVector(0, 0, +WallHeight) + direction / (IsBigCollumn ? 1 : 2))*UnitSize);

			Vertices.Emplace((gridbase[cell] + direction / (IsBigCollumn ? 1 : 2))*UnitSize);
			Vertices.Emplace((gridbase[cell] + direction / (IsBigCollumn ? 1 : 2))*UnitSize);
			Vertices.Emplace((gridbase[cell] + direction / (IsBigCollumn ? 1 : 2))*UnitSize);

			VertexColors.Emplace(FColor(255, 0, 0));
			VertexColors.Emplace(FColor(255, 0, 0));
			VertexColors.Emplace(FColor(255, 0, 0));
			VertexColors.Emplace(FColor(255, 0, 0));
			VertexColors.Emplace(FColor(255, 0, 0));
			VertexColors.Emplace(FColor(255, 0, 0));


		}

		Triangles.Emplace(cell * 24 + 18);// loop up hand note for explanation
		Triangles.Emplace(cell * 24 + 12);
		Triangles.Emplace(cell * 24 + 0);

		Triangles.Emplace(cell * 24 + 6);
		Triangles.Emplace(cell * 24 + 18);
		Triangles.Emplace(cell * 24 + 0);

		Triangles.Emplace(cell * 24 + 3);
		Triangles.Emplace(cell * 24 + 15);
		Triangles.Emplace(cell * 24 + 21);

		Triangles.Emplace(cell * 24 + 3);
		Triangles.Emplace(cell * 24 + 21);
		Triangles.Emplace(cell * 24 + 9);

		Triangles.Emplace(cell * 24 + 22);
		Triangles.Emplace(cell * 24 + 16);
		Triangles.Emplace(cell * 24 + 13);

		Triangles.Emplace(cell * 24 + 19);
		Triangles.Emplace(cell * 24 + 22);
		Triangles.Emplace(cell * 24 + 13);

		Triangles.Emplace(cell * 24 + 10);
		Triangles.Emplace(cell * 24 + 23);
		Triangles.Emplace(cell * 24 + 20);

		Triangles.Emplace(cell * 24 + 7);
		Triangles.Emplace(cell * 24 + 10);
		Triangles.Emplace(cell * 24 + 20);

		Triangles.Emplace(cell * 24 + 4);
		Triangles.Emplace(cell * 24 + 11);
		Triangles.Emplace(cell * 24 + 8);

		Triangles.Emplace(cell * 24 + 1);
		Triangles.Emplace(cell * 24 + 4);
		Triangles.Emplace(cell * 24 + 8);

		Triangles.Emplace(cell * 24 + 17);
		Triangles.Emplace(cell * 24 + 5);
		Triangles.Emplace(cell * 24 + 2);

		Triangles.Emplace(cell * 24 + 14);
		Triangles.Emplace(cell * 24 + 17);
		Triangles.Emplace(cell * 24 + 2);


		FProcMeshTangent tangent1 = FProcMeshTangent(Vertices[cell * 24 + 6] - Vertices[cell * 24 + 0], true);
		FProcMeshTangent tangent2 = FProcMeshTangent(Vertices[cell * 24 + 9] - Vertices[cell * 24 + 3], true);
		FProcMeshTangent tangent3 = FProcMeshTangent(Vertices[cell * 24 + 19] - Vertices[cell * 24 + 13], true);
		FProcMeshTangent tangent4 = FProcMeshTangent(Vertices[cell * 24 + 7] - Vertices[cell * 24 + 20], true);
		FProcMeshTangent tangent5 = FProcMeshTangent(Vertices[cell * 24 + 1] - Vertices[cell * 24 + 8], true);
		FProcMeshTangent tangent6 = FProcMeshTangent(Vertices[cell * 24 + 14] - Vertices[cell * 24 + 2], true);

		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent4);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent4);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent4);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent4);




		UVs.Emplace(FVector2D(0, 0));// loop up hand note for explanation
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(0, 1));



		FVector normal1 = FVector::CrossProduct(Vertices[cell * 24 + 6] - Vertices[cell * 24 + 0], Vertices[cell * 24 + 6] - Vertices[cell * 24 + 18]).GetSafeNormal();
		FVector normal2 = FVector::CrossProduct(Vertices[cell * 24 + 9] - Vertices[cell * 24 + 3], Vertices[cell * 24 + 9] - Vertices[cell * 24 + 21]).GetSafeNormal();
		FVector normal3 = FVector::CrossProduct(Vertices[cell * 24 + 19] - Vertices[cell * 24 + 13], Vertices[cell * 24 + 19] - Vertices[cell * 24 + 22]).GetSafeNormal();
		FVector normal4 = FVector::CrossProduct(Vertices[cell * 24 + 7] - Vertices[cell * 24 + 20], Vertices[cell * 24 + 7] - Vertices[cell * 24 + 10]).GetSafeNormal();
		FVector normal5 = FVector::CrossProduct(Vertices[cell * 24 + 1] - Vertices[cell * 24 + 8], Vertices[cell * 24 + 1] - Vertices[cell * 24 + 4]).GetSafeNormal();
		FVector normal6 = FVector::CrossProduct(Vertices[cell * 24 + 14] - Vertices[cell * 24 + 2], Vertices[cell * 24 + 14] - Vertices[cell * 24 + 17]).GetSafeNormal();

		Normals.Emplace(normal1);
		Normals.Emplace(normal5);
		Normals.Emplace(normal6);
		Normals.Emplace(normal2);
		Normals.Emplace(normal5);
		Normals.Emplace(normal6);
		Normals.Emplace(normal1);
		Normals.Emplace(normal4);
		Normals.Emplace(normal5);
		Normals.Emplace(normal2);
		Normals.Emplace(normal4);
		Normals.Emplace(normal5);
		Normals.Emplace(normal1);
		Normals.Emplace(normal3);
		Normals.Emplace(normal6);
		Normals.Emplace(normal2);
		Normals.Emplace(normal3);
		Normals.Emplace(normal6);
		Normals.Emplace(normal1);
		Normals.Emplace(normal3);
		Normals.Emplace(normal4);
		Normals.Emplace(normal2);
		Normals.Emplace(normal3);
		Normals.Emplace(normal4);

	}

}



void AMazeActor::GenerateProceduralVertWall(TArray<FVector> gridvertical, TArray<FVector> &Vertices, TArray<int32> &Triangles, TArray<FVector> &Normals, TArray<FVector2D> &UVs, TArray<FProcMeshTangent> &Tangents, TArray<FColor> &VertexColors)
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	Tangents.Empty();
	VertexColors.Empty();

	int currentver =0;

	for (int cell = 0; cell < gridvertical.Num(); cell++)
	{
		for (auto &direction : VerticeDirection)
		{
			Vertices.Emplace((gridvertical[cell] + FVector(0, 0, +WallHeight) + direction * FVector(1, CellSize - (IsBigCollumn ? 2 : 1), 1) / 2)*UnitSize);
			Vertices.Emplace((gridvertical[cell] + FVector(0, 0, +WallHeight) + direction * FVector(1, CellSize - (IsBigCollumn ? 2 : 1), 1) / 2)*UnitSize);
			Vertices.Emplace((gridvertical[cell] + FVector(0, 0, +WallHeight) + direction * FVector(1, CellSize - (IsBigCollumn ? 2 : 1), 1) / 2)*UnitSize);

			Vertices.Emplace((gridvertical[cell] + direction * FVector(1, CellSize - (IsBigCollumn ? 2 : 1), 1) / 2)*UnitSize);
			Vertices.Emplace((gridvertical[cell] + direction * FVector(1, CellSize - (IsBigCollumn ? 2 : 1), 1) / 2)*UnitSize);
			Vertices.Emplace((gridvertical[cell] + direction * FVector(1, CellSize - (IsBigCollumn ? 2 : 1), 1) / 2)*UnitSize);

			VertexColors.Emplace(FColor(0, 255, 0));
			VertexColors.Emplace(FColor(0, 255, 0));
			VertexColors.Emplace(FColor(0, 255, 0));
			VertexColors.Emplace(FColor(0, 255, 0));
			VertexColors.Emplace(FColor(0, 255, 0));
			VertexColors.Emplace(FColor(0, 255, 0));

		}

		Triangles.Emplace(cell * 24 + 18 + currentver);// loop up hand note for explanation
		Triangles.Emplace(cell * 24 + 12 + currentver);
		Triangles.Emplace(cell * 24 + 0 + currentver);

		Triangles.Emplace(cell * 24 + 6 + currentver);
		Triangles.Emplace(cell * 24 + 18 + currentver);
		Triangles.Emplace(cell * 24 + 0 + currentver);

		Triangles.Emplace(cell * 24 + 3 + currentver);
		Triangles.Emplace(cell * 24 + 15 + currentver);
		Triangles.Emplace(cell * 24 + 21 + currentver);

		Triangles.Emplace(cell * 24 + 3 + currentver);
		Triangles.Emplace(cell * 24 + 21 + currentver);
		Triangles.Emplace(cell * 24 + 9 + currentver);

		Triangles.Emplace(cell * 24 + 22 + currentver);
		Triangles.Emplace(cell * 24 + 16 + currentver);
		Triangles.Emplace(cell * 24 + 13 + currentver);

		Triangles.Emplace(cell * 24 + 19 + currentver);
		Triangles.Emplace(cell * 24 + 22 + currentver);
		Triangles.Emplace(cell * 24 + 13 + currentver);

		Triangles.Emplace(cell * 24 + 10 + currentver);
		Triangles.Emplace(cell * 24 + 23 + currentver);
		Triangles.Emplace(cell * 24 + 20 + currentver);

		Triangles.Emplace(cell * 24 + 7 + currentver);
		Triangles.Emplace(cell * 24 + 10 + currentver);
		Triangles.Emplace(cell * 24 + 20 + currentver);

		Triangles.Emplace(cell * 24 + 4 + currentver);
		Triangles.Emplace(cell * 24 + 11 + currentver);
		Triangles.Emplace(cell * 24 + 8 + currentver);

		Triangles.Emplace(cell * 24 + 1 + currentver);
		Triangles.Emplace(cell * 24 + 4 + currentver);
		Triangles.Emplace(cell * 24 + 8 + currentver);

		Triangles.Emplace(cell * 24 + 17 + currentver);
		Triangles.Emplace(cell * 24 + 5 + currentver);
		Triangles.Emplace(cell * 24 + 2 + currentver);

		Triangles.Emplace(cell * 24 + 14 + currentver);
		Triangles.Emplace(cell * 24 + 17 + currentver);
		Triangles.Emplace(cell * 24 + 2 + currentver);


		FProcMeshTangent tangent1 = FProcMeshTangent(Vertices[cell * 24 + 6 + currentver] - Vertices[cell * 24 + 0 + currentver], true);
		FProcMeshTangent tangent2 = FProcMeshTangent(Vertices[cell * 24 + 9 + currentver] - Vertices[cell * 24 + 3 + currentver], true);
		FProcMeshTangent tangent3 = FProcMeshTangent(Vertices[cell * 24 + 19 + currentver] - Vertices[cell * 24 + 13 + currentver], true);
		FProcMeshTangent tangent4 = FProcMeshTangent(Vertices[cell * 24 + 7 + currentver] - Vertices[cell * 24 + 20 + currentver], true);
		FProcMeshTangent tangent5 = FProcMeshTangent(Vertices[cell * 24 + 1 + currentver] - Vertices[cell * 24 + 8 + currentver], true);
		FProcMeshTangent tangent6 = FProcMeshTangent(Vertices[cell * 24 + 14 + currentver] - Vertices[cell * 24 + 2 + currentver], true);

		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent4);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent4);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent4);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent4);




		UVs.Emplace(FVector2D(0, 0));// loop up hand note for explanation
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(0, 1));



		FVector normal1 = FVector::CrossProduct(Vertices[cell * 24 + 6 + currentver] - Vertices[cell * 24 + 0 + currentver], Vertices[cell * 24 + 6 + currentver] - Vertices[cell * 24 + 18 + currentver]).GetSafeNormal();
		FVector normal2 = FVector::CrossProduct(Vertices[cell * 24 + 9 + currentver] - Vertices[cell * 24 + 3 + currentver], Vertices[cell * 24 + 9 + currentver] - Vertices[cell * 24 + 21 + currentver]).GetSafeNormal();
		FVector normal3 = FVector::CrossProduct(Vertices[cell * 24 + 19 + currentver] - Vertices[cell * 24 + 13 + currentver], Vertices[cell * 24 + 19 + currentver] - Vertices[cell * 24 + 22 + currentver]).GetSafeNormal();
		FVector normal4 = FVector::CrossProduct(Vertices[cell * 24 + 7 + currentver] - Vertices[cell * 24 + 20 + currentver], Vertices[cell * 24 + 7 + currentver] - Vertices[cell * 24 + 10 + currentver]).GetSafeNormal();
		FVector normal5 = FVector::CrossProduct(Vertices[cell * 24 + 1 + currentver] - Vertices[cell * 24 + 8 + currentver], Vertices[cell * 24 + 1 + currentver] - Vertices[cell * 24 + 4 + currentver]).GetSafeNormal();
		FVector normal6 = FVector::CrossProduct(Vertices[cell * 24 + 14 + currentver] - Vertices[cell * 24 + 2 + currentver], Vertices[cell * 24 + 14 + currentver] - Vertices[cell * 24 + 17 + currentver]).GetSafeNormal();

		Normals.Emplace(normal1);
		Normals.Emplace(normal5);
		Normals.Emplace(normal6);
		Normals.Emplace(normal2);
		Normals.Emplace(normal5);
		Normals.Emplace(normal6);
		Normals.Emplace(normal1);
		Normals.Emplace(normal4);
		Normals.Emplace(normal5);
		Normals.Emplace(normal2);
		Normals.Emplace(normal4);
		Normals.Emplace(normal5);
		Normals.Emplace(normal1);
		Normals.Emplace(normal3);
		Normals.Emplace(normal6);
		Normals.Emplace(normal2);
		Normals.Emplace(normal3);
		Normals.Emplace(normal6);
		Normals.Emplace(normal1);
		Normals.Emplace(normal3);
		Normals.Emplace(normal4);
		Normals.Emplace(normal2);
		Normals.Emplace(normal3);
		Normals.Emplace(normal4);
	}


	
}

void AMazeActor::GenerateProceduralHoriWall(TArray<FVector> gridhorizon, TArray<FVector> &Vertices, TArray<int32> &Triangles, TArray<FVector> &Normals, TArray<FVector2D> &UVs, TArray<FProcMeshTangent> &Tangents, TArray<FColor> &VertexColors)
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	Tangents.Empty();
	VertexColors.Empty();

	int currentver = 0;

	for (int cell = 0; cell < gridhorizon.Num(); cell++)
	{
		for (auto &direction : VerticeDirection)
		{
			Vertices.Emplace((gridhorizon[cell] + FVector(0, 0, +WallHeight) + direction * FVector(CellSize - (IsBigCollumn ? 2 : 1), 1, 1) / 2)*UnitSize);
			Vertices.Emplace((gridhorizon[cell] + FVector(0, 0, +WallHeight) + direction * FVector(CellSize - (IsBigCollumn ? 2 : 1), 1, 1) / 2)*UnitSize);
			Vertices.Emplace((gridhorizon[cell] + FVector(0, 0, +WallHeight) + direction * FVector(CellSize - (IsBigCollumn ? 2 : 1), 1, 1) / 2)*UnitSize);

			Vertices.Emplace((gridhorizon[cell] + direction * FVector(CellSize - (IsBigCollumn ? 2 : 1), 1, 1) / 2)*UnitSize);
			Vertices.Emplace((gridhorizon[cell] + direction * FVector(CellSize - (IsBigCollumn ? 2 : 1), 1, 1) / 2)*UnitSize);
			Vertices.Emplace((gridhorizon[cell] + direction * FVector(CellSize - (IsBigCollumn ? 2 : 1), 1, 1) / 2)*UnitSize);

			VertexColors.Emplace(FColor(0, 0, 255));
			VertexColors.Emplace(FColor(0, 0, 255));
			VertexColors.Emplace(FColor(0, 0, 255));
			VertexColors.Emplace(FColor(0, 0, 255));
			VertexColors.Emplace(FColor(0, 0, 255));
			VertexColors.Emplace(FColor(0, 0, 255));
		}

		Triangles.Emplace(cell * 24 + 18 + currentver);// loop up hand note for explanation
		Triangles.Emplace(cell * 24 + 12 + currentver);
		Triangles.Emplace(cell * 24 + 0 + currentver);

		Triangles.Emplace(cell * 24 + 6 + currentver);
		Triangles.Emplace(cell * 24 + 18 + currentver);
		Triangles.Emplace(cell * 24 + 0 + currentver);

		Triangles.Emplace(cell * 24 + 3 + currentver);
		Triangles.Emplace(cell * 24 + 15 + currentver);
		Triangles.Emplace(cell * 24 + 21 + currentver);

		Triangles.Emplace(cell * 24 + 3 + currentver);
		Triangles.Emplace(cell * 24 + 21 + currentver);
		Triangles.Emplace(cell * 24 + 9 + currentver);

		Triangles.Emplace(cell * 24 + 22 + currentver);
		Triangles.Emplace(cell * 24 + 16 + currentver);
		Triangles.Emplace(cell * 24 + 13 + currentver);

		Triangles.Emplace(cell * 24 + 19 + currentver);
		Triangles.Emplace(cell * 24 + 22 + currentver);
		Triangles.Emplace(cell * 24 + 13 + currentver);

		Triangles.Emplace(cell * 24 + 10 + currentver);
		Triangles.Emplace(cell * 24 + 23 + currentver);
		Triangles.Emplace(cell * 24 + 20 + currentver);

		Triangles.Emplace(cell * 24 + 7 + currentver);
		Triangles.Emplace(cell * 24 + 10 + currentver);
		Triangles.Emplace(cell * 24 + 20 + currentver);

		Triangles.Emplace(cell * 24 + 4 + currentver);
		Triangles.Emplace(cell * 24 + 11 + currentver);
		Triangles.Emplace(cell * 24 + 8 + currentver);

		Triangles.Emplace(cell * 24 + 1 + currentver);
		Triangles.Emplace(cell * 24 + 4 + currentver);
		Triangles.Emplace(cell * 24 + 8 + currentver);

		Triangles.Emplace(cell * 24 + 17 + currentver);
		Triangles.Emplace(cell * 24 + 5 + currentver);
		Triangles.Emplace(cell * 24 + 2 + currentver);

		Triangles.Emplace(cell * 24 + 14 + currentver);
		Triangles.Emplace(cell * 24 + 17 + currentver);
		Triangles.Emplace(cell * 24 + 2 + currentver);


		FProcMeshTangent tangent1 = FProcMeshTangent(Vertices[cell * 24 + 6 + currentver] - Vertices[cell * 24 + 0 + currentver], true);
		FProcMeshTangent tangent2 = FProcMeshTangent(Vertices[cell * 24 + 9 + currentver] - Vertices[cell * 24 + 3 + currentver], true);
		FProcMeshTangent tangent3 = FProcMeshTangent(Vertices[cell * 24 + 19 + currentver] - Vertices[cell * 24 + 13 + currentver], true);
		FProcMeshTangent tangent4 = FProcMeshTangent(Vertices[cell * 24 + 7 + currentver] - Vertices[cell * 24 + 20 + currentver], true);
		FProcMeshTangent tangent5 = FProcMeshTangent(Vertices[cell * 24 + 1 + currentver] - Vertices[cell * 24 + 8 + currentver], true);
		FProcMeshTangent tangent6 = FProcMeshTangent(Vertices[cell * 24 + 14 + currentver] - Vertices[cell * 24 + 2 + currentver], true);

		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent4);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent4);
		Tangents.Emplace(tangent5);
		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent6);
		Tangents.Emplace(tangent1);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent4);
		Tangents.Emplace(tangent2);
		Tangents.Emplace(tangent3);
		Tangents.Emplace(tangent4);




		UVs.Emplace(FVector2D(0, 0));// loop up hand note for explanation
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(0, 1));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(1, 0));
		UVs.Emplace(FVector2D(0, 0));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(1, 1));
		UVs.Emplace(FVector2D(0, 1));



		FVector normal1 = FVector::CrossProduct(Vertices[cell * 24 + 6 + currentver] - Vertices[cell * 24 + 0 + currentver], Vertices[cell * 24 + 6 + currentver] - Vertices[cell * 24 + 18 + currentver]).GetSafeNormal();
		FVector normal2 = FVector::CrossProduct(Vertices[cell * 24 + 9 + currentver] - Vertices[cell * 24 + 3 + currentver], Vertices[cell * 24 + 9 + currentver] - Vertices[cell * 24 + 21 + currentver]).GetSafeNormal();
		FVector normal3 = FVector::CrossProduct(Vertices[cell * 24 + 19 + currentver] - Vertices[cell * 24 + 13 + currentver], Vertices[cell * 24 + 19 + currentver] - Vertices[cell * 24 + 22 + currentver]).GetSafeNormal();
		FVector normal4 = FVector::CrossProduct(Vertices[cell * 24 + 7 + currentver] - Vertices[cell * 24 + 20 + currentver], Vertices[cell * 24 + 7 + currentver] - Vertices[cell * 24 + 10 + currentver]).GetSafeNormal();
		FVector normal5 = FVector::CrossProduct(Vertices[cell * 24 + 1 + currentver] - Vertices[cell * 24 + 8 + currentver], Vertices[cell * 24 + 1 + currentver] - Vertices[cell * 24 + 4 + currentver]).GetSafeNormal();
		FVector normal6 = FVector::CrossProduct(Vertices[cell * 24 + 14 + currentver] - Vertices[cell * 24 + 2 + currentver], Vertices[cell * 24 + 14 + currentver] - Vertices[cell * 24 + 17 + currentver]).GetSafeNormal();

		Normals.Emplace(normal1);
		Normals.Emplace(normal5);
		Normals.Emplace(normal6);
		Normals.Emplace(normal2);
		Normals.Emplace(normal5);
		Normals.Emplace(normal6);
		Normals.Emplace(normal1);
		Normals.Emplace(normal4);
		Normals.Emplace(normal5);
		Normals.Emplace(normal2);
		Normals.Emplace(normal4);
		Normals.Emplace(normal5);
		Normals.Emplace(normal1);
		Normals.Emplace(normal3);
		Normals.Emplace(normal6);
		Normals.Emplace(normal2);
		Normals.Emplace(normal3);
		Normals.Emplace(normal6);
		Normals.Emplace(normal1);
		Normals.Emplace(normal3);
		Normals.Emplace(normal4);
		Normals.Emplace(normal2);
		Normals.Emplace(normal3);
		Normals.Emplace(normal4);
	}

}

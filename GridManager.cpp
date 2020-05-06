// Fill out your copyright notice in the Description page of Project Settings.

#include "GridManager.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AGridManager::AGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	scene = CreateDefaultSubobject<USceneComponent>(TEXT("rootscene"));
	scene->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


FVector AGridManager::SquareCellToVector(FVector2D squarecell)
{

	return(FVector(squarecell.X, squarecell.Y, 0) * GridSize);

}

FVector2D AGridManager::VectorToSquareCell(FVector vector)
{
	float x = roundl((vector.X) / GridSize);
	float y = roundl((vector.Y) / GridSize);
	return FVector2D(x, y);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"


UCLASS()
class CASTLEDEFENSE_API AGridManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Category = scene, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* scene;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		int GridSize = 200;

public:

	UFUNCTION(BlueprintCallable, Category = "Grid")
		FVector SquareCellToVector(FVector2D squarecell);

	UFUNCTION(BlueprintCallable, Category = "Grid")
		FVector2D VectorToSquareCell(FVector vector);

};

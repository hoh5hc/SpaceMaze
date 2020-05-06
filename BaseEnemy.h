// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/BaseActor.h"
#include "BaseEnemy.generated.h"

UCLASS()
class CASTLEDEFENSE_API ABaseEnemy : public ABaseActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector CurrentVelocity = FVector(0, -500.0f, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer)
	float CyclicRate = 0.5f;

	FTimerHandle CyclicTimerHandle;

	UFUNCTION()
	virtual void CyclicTimer();

	class AGridManager* GridManager;

	bool IsAttack = false;
};

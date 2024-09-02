// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Grid/GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/Tower/BaseTower.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//hi im change sth

}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(CyclicTimerHandle, this, &ABaseEnemy::CyclicTimer, CyclicRate, true, CyclicRate);

	TArray<AActor*> foundactor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), foundactor);

	GridManager = Cast<AGridManager>(foundactor[0]);
	//master do again in web
	//master do sth here
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsAttack)
	{
		if (!CurrentVelocity.IsZero())
		{

			FVector NewLocation = CurrentVelocity * DeltaTime;
			//FRotator oldrotator = GetActorRotation();
			//FRotator oldrotator1 = GetActorRotation();
			//oldrotator1.Pitch = 0;
			//SetActorRotation(oldrotator1);
			AddActorLocalOffset(NewLocation);
			//SetActorRotation(oldrotator);
		}
	}

}

void ABaseEnemy::CyclicTimer()
{
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	FVector Start = GetActorLocation() + FVector(0,0,100);
	FVector End = Start - FVector(0,GridManager->GridSize,0);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, TraceParams);
	if (bHit)
	{		
		ABaseTower* Tower = Cast<ABaseTower>(Hit.GetActor());
		if (Tower)
		{
			IsAttack = true;
			Tower->DamagedHealth(20);
		}
		else
		{
			IsAttack = false;
		}
	}
	else
	{
		IsAttack = false;
	}
}

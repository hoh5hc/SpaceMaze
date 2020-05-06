// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FlyPawn.generated.h"

UCLASS()
class MAZE_API AFlyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFlyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

public:

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* PlaneMesh;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Particles)
		class UParticleSystemComponent* ParticleImpact;

protected:
	/** Bound to the thrust axis */
	void ThrustInput(float Val);

	/** Bound to the vertical axis */
	void MoveUpInput(float Val);

	/** Bound to the horizontal axis */
	void MoveRightInput(float Val);

	void RotatetRightInput(float Val);

	void WheelZoom(float Val);

	void LeaveSignal();

	float armlengthspeed=100;

public:

	/** How quickly forward speed changes */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
		float Acceleration;

	/** How quickly forward speed changes */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
		float BrakeAcceleration;

	/** How quickly pawn can steer */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
		float TurnSpeed;

	/** Max forward speed */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
		float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
		float MinSpeed;
	/** Rotate speed */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
		float RotateSpeed;

	/** Current forward speed */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
	float CurrentForwardSpeed;

	/** Current yaw speed */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
	float CurrentYawSpeed;

	/** Current pitch speed */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
	float CurrentPitchSpeed;

	/** Current roll speed */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
	float CurrentRollSpeed;

	/** Current roll speed */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
		float Health;

	/** Current roll speed */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
		bool IsHoldKey = false;

	/** Current roll speed */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
		float MaxHealth=100.f;

	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
		bool IsDead = false;

	UFUNCTION(BlueprintCallable, Category = "C++ Event")
		void ReduceHealth(float amount);

	UFUNCTION(BlueprintImplementableEvent, Category = "C++ Event")
		void OnActorDeath();

	///**  */
	//UPROPERTY(Category = Inventory, EditAnywhere, BlueprintReadWrite)
	//	TArray<int> Inventory;

	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite)
		int BetRatio;
	UPROPERTY(Category = Spawn, EditAnywhere, BlueprintReadWrite)
		int SignalSpawn = 3;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
		TSubclassOf<class AActor> Signal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
	AFlyPawn* signalspawnedpawn;


};

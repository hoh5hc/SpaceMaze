// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MazePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MAZE_API AMazePlayerController : public APlayerController
{
	GENERATED_BODY()

	AMazePlayerController();

protected:
	// Override BeginPlay()
	virtual void BeginPlay() override;
public:

	
};

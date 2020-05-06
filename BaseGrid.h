// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"SquareCell.h"
#include "BaseGrid.generated.h"


USTRUCT(BlueprintType)
struct FBaseGrid //Grid data for base maze
{
	GENERATED_BODY()

	/*number of Column of the grid*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Col;
	/*number of Row of the grid*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Row;

	/*hash array contain all cell of maze grid*/
	TSet<FSquareCell> GridCell;

	/*default constructor*/
	FBaseGrid()
	{

	}

	/*return size of grid col*row */
	FORCEINLINE	int GetGridSize()
	{
		return Col * Row;
	}

	/*init the grid array with col and row*/
	bool InitGrid(int col,int row);

	/*init neighbor of the current grid cell*/
	bool InitCellNeighbor();

	/*return random cell from the grid*/
	FORCEINLINE FSquareCell* GetRandomCell()
	{
		return GridCell.Find(FSquareCell(FMath::RandRange(0, Col-1), FMath::RandRange(0, Row-1)));
	}

	/*Calculate position of grid wall*/
	TArray<FTransform> InitGridBaseDisplay(uint8 cellsize, uint8 unitsize);
	bool CalculateWallPosition(uint8 cellsize, TArray<FVector> &gridbase, TArray<FVector> &gridvertical, TArray<FVector> &gridhorizon);
	/***************************Maze algorithm part********************************/

	/*create very complex and long line maze, little dead end*/
	void AlgoRecursiveBacktrack();

	/*simple maze with bias in north and east corner*/
	void AlgoBinaryTree();

	/*similar to binary tree but eliminate bias in the east*/
	void AlgoSideWinder();

	/*no bias but may take long time to generate with large maze, lot of dead end and twist*/
	void AlgoAldousBroder();

	/*similar to aldous broder*/
	void AlgoWilson();

};

/*contain direction from a cell : North South East West*/
const TArray<FSquareCell> NeighborDirection = {FSquareCell(0,1),FSquareCell(0,-1),FSquareCell(1,0),FSquareCell(-1,0)};
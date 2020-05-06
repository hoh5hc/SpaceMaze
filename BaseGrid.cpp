// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseGrid.h"


bool FBaseGrid::InitGrid(int col, int row)
{
	Col = col; //assign col
	Row = row; //assign row
	for (int y = 0; y < row; y++) //loop row Y
	{
		for (int x=0; x < col; x++) //loop col X
		{
			GridCell.Emplace(FSquareCell(x, y));

		}
	}
	return false; //will be implememnt to return if not success
}


bool FBaseGrid::InitCellNeighbor()
{
	for (auto& cell : GridCell) //loop all cell
	{
		for (auto& direction : NeighborDirection) //loop all 4 direction
		{
			FSquareCell* neighborcell= GridCell.Find(cell + direction); // check if neighbor exist in grid
			if(neighborcell)
				cell.NeighborCell.Emplace(neighborcell);
		}

	}
	return false; //will be implememnt to return if not success
}

TArray<FTransform> FBaseGrid::InitGridBaseDisplay(uint8 cellsize,uint8 unitsize)
{
	TArray<FTransform> returngridbase;
	float basesize = cellsize*unitsize;
	for (int baserow = 0; baserow < (Row+1); baserow++)
	{
		for (int basecol = 0; basecol < (Col + 1); basecol++)
		{
			returngridbase.Emplace(FTransform(FRotator(0, 0, 0), FVector(basecol*basesize, baserow*basesize, 0), FVector(0.1, 0.1, 0.1)));
			if (baserow < Row) //vertical wall
			{
				FSquareCell *currentcell = GridCell.Find(FSquareCell(basecol-1, baserow));
				FSquareCell *nextcell = GridCell.Find(FSquareCell(basecol, baserow));
				if (currentcell)
				{
					if(!currentcell->IsLinkedToCell(nextcell))
						returngridbase.Emplace(FTransform(FRotator(0, 0, 0), FVector(basecol*basesize, (baserow + 0.5)*basesize, 0), FVector(0.1, 0.1*(cellsize - 1), 0.1)));
				}
				else
				{
					returngridbase.Emplace(FTransform(FRotator(0, 0, 0), FVector(basecol*basesize, (baserow + 0.5)*basesize, 0), FVector(0.1, 0.1*(cellsize - 1), 0.1)));
				}
			}
			
			if (basecol < Col) //horizon wall
			{
				FSquareCell *currentcell = GridCell.Find(FSquareCell(basecol , baserow-1));
				FSquareCell *nextcell = GridCell.Find(FSquareCell(basecol, baserow));
				if (currentcell)
				{
					if (!currentcell->IsLinkedToCell(nextcell))
						returngridbase.Emplace(FTransform(FRotator(0, 0, 0), FVector((basecol + 0.5)*basesize, baserow*basesize, 0), FVector(0.1*(cellsize - 1), 0.1, 0.1)));
				}
				else
					returngridbase.Emplace(FTransform(FRotator(0, 0, 0), FVector((basecol + 0.5)*basesize, baserow*basesize, 0), FVector(0.1*(cellsize - 1), 0.1, 0.1)));
			}
		}		
	}
	return returngridbase;
}

bool FBaseGrid::CalculateWallPosition(uint8 cellsize, TArray<FVector> &gridbase, TArray<FVector> &gridvertical, TArray<FVector> &gridhorizon)
{
	bool success = false;
	float basesize = cellsize;
	for (int baserow = 0; baserow < (Row + 1); baserow++)
	{
		for (int basecol = 0; basecol < (Col + 1); basecol++)
		{
			gridbase.Emplace(FVector(basecol*basesize, baserow*basesize, 0));
			if (baserow < Row) //vertical wall
			{
				FSquareCell *currentcell = GridCell.Find(FSquareCell(basecol - 1, baserow));
				FSquareCell *nextcell = GridCell.Find(FSquareCell(basecol, baserow));
				if (currentcell)
				{
					if (!currentcell->IsLinkedToCell(nextcell))
						gridvertical.Emplace(FVector(basecol*basesize, (baserow + 0.5)*basesize, 0));
				}
				else
				{
					gridvertical.Emplace(FVector(basecol*basesize, (baserow + 0.5)*basesize, 0));
				}
			}

			if (basecol < Col) //horizon wall
			{
				FSquareCell *currentcell = GridCell.Find(FSquareCell(basecol, baserow - 1));
				FSquareCell *nextcell = GridCell.Find(FSquareCell(basecol, baserow));
				if (currentcell)
				{
					if (!currentcell->IsLinkedToCell(nextcell))
						gridhorizon.Emplace(FVector((basecol + 0.5)*basesize, baserow*basesize, 0));
				}
				else
					gridhorizon.Emplace(FVector((basecol + 0.5)*basesize, baserow*basesize, 0));
			}
		}
	}
	return success;
}


void FBaseGrid::AlgoRecursiveBacktrack()
{
	TArray<FSquareCell*> stack; // stack to push and pop visited cell
	stack.Emplace(GetRandomCell()); // random start cell
	while (stack.Num()>0) // repeat until stack is empty
	{
		FSquareCell* current = stack.Last(); //get last recent cell
		FSquareCell* unlinkneighbor = current->GetRandUnLinkNeighborCell(); // get unlink neighbor of current cell
		if (unlinkneighbor)
		{
			current->LinkCell(unlinkneighbor); //link if there is unlink neighbor
			stack.Push(unlinkneighbor); // push this neighbor to stack
		}
		else
		{
			stack.Pop(); // remove the current cell if no unlink neighbor found
		}
	}
}

void FBaseGrid::AlgoBinaryTree()
{
	for (auto &cell : GridCell) // loop all the cell in grid
	{
		TArray<FSquareCell*> NorthEastNeighbors; // array contain north and east neighbor
		for (auto &neighborcell : cell.NeighborCell)  //loop over all neighbor
		{
			if (*neighborcell == cell + NeighborDirection[0]) // check if there is north neighbor
			{
				NorthEastNeighbors.Emplace(neighborcell);
			}
			if (*neighborcell == cell + NeighborDirection[2]) // check if there is east neighbor
			{
				NorthEastNeighbors.Emplace(neighborcell);
			}
		}
		if (NorthEastNeighbors.Num() > 0) // any N/E neighbor?
		{
			FSquareCell* randneighbor = NorthEastNeighbors[FMath::RandRange(0,NorthEastNeighbors.Num() - 1)]; // get random N/E neighbor and link to current cell
			if (randneighbor)
				cell.LinkCell(randneighbor);
		}
	}
}

void FBaseGrid::AlgoSideWinder()
{
	TArray<FSquareCell*> rowstack;
	for (auto &cell : GridCell) //loop over all cell in grid
	{
		TArray<FSquareCell*> NorthEastNeighbors;
		for (auto &neighborcell : cell.NeighborCell) //choose out N/E like binary tree
		{
			if (*neighborcell == cell + NeighborDirection[0])
			{
				NorthEastNeighbors.Emplace(neighborcell);
			}
			if (*neighborcell == cell + NeighborDirection[2])
			{
				NorthEastNeighbors.Emplace(neighborcell);
			}
		}
		if (NorthEastNeighbors.Num() > 0)
		{
			FSquareCell* randneighbor = NorthEastNeighbors[FMath::RandRange(0, NorthEastNeighbors.Num() - 1)]; //random N/E neighbor
			rowstack.Emplace(&cell); 
			if (*randneighbor== cell + NeighborDirection[2]) // if this neibor is east then link it
			{
				cell.LinkCell(randneighbor);
				
			}
			else // if not then choose random north from the row stack
			{
				FSquareCell* randstack= rowstack[FMath::RandRange(0, rowstack.Num() - 1)];
				FSquareCell* randstacknorth = randstack->NeighborCell[0];
				randstack->LinkCell(randstacknorth);
				rowstack.Empty();
			}
				
		}
	}
}


void FBaseGrid::AlgoAldousBroder()
{
	FSquareCell* cell = GetRandomCell(); // get random cell
	int unvisited = GetGridSize() - 1;
	while (unvisited > 0) // loop untill all cell is visited
	{
		FSquareCell* neighbor = cell->GetRandomNeighborCell();
		if (neighbor->Link.Num() <= 0) // get random neighbor then link it if it's unlink
		{
			cell->LinkCell(neighbor);
			unvisited--;
		}
		cell = neighbor; //assign current neighbor for nex loop
	}
}


void FBaseGrid::AlgoWilson()
{
	TArray<FSquareCell*> unvisitedcell;
	for (auto &i : GridCell) // assign unvisited cell
	{
		unvisitedcell.Emplace(&i);
	}
	unvisitedcell.Remove(GetRandomCell()); //start with visit one cell
	while (unvisitedcell.Num() > 0) // repeat till all cell visited
	{
		TArray<FSquareCell*> wanderpath;
		FSquareCell* wandercell = unvisitedcell[FMath::RandRange(0,unvisitedcell.Num()-1)];
		wanderpath.Emplace(wandercell);
		while (unvisitedcell.Find(wandercell)>=0) // choose random cell and go until got the visited cell
		{
			wandercell=wandercell->GetRandomNeighborCell();
			int position= wanderpath.Find(wandercell);
			if (position >= 0) // the path meet itself ,so remove the loop 
			{
				wanderpath.RemoveAt<int>(position + 1, wanderpath.Num() - (position + 1));
			}
			else
			{
				wanderpath.Emplace(wandercell);
			}
		}

		for (int cell=0;cell < wanderpath.Num()-1;cell++) // assign all the cell in path to visited
		{
			wanderpath[cell]->LinkCell(wanderpath[cell + 1]);
			unvisitedcell.Remove(wanderpath[cell]);
		}
	}

}


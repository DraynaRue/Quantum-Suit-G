// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "QuantumSuitGGameMode.h"
#include "QuantumSuitGPawn.h"

AQuantumSuitGGameMode::AQuantumSuitGGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AQuantumSuitGPawn::StaticClass();
}

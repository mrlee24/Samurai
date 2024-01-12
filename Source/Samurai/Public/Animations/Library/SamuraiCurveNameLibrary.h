// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SamuraiCurveNameLibrary.generated.h"

// Curves.
static const FName NAME_W_Gait(TEXT("Gait"));
static const FName NAME_TurnYawWeight(TEXT("TurnYawWeight"));
static const FName NAME_RemainingTurnYaw(TEXT("RemainingTurnYaw"));
static const FName NAME_GroundDistance(TEXT("GroundDistance"));

// IK.
static const FName NAME_DisableLegIK(TEXT("DisableLegIK"));

/**
 * 
 */
UCLASS()
class SAMURAI_API USamuraiCurveNameLibrary : public UObject
{
	GENERATED_BODY()
};

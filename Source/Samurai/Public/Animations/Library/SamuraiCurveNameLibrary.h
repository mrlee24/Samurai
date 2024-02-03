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
static const FName NAME_FeetPosition(TEXT("Feet_Position"));

// IK.
static const FName NAME_Enable_FootIK_R(TEXT("Enable_FootIK_R"));
static const FName NAME_Enable_FootIK_L(TEXT("Enable_FootIK_L"));
static const FName NAME_Enable_HandIK_L(TEXT("Enable_HandIK_L"));
static const FName NAME_Enable_HandIK_R(TEXT("Enable_HandIK_R"));
static const FName NAME_FootLock_L(TEXT("FootLock_L"));
static const FName NAME_FootLock_R(TEXT("FootLock_R"));

// Bones.
static const FName NAME_IK_Foot_R(TEXT("ik_foot_r"));
static const FName NAME_IK_Foot_L(TEXT("ik_foot_l"));
static const FName NAME_RootBone(TEXT("root"));

/**
 * 
 */
UCLASS()
class SAMURAI_API USamuraiCurveNameLibrary : public UObject
{
	GENERATED_BODY()
};

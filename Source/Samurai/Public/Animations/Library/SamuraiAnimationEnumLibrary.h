// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SamuraiAnimationEnumLibrary.generated.h"

UENUM(BlueprintType)
enum class ESamuraiCardinalDirection : uint8
{
	EForward,
	EBackward,
	ELeft,
	ERight
};

UENUM(BlueprintType)
enum class ESamuraiMovementDirection : uint8
{
	EForward,
	EBackward,
};

UENUM(BlueprintType)
enum class ESamuraiRootYawOffsetMode : uint8
{
	EBlendOut,
	EHold,
	EAccumulate
};

UENUM(BlueprintType)
enum class ESamuraiStance : uint8
{
	ENone,
	EStanding,
	ECrouching,
};

UENUM(BlueprintType)
enum class ESamuraiGait : uint8
{
	EWalking,
	ERunning,
	ESprinting
};

UENUM(BlueprintType)
enum class ESamuraiCardinalMode : uint8
{
	EStart,
	EStop,
	ELoop,
	EPivot,
};

UENUM(BlueprintType)
enum class ESamuraiBlendMaskMode : uint8
{
	ENone,
	ELeftHand,
	ELeftArm,
	ERightHand,
	ERightArm,
	EUpperBody,
	ELowerBody
};


UENUM(BlueprintType)
enum class ESamuraiBlendMaskAdditiveMode : uint8
{
	EMeshSpace,
	ELocalSpace,
};

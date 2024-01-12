// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SamuraiLocomotionEnumLibrary.generated.h"

UENUM(BlueprintType)
enum class ESamuraiRotationMode : uint8
{
	EVelocityDirection,
	ELookingDirection,
	EControlDirection,
};
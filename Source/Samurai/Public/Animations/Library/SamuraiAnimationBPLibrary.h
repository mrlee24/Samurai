// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SamuraiAnimationEnumLibrary.h"
#include "SamuraiAnimationBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SAMURAI_API USamuraiAnimationBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public: // Directions.
	
	UFUNCTION(BlueprintPure, meta = (BluerpintThreadSafe), Category = "SamuraiAnimationBPLibrary")
	static ESamuraiCardinalDirection SelectCardinalDirectionFromAngle(const float angle, const float deadZone,
																	  const ESamuraiCardinalDirection currentDirection =
																		  ESamuraiCardinalDirection::EForward,
																	  const bool shouldUseCurrentDirection = false);

	UFUNCTION(BlueprintPure, meta = (BluerpintThreadSafe), Category = "SamuraiAnimationBPLibrary")
	static ESamuraiCardinalDirection GetOppositeCardinalDirection(
		const ESamuraiCardinalDirection currentDirection = ESamuraiCardinalDirection::EForward);
	
	UFUNCTION(BlueprintPure, meta = (BluerpintThreadSafe), Category = "SamuraiAnimationBPLibrary")
	static ESamuraiMovementDirection GetOppositeMovementDirection(const ESamuraiMovementDirection currentDirection = ESamuraiMovementDirection::EBackward);
	
	UFUNCTION(BlueprintPure, meta = (BluerpintThreadSafe), Category = "SamuraiAnimationBPLibrary")
	static bool IsMovingForwardOrBackward(const ESamuraiCardinalDirection currentDirection);

public: // Convertors.
	
	UFUNCTION(BlueprintPure, meta = (BluerpintThreadSafe), Category = "SamuraiAnimationBPLibrary")
	static FGameplayTag ConvertEnumGaitToGameplayTag(const ESamuraiGait gait);

	UFUNCTION(BlueprintPure, meta = (BluerpintThreadSafe), Category = "SamuraiAnimationBPLibrary")
	static ESamuraiGait ConvertGaitGameplayTagToEnumGait(const FGameplayTag& gait);

public:
	
	/**
	 * @returns true if the value is within the given range
	 */
	UFUNCTION(BlueprintPure, meta = (BluerpintThreadSafe), Category = "SamuraiAnimationBPLibrary")
	static bool InRange(float value, float min, float max);
};

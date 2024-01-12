// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SamuraiLocomotionStructLibrary.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSamuraiLocomotionState
{
	GENERATED_BODY()

public:

	FSamuraiLocomotionState()
	{
	};

	FSamuraiLocomotionState(FGameplayTag state, float maxSpeed) :
		State(state),
		MaxSpeed(maxSpeed)
	{
	};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag State = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxSpeed = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MovementInputAmount = 0.f;

	FORCEINLINE bool operator <(const FSamuraiLocomotionState& other) const
	{
		return MaxSpeed < other.MaxSpeed;
	}

	FORCEINLINE bool operator ==(const FSamuraiLocomotionState& other) const
	{
		return State == other.State;
	}

	FORCEINLINE bool operator !=(const FGameplayTag& other) const
	{
		return State != other;
	}

	FORCEINLINE bool operator ==(const FGameplayTag& other) const
	{
		return State == other;
	}

	FORCEINLINE bool operator !=(const FSamuraiLocomotionState& other) const
	{
		return State != other.State;
	}
};
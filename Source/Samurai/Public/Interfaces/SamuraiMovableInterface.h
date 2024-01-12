// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SamuraiMovableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class USamuraiMovableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SAMURAI_API ISamuraiMovableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Returns Base Movement Component.
	virtual class UPawnMovementComponent* GetMovementComponent() const = 0;

	// Returns Locomotion Component.
	virtual class USamuraiLocomotionComponent* GetLocomotionComponent() const = 0;

	virtual FVector GetCurrentAcceleration() const = 0;

	virtual float GetMaxAcceleration() const = 0;
};

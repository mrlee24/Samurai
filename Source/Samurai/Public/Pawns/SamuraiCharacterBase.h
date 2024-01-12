// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/SamuraiMovableInterface.h"
#include "SamuraiCharacterBase.generated.h"

UCLASS()
class SAMURAI_API ASamuraiCharacterBase : public ACharacter,
										  public ISamuraiMovableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASamuraiCharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public: // ~ ISamuraiMovableInterface ~

	virtual class UPawnMovementComponent* GetMovementComponent() const override;
	virtual class USamuraiLocomotionComponent* GetLocomotionComponent() const override;
	virtual FVector GetCurrentAcceleration() const override;
	virtual float GetMaxAcceleration() const override;

protected:

	// This value will be used to define whether we should add AxisValueOffset
	// Whenever the current axis value is too small (FMath::Abs(axisValue) < MaxAxisValueOffset)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Axis Value")
	float MaxAxisValueOffset = .1f;

	// This value will be added to the current axisValue.
	// Whenever (FMath::Abs(axisValue) < MaxAxisValueOffset) is qualified.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Axis Value")
	float AxisValueOffset = .2f;

	UFUNCTION(BlueprintCallable, Category = "Axis Value")
	void CalculateMovementInput(const float axisValue, float& outAxisValue) const;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USamuraiLocomotionComponent> LocomotionComponent = nullptr;
};

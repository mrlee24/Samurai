// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/SamuraiCharacterBase.h"

#include "Animations/Components/SamuraiLocomotionComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ASamuraiCharacterBase::ASamuraiCharacterBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = 0;
	bReplicates = true;
	SetReplicatingMovement(true);
	
	LocomotionComponent = CreateDefaultSubobject<USamuraiLocomotionComponent>(TEXT("Locomotion Component"));
}

// Called when the game starts or when spawned
void ASamuraiCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASamuraiCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASamuraiCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UPawnMovementComponent* ASamuraiCharacterBase::GetMovementComponent() const
{
	return Cast<UPawnMovementComponent>(GetCharacterMovement());
}

USamuraiLocomotionComponent* ASamuraiCharacterBase::GetLocomotionComponent() const
{
	return LocomotionComponent;
}

FVector ASamuraiCharacterBase::GetCurrentAcceleration() const
{
	const TObjectPtr<UCharacterMovementComponent> characterMovementComponent = GetCharacterMovement();
	if (!characterMovementComponent)
	{
		return FVector::ZeroVector;
	}
	
	return characterMovementComponent->GetCurrentAcceleration();
}

float ASamuraiCharacterBase::GetMaxAcceleration() const
{
	const TObjectPtr<UCharacterMovementComponent> characterMovementComponent = GetCharacterMovement();
	if (!characterMovementComponent)
	{
		return 0.f;
	}
	
	return characterMovementComponent->GetMaxAcceleration();
}

void ASamuraiCharacterBase::CalculateMovementInput(const float axisValue, float& outAxisValue) const
{
	const bool isMovingOnGround = GetMovementComponent()->IsMovingOnGround();
	const bool hasVelocity = !FMath::IsNearlyZero(GetVelocity().SizeSquared2D());
	if (isMovingOnGround && hasVelocity)
	{
		const bool isAxisZero = (FMath::IsNearlyZero(axisValue));
		const bool canBufferAxis = (!isAxisZero && FMath::Abs(axisValue) < MaxAxisValueOffset);
		if (canBufferAxis)
		{
			const bool isAxisPositive = (axisValue > 0.f);
			const float bufferAxis = (isAxisPositive) ? AxisValueOffset : -AxisValueOffset;
			outAxisValue = axisValue + bufferAxis;
			return;
		}
	}

	outAxisValue = axisValue;
}


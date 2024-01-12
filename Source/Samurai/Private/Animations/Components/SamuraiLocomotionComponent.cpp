// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Components/SamuraiLocomotionComponent.h"

#include "SamuraiGameplayTags.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interfaces/SamuraiMovableInterface.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogSamuraiLocomotionComponent);

// Sets default values for this component's properties
USamuraiLocomotionComponent::USamuraiLocomotionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	// Init
	LocomotionStateTag = GTag_Movement_State_Walk;
	DesiredLocomotionTag = GTag_Movement_State_Run;

	// Cache
	LocomotionWalkStateTag = GTag_Movement_State_Walk;
	LocomotionRunStateTag = GTag_Movement_State_Run;
	LocomotionSprintStateTag = GTag_Movement_State_Sprint;
}

// Called when the game starts
void USamuraiLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwningPawn = Cast<APawn>(GetOwner());
	if (!OwningPawn)
	{
		UE_LOG(LogSamuraiLocomotionComponent, Error, TEXT("OwningPawn is invalid"));
		return;
	}

	MovableInterface.SetObject(OwningPawn);
	MovableInterface.SetInterface(Cast<ISamuraiMovableInterface>(OwningPawn));

	PawnMovementComponent = OwningPawn->GetMovementComponent();

	if (!LocomotionStates.IsEmpty())
	{
		AnimatedWalkSpeed = LocomotionStates.FindChecked(LocomotionWalkStateTag).MaxSpeed;
		AnimatedRunSpeed = LocomotionStates.FindChecked(LocomotionRunStateTag).MaxSpeed;
		AnimatedSprintSpeed = LocomotionStates.FindChecked(LocomotionSprintStateTag).MaxSpeed;
	}

	// Set default rotation values
	TargetRotation = OwningPawn->GetActorRotation();
	LastVelocityRotation = TargetRotation;

	ForceUpdateLocomotion();
}

// Called every frame
void USamuraiLocomotionComponent::TickComponent(float deltaTime, ELevelTick tickType,
                                                FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	// ...
	UpdateEssentialValues(deltaTime);
	UpdateLocomotion();
	UpdateGroundedRotation(deltaTime);
}

void USamuraiLocomotionComponent::PostInitProperties()
{
	Super::PostInitProperties();

	LocomotionStates.Add(LocomotionWalkStateTag, FSamuraiLocomotionState(LocomotionWalkStateTag, 150.f));
	LocomotionStates.Add(LocomotionRunStateTag, FSamuraiLocomotionState(LocomotionRunStateTag, 350.f));
	LocomotionStates.Add(LocomotionSprintStateTag, FSamuraiLocomotionState(LocomotionSprintStateTag, 735.f));
}

void USamuraiLocomotionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated properties.
	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedCurrentAcceleration, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedControlRotation, COND_SkipOwner);
	
	// Desired properties.
	DOREPLIFETIME(ThisClass, DesiredLocomotionTag);
	DOREPLIFETIME_CONDITION(ThisClass, DesiredStance, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, DesiredRotationMode, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(ThisClass, RotationMode, COND_SkipOwner);
}

FGameplayTag USamuraiLocomotionComponent::GetAllowedLocomotionStateTag() const
{
	if (!IsDefaultLocomotionState(DesiredLocomotionTag))
	{
		return DesiredLocomotionTag;
	}

	return GetActualLocomotionStateTag(LocomotionRunStateTag);
}

FGameplayTag USamuraiLocomotionComponent::GetActualLocomotionStateTag(
	const FGameplayTag& allowedLocomotionStateTag) const
{
	if (!IsDefaultLocomotionState(allowedLocomotionStateTag))
	{
		return allowedLocomotionStateTag;
	}

	const float walkSpeed = LocomotionStates.Find(LocomotionWalkStateTag)->MaxSpeed;
	const float runSpeed = LocomotionStates.Find(LocomotionRunStateTag)->MaxSpeed;
	if (Speed2D > runSpeed + 10.f)
	{
		if (LocomotionStateTag.MatchesTag(LocomotionSprintStateTag))
		{
			return LocomotionSprintStateTag;
		}

		return LocomotionRunStateTag;
	}

	if (Speed2D >= walkSpeed + 10.f)
	{
		return LocomotionRunStateTag;
	}

	return LocomotionWalkStateTag;
}

void USamuraiLocomotionComponent::UpdateEssentialValues(float deltaTime)
{
	// Don't call Super because it does not have any implementations
	
	if (!OwningPawn)
	{
		UE_LOG(LogSamuraiLocomotionComponent, Error, TEXT("OwningPawn is invalid"));
		return;
	}

	if (!MovableInterface)
	{
		UE_LOG(LogSamuraiLocomotionComponent, Error, TEXT("MovableInterface is invalid"));
		return;
	}
	
	const bool isSimulated = OwningPawn->GetLocalRole() == ROLE_SimulatedProxy;
	if (!isSimulated)
	{
		// Updating acceleration data.
		ReplicatedCurrentAcceleration = MovableInterface->GetCurrentAcceleration();
		EasedMaxAcceleration = MovableInterface->GetMaxAcceleration();

		// Updating control rotation data. 
		ReplicatedControlRotation = OwningPawn->GetControlRotation();
	}
	else
	{
		const float maxAcceleration = MovableInterface->GetMaxAcceleration();
		EasedMaxAcceleration = (!FMath::IsNearlyZero(maxAcceleration) ? maxAcceleration : EasedMaxAcceleration / 2);
	}

	// Interp ControlRotation to current control rotation for smooth character rotation movement. Decrease InterpSpeed
	// for slower but smoother movement.
	ControlRotation = FMath::RInterpTo(ControlRotation, ReplicatedControlRotation, deltaTime, 30.f);
	
	const FVector& velocity = OwningPawn->GetVelocity();
	Speed2D = velocity.Size2D();
	const bool isMoving = !FMath::IsNearlyZero(Speed2D);
	if (isMoving)
	{
		LastVelocityRotation = velocity.ToOrientationRotator();
	}
}

void USamuraiLocomotionComponent::UpdateLocomotion()
{
	if (!PawnMovementComponent)
	{
		UE_LOG(LogSamuraiLocomotionComponent, Error, TEXT("PawnMovementComponent is invalid"));
		return;
	}

	// Set the Allowed Locomotion State Name
	const FGameplayTag& allowedLocomotionStateTag = GetAllowedLocomotionStateTag();

	// Determine the Actual Locomotion State. If it is different from the current Locomotion State, Set the new Locomotion State Event.
	const FGameplayTag& actualLocomotionStateTag = GetActualLocomotionStateTag(allowedLocomotionStateTag);
	if (!actualLocomotionStateTag.MatchesTagExact(LocomotionStateTag))
	{
		SetLocomotionStateTag(actualLocomotionStateTag);
	}

	// Update the Character Max Walk Speed to the configured speeds based on the currently Allowed Locomotion State.
	BroadcastAllowedLocomotionStateChanged(*LocomotionStates.Find(allowedLocomotionStateTag));
}

void USamuraiLocomotionComponent::UpdateGroundedRotation(float deltaTime)
{
	const bool canUpdateMovingRotation = !FMath::IsNearlyZero(Speed2D);
	if (canUpdateMovingRotation)
	{
		switch (RotationMode)
		{
		case ESamuraiRotationMode::EVelocityDirection:
			{
				SmoothCharacterRotation({0.f, LastVelocityRotation.Yaw, 0.f}, 800.f, 5.f, deltaTime);
				break;
			}
		case ESamuraiRotationMode::ELookingDirection:
			{
				const float yawValue = (IsSprinting() ? LastVelocityRotation.Yaw : ControlRotation.Yaw);
				SmoothCharacterRotation({0.f, yawValue, 0.f}, 500.f, 5.f, deltaTime);
				break;
			}
		case ESamuraiRotationMode::EControlDirection:
			{
				SmoothCharacterRotation({0.0f, ControlRotation.Yaw, 0.0f}, 1000.0f, 20.0f, deltaTime);
				break;
			}
		}
	}
}

void USamuraiLocomotionComponent::ForceUpdateLocomotion()
{
	SetLocomotionStateTag(DesiredLocomotionTag, true);
	SetStance(DesiredStance, true);
	SetRotationMode(DesiredRotationMode, true);
}

float USamuraiLocomotionComponent::GetLocomotionStateSpeed(const FGameplayTag& stateTag) const
{
	if (!stateTag.IsValid())
	{
		UE_LOG(LogSamuraiLocomotionComponent, Error, TEXT("State Tag is invalid"));
		return 1.f;
	}

	return LocomotionStates.Find(stateTag)->MaxSpeed;
}

bool USamuraiLocomotionComponent::IsDefaultLocomotionState(const FGameplayTag& stateTag) const
{
	return stateTag.MatchesTag(LocomotionWalkStateTag) || stateTag.MatchesTag(LocomotionRunStateTag) || stateTag.
		MatchesTag(LocomotionSprintStateTag);
}

bool USamuraiLocomotionComponent::IsSprinting() const
{
	return LocomotionStateTag.MatchesTagExact(LocomotionSprintStateTag);
}

void USamuraiLocomotionComponent::OnRep_RotationMode(ESamuraiRotationMode rotationMode) const
{
	BroadcastCurrentRotationMode(rotationMode);
}

void USamuraiLocomotionComponent::SetLocomotionStateTag(const FGameplayTag& newLocomotionStateTag,
                                                          const bool shouldForce)
{
	if (shouldForce || !LocomotionStateTag.MatchesTagExact(newLocomotionStateTag))
	{
		// Cache our last state.
		const FGameplayTag& lastLocomotionTag = LocomotionStateTag;
		LocomotionStateTag = newLocomotionStateTag;
		BroadcastLocomotionStateChanged(lastLocomotionTag);
	}
}

void USamuraiLocomotionComponent::SetStance(ESamuraiStance newStance, const bool shouldForce)
{
	if (shouldForce || Stance != newStance)
	{
		// Cache our last stance.
		const ESamuraiStance lastStance = Stance;
		Stance = newStance;
		BroadcastCurrentStanceChanged(lastStance);
	}
}

void USamuraiLocomotionComponent::SetRotationMode(ESamuraiRotationMode newRotationMode, const bool shouldForce)
{
	if (!OwningPawn)
	{
		UE_LOG(LogSamuraiLocomotionComponent, Error, TEXT("OwningPawn is invalid"));
		return;
	}

	if (shouldForce || RotationMode != newRotationMode)
	{
		const ESamuraiRotationMode lastRotationMode = RotationMode;
		RotationMode = newRotationMode;

		BroadcastCurrentRotationMode(lastRotationMode);

		if (OwningPawn->GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetCurrentRotationMode(newRotationMode, shouldForce);
		}
	}
}

ESamuraiRotationMode USamuraiLocomotionComponent::GetRotationMode() const
{
	return RotationMode;
}

ESamuraiRotationMode USamuraiLocomotionComponent::GetDesiredRotationMode() const
{
	return DesiredRotationMode;
}

const FVector& USamuraiLocomotionComponent::GetReplicatedCurrentAcceleration() const
{
	return ReplicatedCurrentAcceleration;
}

void USamuraiLocomotionComponent::Server_SetCurrentRotationMode_Implementation(ESamuraiRotationMode newRotationMode,
                                                                               const bool shouldForce)
{
	SetRotationMode(newRotationMode, shouldForce);
}

bool USamuraiLocomotionComponent::Server_SetCurrentRotationMode_Validate(ESamuraiRotationMode newRotationMode,
                                                                         const bool shouldForce)
{
	return true;
}

void USamuraiLocomotionComponent::SetDesiredRotationMode(ESamuraiRotationMode newRotationMode)
{
	if (!OwningPawn)
	{
		UE_LOG(LogSamuraiLocomotionComponent, Error, TEXT("OwningPawn is invalid"));
		return;
	}

	DesiredRotationMode = newRotationMode;
	if (OwningPawn->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredRotationMode(newRotationMode);
	}
}

void USamuraiLocomotionComponent::SmoothCharacterRotation(const FRotator& target, const float targetInterpSpeed,
                                                          const float actorInterpSpeed, const float deltaTime)
{
	if (!OwningPawn)
	{
		UE_LOG(LogSamuraiLocomotionComponent, Error, TEXT("OwningPawn is invalid"));
		return;
	}

	TargetRotation = FMath::RInterpConstantTo(TargetRotation, target, deltaTime, targetInterpSpeed);
	OwningPawn->SetActorRotation(FMath::RInterpTo(OwningPawn->GetActorRotation(), TargetRotation, deltaTime,
	                                              actorInterpSpeed));
}

void USamuraiLocomotionComponent::Server_SetDesiredRotationMode_Implementation(ESamuraiRotationMode newRotationMode)
{
	SetDesiredRotationMode(newRotationMode);
}

bool USamuraiLocomotionComponent::Server_SetDesiredRotationMode_Validate(ESamuraiRotationMode newRotationMode)
{
	return true;
}

ESamuraiStance USamuraiLocomotionComponent::GetStance() const
{
	return Stance;
}

ESamuraiStance USamuraiLocomotionComponent::GetDesiredStance() const
{
	return DesiredStance;
}

void USamuraiLocomotionComponent::BroadcastAllowedLocomotionStateChanged(
	const FSamuraiLocomotionState& locomotionState) const
{
	if (OnAllowedLocomotionStateChanged.IsBound())
	{
		OnAllowedLocomotionStateChanged.Broadcast(locomotionState);
	}

	if (OnAllowedLocomotionStateChangedNative.IsBound())
	{
		OnAllowedLocomotionStateChangedNative.Broadcast(locomotionState);
	}
}

void USamuraiLocomotionComponent::BroadcastLocomotionStateChanged(const FGameplayTag& locomotionStateTag) const
{
	if (OnLocomotionTagChanged.IsBound())
	{
		OnLocomotionTagChanged.Broadcast(locomotionStateTag);
	}

	if (OnCurrentLocomotionTagChangedNative.IsBound())
	{
		OnCurrentLocomotionTagChangedNative.Broadcast(locomotionStateTag);
	}
}

void USamuraiLocomotionComponent::BroadcastCurrentStanceChanged(const ESamuraiStance stance) const
{
	if (OnStanceChanged.IsBound())
	{
		OnStanceChanged.Broadcast(stance);
	}

	if (OnStanceChangedNative.IsBound())
	{
		OnStanceChangedNative.Broadcast(stance);
	}
}

void USamuraiLocomotionComponent::BroadcastCurrentRotationMode(const ESamuraiRotationMode rotationMode) const
{
	if (OnRotationModeChanged.IsBound())
	{
		OnRotationModeChanged.Broadcast(rotationMode);
	}

	if (OnRotationModeChangedNative.IsBound())
	{
		OnRotationModeChangedNative.Broadcast(rotationMode);
	}
}

const FGameplayTag& USamuraiLocomotionComponent::GetLocomotionStateTag() const
{
	return LocomotionStateTag;
}

const FGameplayTag& USamuraiLocomotionComponent::GetDesiredLocomotionStateTag() const
{
	return DesiredLocomotionTag;
}

void USamuraiLocomotionComponent::SetDesiredLocomotionStateTag(const FGameplayTag& newLocomotionStateTag)
{
	if (!OwningPawn)
	{
		UE_LOG(LogSamuraiLocomotionComponent, Error, TEXT("OwningPawn is invalid"));
		return;
	}

	DesiredLocomotionTag = newLocomotionStateTag;
	if (OwningPawn->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredLocomotionStateTag(newLocomotionStateTag);
	}
}

void USamuraiLocomotionComponent::Server_SetDesiredLocomotionStateTag_Implementation(
	const FGameplayTag& newLocomotionStateTag)
{
	SetDesiredLocomotionStateTag(newLocomotionStateTag);
}

bool USamuraiLocomotionComponent::Server_SetDesiredLocomotionStateTag_Validate(
	const FGameplayTag& newLocomotionStateTag)
{
	return true;
}

void USamuraiLocomotionComponent::SetDesiredStance(ESamuraiStance newStance)
{
	if (!OwningPawn)
	{
		UE_LOG(LogSamuraiLocomotionComponent, Error, TEXT("OwningPawn is invalid"));
		return;
	}

	DesiredStance = newStance;
	if (OwningPawn->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredStance(newStance);
	}
}

void USamuraiLocomotionComponent::Server_SetDesiredStance_Implementation(ESamuraiStance newStance)
{
	SetDesiredStance(newStance);
}

bool USamuraiLocomotionComponent::Server_SetDesiredStance_Validate(ESamuraiStance newStance)
{
	return true;
}

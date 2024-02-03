// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimInstances/Pawns/SamuraiBaseAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "KismetAnimationLibrary.h"
#include "SamuraiGameplayTags.h"
#include "Animations/Components/SamuraiLocomotionComponent.h"
#include "Animations/Library/SamuraiAnimationBPLibrary.h"
#include "Animations/Library/SamuraiCurveNameLibrary.h"
#include "GameFramework/NavMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interfaces/SamuraiMovableInterface.h"

void USamuraiBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const TObjectPtr<APawn> owningPawn = TryGetPawnOwner())
	{
		OwningPawn = owningPawn;
		// Initialize GameplayTagInterface.
		{
			GameplayTagInterface.SetObject(owningPawn);
			GameplayTagInterface.SetInterface(Cast<IGameplayTagAssetInterface>(owningPawn));
		}

		// Initialize PawnMovementComponent.
		PawnMovementComponent = OwningPawn->GetMovementComponent();

		MovableInterface.SetObject(owningPawn);
		MovableInterface.SetInterface( Cast<ISamuraiMovableInterface>(owningPawn));
		
		// (TODO) Initialize LocomotionComponent.
		if (MovableInterface)
		{
			LocomotionComponent = MovableInterface->GetLocomotionComponent();
		}

		IdleAnimationExitNativeDelegate.AddUObject(this, &USamuraiBaseAnimInstance::HandleOnIdleAnimationExit);
		StopAnimationEnterNativeDelegate.AddUObject(this, &USamuraiBaseAnimInstance::HandleOnStopAnimationEnter);
	}
}

void USamuraiBaseAnimInstance::NativeUpdateAnimation(float deltaSeconds)
{
	// Don't call Super since it is empty.

	if (OwningPawn && MovableInterface)
	{
		// Pre-Update essential actor's properties.
		{
			PreUpdateData.bIsAutonomousProxy = OwningPawn->GetLocalRole() == ROLE_AutonomousProxy;
			PreUpdateData.ActorRotation = OwningPawn->GetActorRotation();
			PreUpdateData.ActorLocation = OwningPawn->GetActorLocation();
			PreUpdateData.ActorVelocity = OwningPawn->GetVelocity();
			PreUpdateData.ActorAcceleration = MovableInterface->GetCurrentAcceleration();
			PreUpdateData.AimPitch = OwningPawn->GetBaseAimRotation().Pitch;
		}

		// Pre-Update essential actor's movement properties.
		PreUpdateMovementData(deltaSeconds);

		// Pre-Update essential actor's ability properties.
		PreUpdateAbilityData();

		// Pre-Update animation data.
		PreUpdateAnimationData();

		// Pre-Verify if any montage is playing.
		PreUpdateData.bIsAnyMontagePlaying = IsAnyMontagePlaying();
	}
}

void USamuraiBaseAnimInstance::NativeThreadSafeUpdateAnimation(float deltaSeconds)
{
	// Don't call Super since it is empty.

	UpdateRotationData();
	UpdateLocationData(deltaSeconds);
	UpdateVelocityData(deltaSeconds);
	UpdateAccelerationData();
	UpdateWallDetectionHeuristic();
	UpdatePawnStateData(deltaSeconds);
	UpdateBlendWeightData(deltaSeconds);
	UpdateRootYawOffset(deltaSeconds);
	UpdateAimingData();
	UpdateJumpFallData();

	// Reset this after the first update.
	bIsFirstUpdate = false;
}

void USamuraiBaseAnimInstance::PreUpdateMovementData(float deltaSeconds)
{
	// Override this in subclass, if needed since base class cannot specify movement component.
	
	if (PawnMovementComponent)
	{
		PreUpdateData.GravityZ = PawnMovementComponent->GetGravityZ();
		PreUpdateData.bIsCrouching = PawnMovementComponent->IsCrouching();
		PreUpdateData.bIsFalling = PawnMovementComponent->IsFalling();
		PreUpdateData.bIsMovingOnGround = PawnMovementComponent->IsMovingOnGround();
		PreUpdateData.bIsSwimming = PawnMovementComponent->IsSwimming();
		PreUpdateData.bIsFlying = PawnMovementComponent->IsFlying();
	}
}

void USamuraiBaseAnimInstance::PreUpdateAbilityData()
{
	if (GameplayTagInterface)
	{
		const bool isAiming = GameplayTagInterface->HasMatchingGameplayTag(GTag_Movement_State_Aim);
		const bool isUsingItem = GameplayTagInterface->HasMatchingGameplayTag(GTag_Movement_State_UseItem);
		const bool isDodging = GameplayTagInterface->HasMatchingGameplayTag(GTag_Movement_State_Dodge);
		const bool isJumping = GameplayTagInterface->HasMatchingGameplayTag(GTag_Movement_State_Jump);

		PreUpdateData.AbilityData.bIsAiming = isAiming;
		PreUpdateData.AbilityData.bIsUsingItem = isUsingItem;
		PreUpdateData.AbilityData.bIsDodging = isDodging;
		PreUpdateData.AbilityData.bIsJumping = isJumping;
	}
}

void USamuraiBaseAnimInstance::PreUpdateAnimationData()
{
	if (LocomotionComponent)
	{
		PreUpdateData.AnimationData.LocomotionStateTag = LocomotionComponent->GetLocomotionStateTag();
		PreUpdateData.AnimationData.RotationMode = LocomotionComponent->GetRotationMode();
	}
}

void USamuraiBaseAnimInstance::UpdateRotationData()
{
	RotationData.YawDeltaSinceLastUpdate = PreUpdateData.ActorRotation.Yaw - RotationData.WorldRotation.Yaw;
	RotationData.YawDeltaSpeed =
		UKismetMathLibrary::SafeDivide(RotationData.YawDeltaSinceLastUpdate, GetDeltaSeconds());
	RotationData.WorldRotation = PreUpdateData.ActorRotation;

	const float additiveLean = (PawnStateData.bIsCrouching || PawnStateData.bIsAiming)
		                           ? 0.025f
		                           : 0.0375f;
	RotationData.AdditiveLeanAngle = RotationData.YawDeltaSpeed * additiveLean;
	if (bIsFirstUpdate)
	{
		RotationData.YawDeltaSinceLastUpdate = 0.f;
		RotationData.AdditiveLeanAngle = 0.f;
	}
}

void USamuraiBaseAnimInstance::UpdateLocationData(const float deltaSeconds)
{
	LocationData.DisplacementSinceLastUpdate = (PreUpdateData.ActorLocation - LocationData.WorldLocation).Size2D();
	LocationData.WorldLocation = PreUpdateData.ActorLocation;
	LocationData.DisplacementSpeed = UKismetMathLibrary::SafeDivide(LocationData.DisplacementSinceLastUpdate,
	                                                                deltaSeconds);
	if (bIsFirstUpdate)
	{
		LocationData.DisplacementSinceLastUpdate = 0.f;
		LocationData.DisplacementSpeed = 0.f;
	}
}

void USamuraiBaseAnimInstance::UpdateAccelerationData()
{
	const FVector& worldAcceleration2D = PreUpdateData.ActorAcceleration * FVector(1.f, 1.f, 0.f);
	AccelerationData.LocalAcceleration2D = UKismetMathLibrary::LessLess_VectorRotator(
		worldAcceleration2D, RotationData.WorldRotation);
	AccelerationData.bHasAcceleration = !FMath::IsNearlyZero(AccelerationData.LocalAcceleration2D.Size2D());
	const FVector& lerpBetweenPivotAndWorldAcc = UKismetMathLibrary::VLerp(
		AccelerationData.PivotDirection2D,
		UKismetMathLibrary::Normal(worldAcceleration2D), .5f);
	AccelerationData.PivotDirection2D = UKismetMathLibrary::Normal(lerpBetweenPivotAndWorldAcc);
	const ESamuraiCardinalDirection selectedDirectionFromAngle = USamuraiAnimationBPLibrary::SelectCardinalDirectionFromAngle(
		UKismetAnimationLibrary::CalculateDirection(AccelerationData.PivotDirection2D, RotationData.WorldRotation),
		SettingsData.CardinalDirectionDeadZone);
	LocomotionSMData.CardinalDirectionFromAcceleration = USamuraiAnimationBPLibrary::GetOppositeCardinalDirection(selectedDirectionFromAngle);
}

void USamuraiBaseAnimInstance::UpdateVelocityData(const float deltaSeconds)
{
	const bool wasMovingLastUpdate = !VelocityData.LocalVelocity2D.IsZero();
	VelocityData.WorldVelocity = PreUpdateData.ActorVelocity;
	const FVector& worldVelocity2D = VelocityData.WorldVelocity * FVector(1.f, 1.f, 1.f);
	VelocityData.LocalVelocity2D = UKismetMathLibrary::LessLess_VectorRotator(
		worldVelocity2D, RotationData.WorldRotation);
	
	VelocityData.LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(
		worldVelocity2D, RotationData.WorldRotation);
	VelocityData.LocalVelocityDirectionAngleWithOffset = VelocityData.LocalVelocityDirectionAngle - TurnInPlaceData.
		RootYawOffset;

	VelocityData.LocalVelocityDirection = USamuraiAnimationBPLibrary::SelectCardinalDirectionFromAngle(
		VelocityData.LocalVelocityDirectionAngleWithOffset,
		SettingsData.CardinalDirectionDeadZone,
		VelocityData.LocalVelocityDirection, wasMovingLastUpdate);
	VelocityData.LocalVelocityDirectionNoOffset = USamuraiAnimationBPLibrary::SelectCardinalDirectionFromAngle(
		VelocityData.LocalVelocityDirectionAngle,
		SettingsData.CardinalDirectionDeadZone,
		VelocityData.LocalVelocityDirectionNoOffset,
		wasMovingLastUpdate);
	VelocityData.bHasVelocity = !FMath::IsNearlyZero(VelocityData.LocalVelocity2D.Size2D());
}

void USamuraiBaseAnimInstance::UpdatePawnStateData(const float deltaSeconds)
{
	// On Ground State.
	const bool isOnGround = PreUpdateData.bIsMovingOnGround;
	PawnStateData.bIsOnGround = isOnGround;
	if (isOnGround)
	{
		PawnStateData.bIsPivot = VelocityData.LocalVelocity2D.Dot(AccelerationData.LocalAcceleration2D) < 0.f && !
			PawnStateData.bIsRunningIntoWall;
	}

	// Crouch State.
	const bool wasCrouchingLastUpdate = PawnStateData.bIsCrouching;
	PawnStateData.bIsCrouching = PreUpdateData.bIsCrouching;
	PawnStateData.bIsCrouchStateChanged = (PawnStateData.bIsCrouching != wasCrouchingLastUpdate);

	// Aim State.
	const bool wasAimLastUpdate = PreUpdateData.AbilityData.bIsAiming;
	PawnStateData.bIsAiming = PreUpdateData.AbilityData.bIsAiming != wasAimLastUpdate;

	// Weapon State.
	PawnStateData.TimeSinceUsedWeapon = (PreUpdateData.AbilityData.bIsUsingItem)
		                                    ? 0.f
		                                    : PawnStateData.TimeSinceUsedWeapon + deltaSeconds;

	// In Air State.
	PawnStateData.bIsJumping = PawnStateData.bIsFalling = false;
	if (PreUpdateData.bIsFalling)
	{
		if (VelocityData.WorldVelocity.Z > 0.f)
		{
			PawnStateData.bIsJumping = true;
		}
		else
		{
			PawnStateData.bIsFalling = true;
		}
	}
}

void USamuraiBaseAnimInstance::UpdateBlendWeightData(const float deltaSeconds)
{
	BlendWeightData.UpperBodyDynamicAdditiveWeight = (PreUpdateData.bIsAnyMontagePlaying && PawnStateData.bIsOnGround)
		                                                 ? 1.f
		                                                 : FMath::FInterpTo(
			                                                 BlendWeightData.UpperBodyDynamicAdditiveWeight, 0.f,
			                                                 deltaSeconds, 6.f);
}

void USamuraiBaseAnimInstance::UpdateAimingData()
{
	AimingData.AimPitch = UKismetMathLibrary::NormalizeAxis(PreUpdateData.AimPitch);
}

void USamuraiBaseAnimInstance::UpdateJumpFallData()
{
	PawnStateData.TimeToJumpApex = (PawnStateData.bIsJumping)
		                               ? UKismetMathLibrary::SafeDivide(-VelocityData.WorldVelocity.Z,
		                                                                PreUpdateData.GravityZ)
		                               : 0.f;
}

void USamuraiBaseAnimInstance::UpdateWallDetectionHeuristic()
{
	const FVector& normalAcceleration2D = UKismetMathLibrary::Normal(AccelerationData.LocalAcceleration2D);
	const FVector& normalLocalVelocity2D = UKismetMathLibrary::Normal(VelocityData.LocalVelocity2D);

	PawnStateData.bIsRunningIntoWall = (AccelerationData.LocalAcceleration2D.Size2D() > .1f) && (VelocityData.
		LocalVelocity2D.Size2D() < 200.f) && UKismetMathLibrary::InRange_FloatFloat(
		UKismetMathLibrary::Dot_VectorVector(normalAcceleration2D, normalLocalVelocity2D), -.6f, .6f);
}

void USamuraiBaseAnimInstance::UpdateRootYawOffset(const float deltaSeconds)
{
	// When the feet aren't moving (e.g. during Idle), offset the root in the opposite direction to the Pawn owner's rotation to keep the mesh from rotating with the Pawn.
	if (TurnInPlaceData.RootYawOffsetMode == ESamuraiRootYawOffsetMode::EAccumulate)
	{
		SetRootYawOffset(TurnInPlaceData.RootYawOffset - RotationData.YawDeltaSinceLastUpdate);
	}

	// When in motion, smoothly blend out the offset.
	if (PreUpdateData.AbilityData.bIsDodging && TurnInPlaceData.RootYawOffsetMode == ESamuraiRootYawOffsetMode::EBlendOut)
	{
		SetRootYawOffset(UKismetMathLibrary::FloatSpringInterp(TurnInPlaceData.RootYawOffset, 0.f,
		                                                       TurnInPlaceData.RootYawOffsetSpringState, 80.f, 1.f,
		                                                       deltaSeconds, 1.f, .5f));
	}

	// 	Reset to blending out the yaw offset. Each update, a state needs to request to accumulate or hold the offset. Otherwise, the offset will blend out.
	// This is primarily because the majority of states want the offset to blend out, so this saves on having to tag each state.
	TurnInPlaceData.RootYawOffsetMode = ESamuraiRootYawOffsetMode::EBlendOut;
}

void USamuraiBaseAnimInstance::SetRootYawOffset(const float rootYawOffset)
{
	if (bEnableRootYawOffset)
	{
		const float normalAxisRootYawOffset = UKismetMathLibrary::NormalizeAxis(rootYawOffset);

		const FVector2D& angleClamp = (PawnStateData.bIsCrouching)
			                              ? TurnInPlaceData.RootYawOffsetAngleClampCrouched
			                              : TurnInPlaceData.RootYawOffsetAngleClamp;

		TurnInPlaceData.RootYawOffset = (angleClamp.X == angleClamp.Y)
			                                ? normalAxisRootYawOffset
			                                : UKismetMathLibrary::ClampAngle(
				                                normalAxisRootYawOffset, angleClamp.X, angleClamp.Y);
		AimingData.AimYaw = TurnInPlaceData.RootYawOffset * -1.f;
		return;
	}

	TurnInPlaceData.RootYawOffset = 0.f;
	AimingData.AimYaw = 0.f;
}

void USamuraiBaseAnimInstance::ProcessTurnYawCurve()
{
	// 	The "TurnYawWeight" curve is set to 1 in TurnInPlace animations, so its current value from GetCurveValue will be the current weight of the TurnInPlace animation. 
	// We can use this to "unweight" the TurnInPlace animation to get the full RemainingTurnYaw curve value. 
	float previousTurnYawCurveValue = TurnInPlaceData.TurnYawCurveValue;
	if (FMath::IsNearlyZero(GetCurveValue(NAME_TurnYawWeight)))
	{
		TurnInPlaceData.TurnYawCurveValue = 0.f;
		previousTurnYawCurveValue = 0.f;
		return;
	}

	TurnInPlaceData.TurnYawCurveValue += UKismetMathLibrary::SafeDivide(GetCurveValue(NAME_RemainingTurnYaw),
	                                                                    GetCurveValue(NAME_TurnYawWeight));

	// Avoid applying the curve delta when the curve first becomes relevant. E.g.
	// When a turn animation starts, the previous curve value will be 0 and the current value will be 90, but no actual rotation has happened yet.
	if (previousTurnYawCurveValue != 0.f)
	{
		// Reduce the root yaw offset by the amount of rotation from the turn animation.
		SetRootYawOffset(TurnInPlaceData.RootYawOffset - TurnInPlaceData.TurnYawCurveValue - previousTurnYawCurveValue);
	}
}

bool USamuraiBaseAnimInstance::IsMovingPerpendicularToInitialPivot() const
{
	// We stay in a pivot when pivoting along a line (e.g. triggering a left-right pivot while playing a right-left pivot),
	// but break out if the character makes a perpendicular change in direction.
	const ESamuraiCardinalDirection pivotInitialDirection = LocomotionSMData.PivotInitialDirection;
	const ESamuraiCardinalDirection localVelocityDirection = VelocityData.LocalVelocityDirection;

	const bool isInitialPivotVerticalDir = (pivotInitialDirection == ESamuraiCardinalDirection::EForward ||
		pivotInitialDirection == ESamuraiCardinalDirection::EBackward);
	const bool isLocalVelocityVerticalDir = (localVelocityDirection == ESamuraiCardinalDirection::EForward ||
		localVelocityDirection == ESamuraiCardinalDirection::EBackward);
	const bool isPivotVerticalDirPerpendicularToVelocityDir = (isInitialPivotVerticalDir && !isLocalVelocityVerticalDir);

	const bool isInitialPivotDirHorizontalDir = (pivotInitialDirection == ESamuraiCardinalDirection::ELeft ||
		pivotInitialDirection == ESamuraiCardinalDirection::ERight);
	const bool isLocalVelocityHorizontalDir = (localVelocityDirection == ESamuraiCardinalDirection::ELeft ||
		localVelocityDirection == ESamuraiCardinalDirection::ERight);
	const bool isPivotHorizontalDirPerpendicularToVerticalDir = (isInitialPivotDirHorizontalDir && !isLocalVelocityHorizontalDir);

	return isPivotVerticalDirPerpendicularToVelocityDir || isPivotHorizontalDirPerpendicularToVerticalDir;
}

FSamuraiAnimationStateNativeDelegate& USamuraiBaseAnimInstance::GetIdleAnimationExitNativeDelegate()
{
	return IdleAnimationExitNativeDelegate;
}

FSamuraiAnimationStateNativeDelegate& USamuraiBaseAnimInstance::GetStopAnimationEnterNativeDelegate()
{
	return StopAnimationEnterNativeDelegate;
}

void USamuraiBaseAnimInstance::HandleOnIdleAnimationExit(const FSamuraiDynamicMontageParams& params)
{
	StopSlotAnimation(params.BlendOutTime, params.SlotName);
}

void USamuraiBaseAnimInstance::HandleOnStopAnimationEnter(const FSamuraiDynamicMontageParams& params)
{
	PlayTransition(params);
}

void USamuraiBaseAnimInstance::BroadcastIdleAnimationExit(FSamuraiDynamicMontageParams params)
{
	if (IdleAnimationExitNativeDelegate.IsBound())
	{
		IdleAnimationExitNativeDelegate.Broadcast(params);
	}
}

void USamuraiBaseAnimInstance::PlayTransition(const FSamuraiDynamicMontageParams& params)
{
	PlaySlotAnimationAsDynamicMontage(params.Animation, params.SlotName,
								  params.BlendInTime, params.BlendOutTime, params.PlayRate, 1,
								  0.0f, params.StartTime);
}

const float& USamuraiBaseAnimInstance::GetDisplacementSpeed() const
{
	return LocationData.DisplacementSpeed;
}

float USamuraiBaseAnimInstance::GetSpeed2D() const
{
	return PreUpdateData.ActorVelocity.Size2D();
}

const FSamuraiPreUpdateData& USamuraiBaseAnimInstance::GetPreUpdateData() const
{
	return PreUpdateData;
}

const FSamuraiRotationData& USamuraiBaseAnimInstance::GetRotationData() const
{
	return RotationData;
}

const FSamuraiVelocityData& USamuraiBaseAnimInstance::GetVelocityData() const
{
	return VelocityData;
}

const FSamuraiPawnStateData& USamuraiBaseAnimInstance::GetPawnStateData() const
{
	return PawnStateData;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimInstances/Pawns/SamuraiBaseLinkedAnimInstance.h"

#include "SamuraiGameplayTags.h"
#include "Animations/AnimInstances/Pawns/SamuraiBaseAnimInstance.h"
#include "Animations/Library/SamuraiAnimationBPLibrary.h"

void USamuraiBaseLinkedAnimInstance::NativeInitializeAnimation()
{
	// Don't call Super since it is empty.

	BaseMainAnimInstance = Cast<USamuraiBaseAnimInstance>(GetOwningComponent()->GetAnimInstance());

	OwningComponentZScale = GetOwningComponent()->GetComponentScale().Z;
}

void USamuraiBaseLinkedAnimInstance::NativeUpdateAnimation(float deltaSeconds)
{
	// Don't call Super since it is empty.

	if (BaseMainAnimInstance)
	{
		LocomotionStateTag = BaseMainAnimInstance->GetPreUpdateData().AnimationData.LocomotionStateTag;
		OwningSpeed2D = BaseMainAnimInstance->GetSpeed2D();
		bIsCrouching = BaseMainAnimInstance->GetPawnStateDate().bIsCrouching;
		LocalVelocityDirectionNoOffset = BaseMainAnimInstance->GetVelocityData().LocalVelocityDirectionNoOffset;
		Velocity = BaseMainAnimInstance->GetVelocityData().WorldVelocity;
		LastUpdateRotation = BaseMainAnimInstance->GetRotationData().WorldRotation;
		bIsMovingOnGround = BaseMainAnimInstance->GetPreUpdateData().bIsMovingOnGround;
	}
	
	UpdateSkeletalControls(deltaSeconds);
}

void USamuraiBaseLinkedAnimInstance::NativeThreadSafeUpdateAnimation(float deltaSeconds)
{
}

void USamuraiBaseLinkedAnimInstance::UpdateSkeletalControls(float deltaSeconds)
{
	FVector footOffsetLTarget = FVector::ZeroVector;
	FVector footOffsetRTarget = FVector::ZeroVector;

	// Update Foot Locking values.
	SetFootLocking(deltaSeconds, NAME_Enable_FootIK_L, NAME_FootLock_L,
				   NAME_IK_Foot_L, FootIKValues.FootLock_L_Alpha, FootIKValues.bUseFootLockCurve_L,
				   FootIKValues.FootLock_L_Location, FootIKValues.FootLock_L_Rotation);
	SetFootLocking(deltaSeconds, NAME_Enable_FootIK_R, NAME_FootLock_R,
				   NAME_IK_Foot_R, FootIKValues.FootLock_R_Alpha, FootIKValues.bUseFootLockCurve_R,
				   FootIKValues.FootLock_R_Location, FootIKValues.FootLock_R_Rotation);

	// if (MovementState.InAir())
	// {
	// 	// Reset IK Offsets if In Air
	// 	SetPelvisIKOffset(DeltaSeconds, FVector::ZeroVector, FVector::ZeroVector);
	// 	ResetIKOffsets(DeltaSeconds);
	// }
	// else if (!MovementState.Ragdoll())
	// {
	// 	// Update all Foot Lock and Foot Offset values when not In Air
	// 	SetFootOffsets(DeltaSeconds, NAME_Enable_FootIK_L, IkFootL_BoneName, NAME__ALSCharacterAnimInstance__root,
	// 				   FootOffsetLTarget,
	// 				   FootIKValues.FootOffset_L_Location, FootIKValues.FootOffset_L_Rotation);
	// 	SetFootOffsets(DeltaSeconds, NAME_Enable_FootIK_R, IkFootR_BoneName, NAME__ALSCharacterAnimInstance__root,
	// 				   FootOffsetRTarget,
	// 				   FootIKValues.FootOffset_R_Location, FootIKValues.FootOffset_R_Rotation);
	// 	SetPelvisIKOffset(DeltaSeconds, FootOffsetLTarget, FootOffsetRTarget);
	// }

	// Update all Foot Lock and Foot Offset values when not In Air
	SetFootOffsets(deltaSeconds, NAME_Enable_FootIK_L, NAME_IK_Foot_L, NAME_RootBone,
				   footOffsetLTarget,
				   FootIKValues.FootOffset_L_Location, FootIKValues.FootOffset_L_Rotation);
	SetFootOffsets(deltaSeconds, NAME_Enable_FootIK_R, NAME_Enable_FootIK_R, NAME_RootBone,
				   footOffsetRTarget,
				   FootIKValues.FootOffset_R_Location, FootIKValues.FootOffset_R_Rotation);
	SetPelvisIKOffset(deltaSeconds, footOffsetLTarget, footOffsetRTarget);
}

void USamuraiBaseLinkedAnimInstance::SetFootLocking(float deltaSeconds, FName enableFootIKCurve, FName footLockCurve,
                                                    FName ikFootBone, float& curFootLockAlpha, bool& useFootLockCurve,
                                                    FVector& curFootLockLoc, FRotator& curFootLockRot)
{
	if (GetCurveValue(enableFootIKCurve) <= 0.0f)
	{
		return;
	}

	// Step 1: Set Local FootLock Curve value
	float footLockCurveVal = 0.f;

	if (useFootLockCurve)
	{
		useFootLockCurve = FMath::Abs(GetCurveValue(NAME_RootBone)) <= 0.001f; // || Character->GetLocalRole() != ROLE_AutonomousProxy;
		footLockCurveVal = GetCurveValue(footLockCurve) * (1.f / GetSkelMeshComponent()->AnimUpdateRateParams->UpdateRate);
	}
	else
	{
		useFootLockCurve = GetCurveValue(footLockCurve) >= 0.99f;
		footLockCurveVal = 0.0f;
	}

	// Step 2: Only update the FootLock Alpha if the new value is less than the current, or it equals 1. This makes it
	// so that the foot can only blend out of the locked position or lock to a new position, and never blend in.
	if (footLockCurveVal >= 0.99f || footLockCurveVal < curFootLockAlpha)
	{
		curFootLockAlpha = footLockCurveVal;
	}

	// Step 3: If the Foot Lock curve equals 1, save the new lock location and rotation in component space as the target.
	if (curFootLockAlpha >= 0.99f)
	{
		const FTransform& ownerTransform =
			GetOwningComponent()->GetSocketTransform(ikFootBone, RTS_Component);
		curFootLockLoc = ownerTransform.GetLocation();
		curFootLockRot = ownerTransform.Rotator();
	}

	// Step 4: If the Foot Lock Alpha has a weight,
	// update the Foot Lock offsets to keep the foot planted in place while the capsule moves.
	if (curFootLockAlpha > 0.0f)
	{
		SetFootLockOffsets(deltaSeconds, curFootLockLoc, curFootLockRot);
	}
}

void USamuraiBaseLinkedAnimInstance::SetFootLockOffsets(float deltaSeconds, FVector& localLoc, FRotator& localRot)
{
	FRotator rotationDifference = FRotator::ZeroRotator;
	// Use the delta between the current and last updated rotation to find how much the foot should be rotated
	// to remain planted on the ground.
	if (bIsMovingOnGround)
	{
		rotationDifference = GetOwningActor()->GetActorRotation() - LastUpdateRotation;
		rotationDifference.Normalize();
	}

	// Get the distance traveled between frames relative to the mesh rotation
	// to find how much the foot should be offset to remain planted on the ground.
	const FVector& LocationDifference = GetOwningComponent()->GetComponentRotation().UnrotateVector(
		Velocity * deltaSeconds);

	// Subtract the location difference from the current local location and rotate
	// it by the rotation difference to keep the foot planted in component space.
	localLoc = (localLoc - LocationDifference).RotateAngleAxis(rotationDifference.Yaw, FVector::DownVector);

	// Subtract the Rotation Difference from the current Local Rotation to get the new local rotation.
	FRotator delta = localRot - rotationDifference;
	delta.Normalize();
	localRot = delta;
}

void USamuraiBaseLinkedAnimInstance::SetPelvisIKOffset(float deltaSeconds, FVector footOffsetLTarget,
                                                       FVector footOffsetRTarget)
{
	// Calculate the Pelvis Alpha by finding the average Foot IK weight. If the alpha is 0, clear the offset.
	FootIKValues.PelvisAlpha =
		(GetCurveValue(NAME_Enable_FootIK_L) + GetCurveValue(NAME_Enable_FootIK_R)) / 2.0f;

	if (FootIKValues.PelvisAlpha > 0.0f)
	{
		// Step 1: Set the new Pelvis Target to be the lowest Foot Offset
		const FVector PelvisTarget = footOffsetLTarget.Z < footOffsetRTarget.Z ? footOffsetLTarget : footOffsetRTarget;

		// Step 2: Interp the Current Pelvis Offset to the new target value.
		//Interpolate at different speeds based on whether the new target is above or below the current one.
		const float InterpSpeed = PelvisTarget.Z > FootIKValues.PelvisOffset.Z ? 10.0f : 15.0f;
		FootIKValues.PelvisOffset =
			FMath::VInterpTo(FootIKValues.PelvisOffset, PelvisTarget, deltaSeconds, InterpSpeed);
	}
	else
	{
		FootIKValues.PelvisOffset = FVector::ZeroVector;
	}
}

void USamuraiBaseLinkedAnimInstance::ResetIKOffsets(float deltaSeconds)
{
	// Interp Foot IK offsets back to 0
	FootIKValues.FootOffset_L_Location = FMath::VInterpTo(FootIKValues.FootOffset_L_Location,
														  FVector::ZeroVector, deltaSeconds, 15.0f);
	FootIKValues.FootOffset_R_Location = FMath::VInterpTo(FootIKValues.FootOffset_R_Location,
														  FVector::ZeroVector, deltaSeconds, 15.0f);
	FootIKValues.FootOffset_L_Rotation = FMath::RInterpTo(FootIKValues.FootOffset_L_Rotation,
														  FRotator::ZeroRotator, deltaSeconds, 15.0f);
	FootIKValues.FootOffset_R_Rotation = FMath::RInterpTo(FootIKValues.FootOffset_R_Rotation,
														  FRotator::ZeroRotator, deltaSeconds, 15.0f);
}

void USamuraiBaseLinkedAnimInstance::SetFootOffsets(float deltaSeconds, FName enableFootIKCurve, FName ikFootBone,
                                                    FName rootBone, FVector& curLocationTarget,
                                                    FVector& curLocationOffset, FRotator& curRotationOffset)
{
	// Only update Foot IK offset values if the Foot IK curve has a weight. If it equals 0, clear the offset values.
	if (GetCurveValue(enableFootIKCurve) <= 0)
	{
		curLocationOffset = FVector::ZeroVector;
		curRotationOffset = FRotator::ZeroRotator;
		return;
	}

	// Step 1: Trace downward from the foot location to find the geometry.
	// If the surface is walkable, save the Impact Location and Normal.
	USkeletalMeshComponent* ownerComp = GetOwningComponent();
	FVector ikFootFloorLoc = ownerComp->GetSocketLocation(ikFootBone);
	ikFootFloorLoc.Z = ownerComp->GetSocketLocation(rootBone).Z;

	UWorld* world = GetWorld();
	check(world);

	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwningActor());

	const FVector traceStart = ikFootFloorLoc + FVector(0.0, 0.0, IK_TraceDistanceAboveFoot);
	const FVector traceEnd = ikFootFloorLoc - FVector(0.0, 0.0, IK_TraceDistanceBelowFoot);

	FHitResult hitResult;
	const bool isHit = world->LineTraceSingleByChannel(hitResult,
	                                                  traceStart,
	                                                  traceEnd,
	                                                  ECC_Visibility, params);

	FRotator targetRotOffset = FRotator::ZeroRotator;
	if (isHit)
	{
		FVector impactPoint = hitResult.ImpactPoint;
		FVector impactNormal = hitResult.ImpactNormal;

		// Step 1.1: Find the difference in location from the Impact point and the expected (flat) floor location.
		// These values are offset by the normal multiplied by the
		// foot height to get better behavior on angled surfaces.
		curLocationTarget = (impactPoint + impactNormal * 13.5f) -
			(ikFootFloorLoc + FVector(0, 0, 13.5f));

		// Step 1.2: Calculate the Rotation offset by getting the Atan2 of the Impact Normal.
		targetRotOffset.Pitch = -FMath::RadiansToDegrees(FMath::Atan2(impactNormal.X, impactNormal.Z));
		targetRotOffset.Roll = FMath::RadiansToDegrees(FMath::Atan2(impactNormal.Y, impactNormal.Z));
	}

	// Step 2: Interp the Current Location Offset to the new target value.
	// Interpolate at different speeds based on whether the new target is above or below the current one.
	const float interpSpeed = curLocationOffset.Z > curLocationTarget.Z ? 30.f : 15.0f;
	curLocationOffset = FMath::VInterpTo(curLocationOffset, curLocationTarget, deltaSeconds, interpSpeed);

	// Step 3: Interp the Current Rotation Offset to the new target value.
	curRotationOffset = FMath::RInterpTo(curRotationOffset, targetRotOffset, deltaSeconds, 30.0f);

}

bool USamuraiBaseLinkedAnimInstance::IsOnPivot() const
{
	return bIsOnPivot;
}

bool USamuraiBaseLinkedAnimInstance::IsMovingForwardOrBackward() const
{
	return bIsMovingForwardOrBackward;
}

const float& USamuraiBaseLinkedAnimInstance::GetBlendSpaceDirection() const
{
	return BlendSpaceDirection;
}

const float& USamuraiBaseLinkedAnimInstance::GetBlendSpacePlayRate() const
{
	return BlendSpacePlayRate;
}

const float& USamuraiBaseLinkedAnimInstance::GetStrideBlend() const
{
	return StrideBlend;
}

const float& USamuraiBaseLinkedAnimInstance::GetLocomotionCycleBlendWeight() const
{
	return LocomotionCycleBlendWeight;
}

const float& USamuraiBaseLinkedAnimInstance::GetBlendSpaceDetailPlayRate() const
{
	return BlendSpaceDetailPlayRate;
}

const float& USamuraiBaseLinkedAnimInstance::GetBlendSpaceDetailStartPosition() const
{
	return BlendSpaceDetailStartPosition;
}

void USamuraiBaseLinkedAnimInstance::SetIsOnPivot(const bool isOnPivot)
{
	bIsOnPivot = isOnPivot;
}

void USamuraiBaseLinkedAnimInstance::SetIsMovingForwardOrBackward(const bool isForwardOrBackward)
{
	bIsMovingForwardOrBackward = isForwardOrBackward;
}

void USamuraiBaseLinkedAnimInstance::SetMovementDirection(const ESamuraiMovementDirection movementDirection)
{
	MovementDirection = movementDirection;
}

USamuraiBaseAnimInstance* USamuraiBaseLinkedAnimInstance::GetBaseMainAnimInstanceThreadSafe() const
{
	return BaseMainAnimInstance;
}

ESamuraiMovementDirection USamuraiBaseLinkedAnimInstance::GetMovementDirection() const
{
	return MovementDirection;
}

bool USamuraiBaseLinkedAnimInstance::IsLocomotionStateTagActive(FGameplayTag locomotionStateTag) const
{
	return (locomotionStateTag.IsValid() && locomotionStateTag.MatchesTagExact(LocomotionStateTag));
}

bool USamuraiBaseLinkedAnimInstance::IsMovementDirectionForward() const
{
	return (MovementDirection == ESamuraiMovementDirection::EForward);
}

ESamuraiMovementDirection USamuraiBaseLinkedAnimInstance::GetOppositeMovementDirection(const ESamuraiMovementDirection movementDirection) const
{
	ESamuraiMovementDirection result = ESamuraiMovementDirection::EForward;
	switch (movementDirection)
	{
	case ESamuraiMovementDirection::EForward:
		result = ESamuraiMovementDirection::EBackward;
		break;
	case ESamuraiMovementDirection::EBackward:
		result = ESamuraiMovementDirection::EForward;
		break;
	}

	return result;
}

ESamuraiStance USamuraiBaseLinkedAnimInstance::GetStance() const
{
	return (bIsCrouching) ? ESamuraiStance::ECrouching : ESamuraiStance::EStanding;
}

const float& USamuraiBaseLinkedAnimInstance::GetVelocityBlendInterpSpeed() const
{
	return (GetStance() == ESamuraiStance::EStanding)
		       ? CycleAnimationSets_Standing.VelocityBlendInterpSpeed
		       : CycleAnimationSets_Crouching.VelocityBlendInterpSpeed;
}

FGameplayTag USamuraiBaseLinkedAnimInstance::GetLocomotionStateTagFromBlendWeight(const float blendWeight) const
{
	return (blendWeight == 1.f) ? GTag_Movement_State_Walk : GTag_Movement_State_Run;
}

void USamuraiBaseLinkedAnimInstance::GetCycleMovementDetailAnimationSetSettingsByLocomotionState(const FGameplayTag& locomotionStateTag,
																								 FSamuraiCycleDetailAnimationSettings& outSetSettings) const
{
	if (!locomotionStateTag.IsValid())
	{
		return;
	}
	
	outSetSettings = *CycleDetailAnimationSet_Standing.AnimationSetSettings.Find(locomotionStateTag);
}

void USamuraiBaseLinkedAnimInstance::GetCycleMovementAnimationSetByLocomotionState(const ESamuraiStance stance,
                                                                                   const FGameplayTag& locomotionStateTag,
                                                                                   FSamuraiCycleAnimationSet& outAnimationSet) const
{
	if (!locomotionStateTag.IsValid() || stance == ESamuraiStance::ENone)
	{
		return;
	}

	if (stance == ESamuraiStance::EStanding)
	{
		outAnimationSet = *CycleAnimationSets_Standing.AnimationSets.Find(locomotionStateTag);
		return;
	}

	outAnimationSet = *CycleAnimationSets_Crouching.AnimationSets.Find(locomotionStateTag);
} 

void USamuraiBaseLinkedAnimInstance::GetCycleMovementAnimationSetSettingsByLocomotionState(const ESamuraiStance stance,
																						   const FGameplayTag& locomotionStateTag,
																						   FSamuraiCycleAnimationSetSettings& outSetSettings) const
{
	FSamuraiCycleAnimationSet animationSet;
	GetCycleMovementAnimationSetByLocomotionState(stance, locomotionStateTag, animationSet);
	outSetSettings = animationSet.AnimationSetSettings;
}

float USamuraiBaseLinkedAnimInstance::GetAnimCurveClamped(FName curveName, const float bias, const float clampMin,
                                                          const float clampMax) const
{
	return FMath::Clamp(GetCurveValue(curveName) + bias, clampMin, clampMax);
}

float USamuraiBaseLinkedAnimInstance::CalculateStrideBlend(const float animatedSpeed,
                                                           const FVector2D& strideBlendRange) const
{
	// Calculate the stride scale by dividing the owning speed in the 2D plane by the component's Z scale
	const float strideScale = UKismetMathLibrary::SafeDivide(OwningSpeed2D, OwningComponentZScale);

	// Map the stride scale to the specified stride blend range using clamped interpolation
	return FMath::GetMappedRangeValueClamped(
		FVector2D(0.f, animatedSpeed),
		strideBlendRange, strideScale);
}

float USamuraiBaseLinkedAnimInstance::CalculatePlayRate(const float animatedSpeed, const float strideBlend,
                                                        const FVector2D& playRateClamp) const
{
	// Calculate the animation speed rate by dividing the owning 2D speed by the animated speed
	const float animationSpeedRate = UKismetMathLibrary::SafeDivide(OwningSpeed2D, animatedSpeed);

	// Extract the min and max play rates from the specified play rate clamp range
	const float minPlayRate = playRateClamp.X;
	const float maxPlayRate = playRateClamp.Y;

	// Calculate the play rate by dividing animation speed rate by stride blend and Z scale, then clamp the result
	return FMath::Clamp(animationSpeedRate / strideBlend / OwningComponentZScale, minPlayRate, maxPlayRate);
}

void USamuraiBaseLinkedAnimInstance::CalculateStrideBlendAndPlayRate(const float animatedSpeed,
                                                                     const FVector2D& strideBlendRange,
                                                                     const FVector2D& playRateClamp,
                                                                     float& outStrideBlend, float& outPlayRate) const
{
	// Calculate the stride blend factor based on the animated speed and specified stride blend range
	outStrideBlend = CalculateStrideBlend(animatedSpeed, strideBlendRange);

	// Calculate the play rate based on the animated speed, previously calculated stride blend, and play rate clamp range
	outPlayRate = CalculatePlayRate(animatedSpeed, outStrideBlend, playRateClamp);
}

void USamuraiBaseLinkedAnimInstance::CalculateMovementDirection(const float directionAngle,
                                                                const float dirMin, const float dirMax,
                                                                const float buffer, ESamuraiMovementDirection& outMovementDirection) const
{
	// Retrieve the current direction from the provided angle.
	const float currentDirection = directionAngle;

	// Check if the movement direction is considered forward based on the configured conditions.
	bool isForward = false;
	if (IsMovementDirectionForward())
	{
		// Check if the current direction is within the forward range with the specified buffer.
		isForward = USamuraiAnimationBPLibrary::InRange(FMath::Abs(currentDirection),
														dirMin + dirMax,
														dirMax + buffer);
	}
	else
	{
		// Check if the current direction is within the reverse range with the specified buffer.
		isForward = USamuraiAnimationBPLibrary::InRange(FMath::Abs(currentDirection),
														dirMin - dirMax,
														dirMax + buffer);
	}

	outMovementDirection = (isForward) ? ESamuraiMovementDirection::EForward : ESamuraiMovementDirection::EBackward;
}

void USamuraiBaseLinkedAnimInstance::CalculateBlendSpaceDirection(const float directionAngle, float& outBlendSpaceDir) const
{
	// Update output variables based on the calculated movement direction.
	if (IsMovementDirectionForward())
	{
		outBlendSpaceDir = directionAngle;
	}
	else
	{
		// Determine if the movement is towards the right or left.
		const bool isRight = (directionAngle >= 0.f);

		// Map the direction angle to a new range for blend space based on movement to the left.
		const float mapValue = UKismetMathLibrary::MapRangeClamped(FMath::Abs(directionAngle), 90.f, 180.f, 90.f, 0.f);

		// Update the blend space direction based on the mapping and the movement direction.
		outBlendSpaceDir = (isRight) ? mapValue : -mapValue;
	}
}

void USamuraiBaseLinkedAnimInstance::CalculateVelocityBlend(FSamuraiVelocityBlend& outVelocityBlend)
{
	// Check if the movement direction is forward
	if (IsMovementDirectionForward())
	{
		// Set blend values for forward movement
		outVelocityBlend.Forward = 1.0f;
		outVelocityBlend.Backward = 0.0f;
	}
	else
	{
		// Set blend values for backward movement
		outVelocityBlend.Forward = 0.0f;
		outVelocityBlend.Backward = 1.0f;
	}
}

void USamuraiBaseLinkedAnimInstance::CalculateHipDirection(const ESamuraiCardinalDirection cardinalDirection,
                                                           ESamuraiHipDirection& outHipDirection) const
{
	const bool isMovementDirectionForward = IsMovementDirectionForward();
	if (isMovementDirectionForward)
	{
		if (cardinalDirection == ESamuraiCardinalDirection::ERight)
		{
			outHipDirection = ESamuraiHipDirection::ERightForward;
		}
		else if (cardinalDirection == ESamuraiCardinalDirection::ELeft)
		{
			outHipDirection = ESamuraiHipDirection::ELeftForward;
		}
		else
		{
			outHipDirection = ESamuraiHipDirection::EForward;
		}
	}
	else
	{
		if (cardinalDirection == ESamuraiCardinalDirection::ERight)
		{
			outHipDirection = ESamuraiHipDirection::ERightBackward;
		}
		else if (cardinalDirection == ESamuraiCardinalDirection::ELeft)
		{
			outHipDirection = ESamuraiHipDirection::ELeftBackward;
		}
		else
		{
			outHipDirection = ESamuraiHipDirection::EBackward;
		}
	}
}

void USamuraiBaseLinkedAnimInstance::UpdateIdleData()
{
	SetIsOnPivot(false);
	VelocityBlend.Reset();
}

void USamuraiBaseLinkedAnimInstance::UpdateLocomotionCycleData(const float directionAngle, const float blendWeight)
{
	// Set the flag indicating whether the character is moving forward or backward based on the local velocity direction.
	SetIsMovingForwardOrBackward(USamuraiAnimationBPLibrary::IsMovingForwardOrBackward(LocalVelocityDirectionNoOffset));
	
	// Retrieve animation settings for the current stance and locomotion state
	FSamuraiCycleAnimationSetSettings animationSetSettings;
	GetCycleMovementAnimationSetSettingsByLocomotionState(GetStance(), LocomotionStateTag, animationSetSettings);

	// Calculate stride blend and play rate based on the obtained animation settings
	CalculateStrideBlendAndPlayRate(animationSetSettings.AnimatedSpeed, animationSetSettings.StrideBlendRange,
									animationSetSettings.PlayRateClamp, StrideBlend, BlendSpacePlayRate);
	// Determine movement direction based on the given angle
	if (!IsOnPivot() || IsMovingForwardOrBackward())
	{
		CalculateMovementDirection(directionAngle, -90.f, 90.f, 0.f, MovementDirection);
	}

	// Calculate the target velocity blend based on the current movement state
	FSamuraiVelocityBlend targetVelocityBlend;
	CalculateVelocityBlend(targetVelocityBlend);

	// Interpolate the current VelocityBlend values towards the target VelocityBlend values
	const float velocityBlendInterpSpeed = GetVelocityBlendInterpSpeed();
	VelocityBlend.Forward = FMath::FInterpTo(VelocityBlend.Forward, targetVelocityBlend.Forward, GetDeltaSeconds(), velocityBlendInterpSpeed);
	VelocityBlend.Backward = FMath::FInterpTo(VelocityBlend.Backward, targetVelocityBlend.Backward, GetDeltaSeconds(), velocityBlendInterpSpeed);

	CalculateHipDirection(LocalVelocityDirectionNoOffset, HipDirection);

	// Calculate the blend space direction based on the character's movement angle
	CalculateBlendSpaceDirection(directionAngle, BlendSpaceDirection);

	// Set the blend weight for the locomotion cycle state.
	LocomotionCycleBlendWeight = blendWeight;
}

void USamuraiBaseLinkedAnimInstance::UpdateLocomotionCycleDetailsData(const bool isPivot, const float blendWeight)
{
	// Retrieve locomotion cycle detail tag based on the provided blend weight
	const FGameplayTag& locomotionCycleDetailTag = (isPivot) ? GTag_Movement_State_Pivot : GetLocomotionStateTagFromBlendWeight(blendWeight);
	
	// Retrieve animation settings for the current locomotion cycle detail
	FSamuraiCycleDetailAnimationSettings animationSettings;
	GetCycleMovementDetailAnimationSetSettingsByLocomotionState(locomotionCycleDetailTag, animationSettings);

	// Set locomotion cycle detail play rate and start position based on the obtained animation settings
	BlendSpaceDetailPlayRate = animationSettings.PlayRate;
	BlendSpaceDetailStartPosition = animationSettings.StartPosition;
}

void USamuraiBaseLinkedAnimInstance::UpdateLocomotionCycleDetailsRunningData(const float deltaTime)
{
	const bool isOnPivot = IsOnPivot();
	if (isOnPivot)
	{
		TimePendingOutPivot += deltaTime;
		if (TimePendingOutPivot >= MaxPendingPivotTime || IsMovingForwardOrBackward())
		{
			TimePendingOutPivot = 0.f;
			SetIsOnPivot(false);
		}
	}
}

void USamuraiBaseLinkedAnimInstance::UpdateLocomotionCycleDetailsWalkingData(const float deltaTime)
{
	SetIsOnPivot(false);
	TimePendingOutPivot = 0.f;
}

void USamuraiBaseLinkedAnimInstance::DebugMessage(FString message)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, message);
}

UBlendSpace* USamuraiBaseLinkedAnimInstance::SelectCycleDetailAnimation(const ESamuraiMovementDirection movementDirection) const
{
	return (movementDirection == ESamuraiMovementDirection::EForward
		        ? CycleDetailAnimationSet_Standing.Forward_Details
		        : CycleDetailAnimationSet_Standing.Backward_Details);
}

UBlendSpace* USamuraiBaseLinkedAnimInstance::SelectCycleAnimationPose(const ESamuraiStance stance,
                                                                      const FGameplayTag locomotionStateTag) const
{
	FSamuraiCycleAnimationSet animationSet;
	GetCycleMovementAnimationSetByLocomotionState(stance, locomotionStateTag, animationSet);
	return animationSet.Poses;
}

UBlendSpace* USamuraiBaseLinkedAnimInstance::SelectCycleAnimation(const ESamuraiMovementDirection movementDirection,
                                                                  const ESamuraiStance stance,
                                                                  const FGameplayTag locomotionStateTag) const
{
	FSamuraiCycleAnimationSet animationSet;
	GetCycleMovementAnimationSetByLocomotionState(stance, locomotionStateTag, animationSet);

	return (movementDirection == ESamuraiMovementDirection::EForward ? animationSet.Forward : animationSet.Backward);
}

UAnimSequence* USamuraiBaseLinkedAnimInstance::SelectStopAnimation(const ESamuraiStance stance, const ESamuraiHipDirection hipDirection, const bool isLeftFoot, float& outExplicitTime) const
{
	const FSamuraiStopAnimationSet animationSet = StopAnimationSets_Standing.FindChecked(hipDirection);
	outExplicitTime = (isLeftFoot) ? animationSet.Settings.PlantLeftFoot : animationSet.Settings.PlantRightFoot;
	return animationSet.AnimSequence;
}

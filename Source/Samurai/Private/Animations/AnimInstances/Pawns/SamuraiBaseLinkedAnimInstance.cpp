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
	}
}

void USamuraiBaseLinkedAnimInstance::NativeThreadSafeUpdateAnimation(float deltaSeconds)
{
	// FSamuraiCycleAnimationSetSettings animationSettings;
	// GetCycleMovementAnimationSetSettingsByLocomotionState(GetStance(), LocomotionStateTag, animationSettings);
	// CalculateStrideBlendAndPlayRate(animationSettings.AnimatedSpeed, animationSettings.StrideBlendRange, animationSettings.PlayRateClamp, StrideBlend, PlayRate);
}

void USamuraiBaseLinkedAnimInstance::SetPivotMovementDirection(const ESamuraiMovementDirection pivotMovementDirection)
{
	PivotMovementDirection = pivotMovementDirection;
}

bool USamuraiBaseLinkedAnimInstance::IsOnPivot() const
{
	return bIsOnPivot;
}

bool USamuraiBaseLinkedAnimInstance::IsMovingForwardOrBackward() const
{
	return bIsMovingForwardOrBackward;
}

void USamuraiBaseLinkedAnimInstance::SetIsOnPivot(const bool isOnPivot)
{
	bIsOnPivot = isOnPivot;
}

void USamuraiBaseLinkedAnimInstance::SetIsMovingForwardOrBackward(const bool isForwardOrBackward)
{
	bIsMovingForwardOrBackward = isForwardOrBackward;
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

bool USamuraiBaseLinkedAnimInstance::IsMovementSaveDirection(const ESamuraiMovementDirection movementDirectionA,
	const ESamuraiMovementDirection movementDirectionB) const
{
	return (movementDirectionA == movementDirectionB);
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
		outAnimationSet = *CycleAnimationSet_Standing.Find(locomotionStateTag);
		return;
	}

	outAnimationSet = *CycleAnimationSet_Crouching.Find(locomotionStateTag);
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

void USamuraiBaseLinkedAnimInstance::UpdateIdleData()
{
	SetIsOnPivot(false);
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
									animationSetSettings.PlayRateClamp, StrideBlend, PlayRate);
	// Determine movement direction based on the given angle
	if (!IsOnPivot() || IsMovingForwardOrBackward())
	{
		CalculateMovementDirection(directionAngle, -90.f, 90.f, 0.f, MovementDirection);
	}

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
	LocomotionCycleDetailPlayRate = animationSettings.PlayRate;
	LocomotionCycleDetailStartPosition = animationSettings.StartPosition;
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Library/SamuraiAnimationBPLibrary.h"

#include "SamuraiGameplayTags.h"

ESamuraiCardinalDirection USamuraiAnimationBPLibrary::SelectCardinalDirectionFromAngle(const float angle,
                                                                                       const float deadZone, const ESamuraiCardinalDirection currentDirection,
                                                                                       const bool shouldUseCurrentDirection)
{
	const float absAngle = FMath::Abs(angle);

	// Favor playing Fwd/Bwd anims over strafing anims.
	float fwdDeadZone = deadZone;
	float bwdDeadZone = deadZone;

	if (shouldUseCurrentDirection)
	{
		switch (currentDirection)
		{
		// If moving Fwd/Bwd, double the Fwd dead zone.
		// It should be harder to leave Fwd when moving Fwd.
		// When moving Left/Right, the dead zone will be smaller so we don't rapidly toggle between directions.
		case ESamuraiCardinalDirection::EForward:
			fwdDeadZone *= 2.f;
			break;
		case ESamuraiCardinalDirection::EBackward:
			bwdDeadZone *= 2.f;
			break;
		case ESamuraiCardinalDirection::ELeft:
			break;
		case ESamuraiCardinalDirection::ERight:
			break;
		}
	}

	if (absAngle <= 45.f + fwdDeadZone)
	{
		return ESamuraiCardinalDirection::EForward;
	}
	
	if (absAngle >= 135.f - bwdDeadZone)
	{
		return ESamuraiCardinalDirection::EBackward;
	}

	if (angle > 0.f)
	{
		return ESamuraiCardinalDirection::ERight;
	}
	
	return ESamuraiCardinalDirection::ELeft;
}

ESamuraiCardinalDirection USamuraiAnimationBPLibrary::GetOppositeCardinalDirection(
	const ESamuraiCardinalDirection currentDirection)
{
	ESamuraiCardinalDirection result = ESamuraiCardinalDirection::EForward;
	switch (currentDirection)
	{
	case ESamuraiCardinalDirection::EForward:
		result = ESamuraiCardinalDirection::EBackward;
		break;
	case ESamuraiCardinalDirection::EBackward:
		result = ESamuraiCardinalDirection::EForward;
		break;
	case ESamuraiCardinalDirection::ELeft:
		result = ESamuraiCardinalDirection::ERight;
		break;
	case ESamuraiCardinalDirection::ERight:
		result = ESamuraiCardinalDirection::ELeft;
		break;
	default:
		result = ESamuraiCardinalDirection::EForward;
		break;
	}

	return result;
}

ESamuraiMovementDirection USamuraiAnimationBPLibrary::GetOppositeMovementDirection(const ESamuraiMovementDirection currentDirection)
{
	return  (currentDirection == ESamuraiMovementDirection::EForward ? ESamuraiMovementDirection::EBackward : ESamuraiMovementDirection::EForward);
}

bool USamuraiAnimationBPLibrary::IsMovingForwardOrBackward(const ESamuraiCardinalDirection currentDirection)
{
	return (currentDirection == ESamuraiCardinalDirection::EForward || currentDirection == ESamuraiCardinalDirection::EBackward);
}

FGameplayTag USamuraiAnimationBPLibrary::ConvertEnumGaitToGameplayTag(const ESamuraiGait gait)
{
	FGameplayTag result = FGameplayTag::EmptyTag;
	
	switch (gait)
	{
	case ESamuraiGait::EWalking:
		result = GTag_Movement_State_Walk;
		break;
	case ESamuraiGait::ERunning:
		result = GTag_Movement_State_Run;
		break;
	case ESamuraiGait::ESprinting:
		result = GTag_Movement_State_Sprint;
		break;
	}

	return result;
}

ESamuraiGait USamuraiAnimationBPLibrary::ConvertGaitGameplayTagToEnumGait(const FGameplayTag& gait)
{
	if (gait.MatchesTagExact(GTag_Movement_State_Walk))
	{
		return ESamuraiGait::EWalking;
	}

	if (gait.MatchesTagExact(GTag_Movement_State_Run))
	{
		return ESamuraiGait::ERunning;
	}

	if (gait.MatchesTagExact(GTag_Movement_State_Sprint))
	{
		return ESamuraiGait::ESprinting;
	}

	return ESamuraiGait::EWalking;
}

bool USamuraiAnimationBPLibrary::InRange(float value, float min, float max)
{
	return value >= min && value <= max;
}

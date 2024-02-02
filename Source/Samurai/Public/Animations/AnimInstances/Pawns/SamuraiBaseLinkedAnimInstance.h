// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/Library/SamuraiAnimationEnumLibrary.h"
#include "Animations/Library/SamuraiAnimationStructLibrary.h"
#include "SamuraiBaseLinkedAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SAMURAI_API USamuraiBaseLinkedAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float deltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float deltaSeconds) override;

protected:

	void UpdateSkeletalControls(float deltaSeconds);
	
protected: // ~ Getters ~

	UFUNCTION(BlueprintGetter, Category = "ThreadSafe - Getter Functions", meta = (BlueprintThreadSafe))
	bool IsOnPivot() const;
	
	UFUNCTION(BlueprintGetter, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	bool IsMovingForwardOrBackward() const;
	
	UFUNCTION(BlueprintGetter, BlueprintCallable, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	const float& GetBlendSpaceDirection() const;

	UFUNCTION(BlueprintGetter, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	const float& GetBlendSpacePlayRate() const;
	
	UFUNCTION(BlueprintGetter, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	const float& GetStrideBlend() const;
	
	UFUNCTION(BlueprintGetter, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	const float& GetLocomotionCycleBlendWeight() const;
	
	UFUNCTION(BlueprintGetter, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	const float& GetBlendSpaceDetailPlayRate() const;
	
	UFUNCTION(BlueprintGetter, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	const float& GetBlendSpaceDetailStartPosition() const;
	
protected: // ~ Setters ~
	
	UFUNCTION(BlueprintSetter, Category = "ThreadSafe - Setter Functions", meta = (BlueprintThreadSafe))
	void SetIsOnPivot(const bool isOnPivot);
	
	UFUNCTION(BlueprintSetter, Category = "ThreadSafe - Setter Functions", meta = (BlueprintThreadSafe))
	void SetIsMovingForwardOrBackward(const bool isForwardOrBackward);
	
	UFUNCTION(BlueprintSetter, Category = "ThreadSafe - Setter Functions", meta = (BlueprintThreadSafe))
	void SetMovementDirection(const ESamuraiMovementDirection movementDirection);
	
protected: // ~ Helpers ~
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	class USamuraiBaseAnimInstance* GetBaseMainAnimInstanceThreadSafe() const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	ESamuraiMovementDirection GetMovementDirection() const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	bool IsLocomotionStateTagActive(FGameplayTag locomotionStateTag) const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	bool IsMovementDirectionForward() const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	ESamuraiMovementDirection GetOppositeMovementDirection(const ESamuraiMovementDirection movementDirection = ESamuraiMovementDirection::EBackward) const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	ESamuraiStance GetStance() const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	const float& GetVelocityBlendInterpSpeed() const;
	
	/**
	 * Determines the locomotion state tag based on the provided blend weight.
	 *
	 * If the character quickly reaches the maximum speed, the current weight of the locomotion state
	 * might not have reached 1. If the blend weight is equal to 1.0, it returns the 'Walk' state tag; 
	 * otherwise, it returns the 'Run' state tag.
	 *
	 * @param blendWeight The current weight assigned to the state or graph. If equal to 1.0, 'Walk' state is returned; otherwise, 'Run' state is returned.
	 * @return The gameplay tag representing the determined locomotion state.
	 */
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	virtual FGameplayTag GetLocomotionStateTagFromBlendWeight(const float blendWeight) const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	void GetCycleMovementDetailAnimationSetSettingsByLocomotionState(const FGameplayTag& locomotionStateTag, FSamuraiCycleDetailAnimationSettings& outSetSettings) const;

	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	void GetCycleMovementAnimationSetByLocomotionState(const ESamuraiStance stance,
	                                                   const FGameplayTag& locomotionStateTag,
	                                                   FSamuraiCycleAnimationSet& outAnimationSet) const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	void GetCycleMovementAnimationSetSettingsByLocomotionState(const ESamuraiStance stance,
	                                                           const FGameplayTag& locomotionStateTag,
	                                                           FSamuraiCycleAnimationSetSettings& outSetSettings) const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	float GetAnimCurveClamped(FName curveName, const float bias, const float clampMin, const float clampMax) const;

	/**
	 * @brief Calculates the stride blend factor based on the given animated speed, owning speed, and stride blend range.
	 *
	 * This function computes the stride scale by dividing the owning speed in the 2D plane by the component's Z scale.
	 * It then maps this stride scale to the specified stride blend range using clamped interpolation.
	 *
	 * @param animatedSpeed The speed of the animation.
	 * @param strideBlendRange The range within which the stride blend factor is mapped.
	 * @return The calculated stride blend factor.
	 */
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	float CalculateStrideBlend(const float animatedSpeed, const FVector2D& strideBlendRange) const;

	/**
	 * @brief Calculates the play rate for the Samurai character's animation based on the owning 2D speed,
	 *        animated speed, stride blend factor, and specified play rate clamp range.
	 *
	 * This function computes the animation speed rate by dividing the owning 2D speed by the animated speed.
	 * It then calculates the play rate by dividing the animation speed rate by the stride blend factor
	 * and the component's Z scale, and finally clamps the result within the specified play rate clamp range.
	 * 
	 * @param animatedSpeed The speed of the animation.
	 * @param strideBlend The calculated stride blend factor.
	 * @param playRateClamp The clamp range for the resulting play rate.
	 * @return The calculated and clamped play rate for the animation.
	 */
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	float CalculatePlayRate(const float animatedSpeed, const float strideBlend, const FVector2D& playRateClamp) const;

	/**
	 * @brief Calculates both the stride blend factor and play rate for the Samurai character's animation
	 *        based on the given animated speed, stride blend range, and play rate clamp range.
	 *
	 * This function combines the calculation of the stride blend factor and play rate in a single call,
	 * providing a convenient way to obtain both values at once.
	 *
	 * @param animatedSpeed The speed of the animation.
	 * @param strideBlendRange The range within which the stride blend factor is mapped.
	 * @param playRateClamp The clamp range for the resulting play rate.
	 * @param outStrideBlend [out] The calculated stride blend factor to be returned.
	 * @param outPlayRate [out] The calculated and clamped play rate for the animation to be returned.
	 */
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	void CalculateStrideBlendAndPlayRate(const float animatedSpeed,
	                                     const FVector2D& strideBlendRange, const FVector2D& playRateClamp,
	                                     float& outStrideBlend, float& outPlayRate) const;

	/**
	 * @brief Calculates the movement direction based on the provided parameters and updates the output variables.
	 *
	 * @param directionAngle The angle representing the movement direction.
	 * @param dirMin The minimum angle for the valid movement direction range.
	 * @param dirMax The maximum angle for the valid movement direction range.
	 * @param buffer A buffer added to adjust the movement direction range.
	 * @param outMovementDirection Reference to an output parameter for the calculated movement direction.
	 */
	UFUNCTION(BlueprintCallable, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	void CalculateMovementDirection(const float directionAngle, const float dirMin, const float dirMax,
	                                const float buffer, ESamuraiMovementDirection& outMovementDirection) const;

	/**
	 * @brief Calculates the blend space direction based on the given direction angle.
	 *
	 * This function is responsible for determining the appropriate blend space direction
	 * based on the input direction angle. If the character is moving forward, the output
	 * direction is set directly to the input angle. If the character is not moving forward,
	 * the direction is mapped to a new range for blend space, considering movement to the left
	 * or right, and updated accordingly.
	 *
	 * @param directionAngle The angle representing the character's movement direction.
	 * @param outBlendSpaceDir Reference to a float to store the calculated blend space direction.
	 */
	UFUNCTION(BlueprintCallable, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	void CalculateBlendSpaceDirection(const float directionAngle, float& outBlendSpaceDir) const;
	
	/**
	 * @brief Calculates velocity blend based on movement direction.
	 *
	 * This function is responsible for determining the velocity blend values
	 * for forward and backward movement based on the current direction of the character.
	 * If the character is moving forward, the 'Forward' blend is set to 1.0, and 'Backward' is set to 0.0.
	 * If the character is not moving forward, the 'Forward' blend is set to 0.0, and 'Backward' is set to 1.0.
	 *
	 * @param outVelocityBlend Reference to a FSamuraiVelocityBlend struct to store the calculated blend values.
	 */
	UFUNCTION(BlueprintCallable, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	void CalculateVelocityBlend(FSamuraiVelocityBlend& outVelocityBlend);
	
protected:

	UFUNCTION(BlueprintCallable, Category = "ThreadSafe - ThreadSafe Update Functions", meta = (BlueprintThreadSafe))
	void UpdateIdleData();
	
	/**
	 * @brief Updates the animation layer data for the Samurai character based on the given direction angle.
	 *
	 * This function retrieves animation settings for the current locomotion state and stance, calculates
	 * stride blend and play rate based on the obtained settings, and determines movement direction.
	 *
	 * @param directionAngle The angle representing the direction of movement.
	 * @param blendWeight The blend weight of the state.
	 */
	UFUNCTION(BlueprintCallable, Category = "ThreadSafe - ThreadSafe Update Functions", meta = (BlueprintThreadSafe))
	void UpdateLocomotionCycleData(const float directionAngle, const float blendWeight);

	/**
	 * @brief Updates additional locomotion cycle detail data based on the given blend weight.
	 *
	 * @param blendWeight - The blend weight to determine the locomotion cycle detail.
	 */
	UFUNCTION(BlueprintCallable, Category = "ThreadSafe - ThreadSafe Update Functions", meta = (BlueprintThreadSafe))
	void UpdateLocomotionCycleDetailsData(const bool isPivot, const float blendWeight);
	
	UFUNCTION(BlueprintCallable, Category = "ThreadSafe - ThreadSafe Update Functions", meta = (BlueprintThreadSafe))
	void UpdateLocomotionCycleDetailsRunningData(const float deltaTime);
	
	UFUNCTION(BlueprintCallable, Category = "ThreadSafe - ThreadSafe Update Functions", meta = (BlueprintThreadSafe))
	void UpdateLocomotionCycleDetailsWalkingData(const float deltaTime);
	
	UFUNCTION(BlueprintCallable, Category = "ThreadSafe - ThreadSafe Update Functions", meta = (BlueprintThreadSafe))
	void DebugMessage(FString message);
	
protected:
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Selector Functions", meta = (BlueprintThreadSafe))
	class UBlendSpace* SelectCycleDetailAnimation(const ESamuraiMovementDirection movementDirection) const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Selector Functions", meta = (BlueprintThreadSafe))
	class UBlendSpace* SelectCycleAnimationPose(const ESamuraiStance stance,
	                                            const FGameplayTag locomotionStateTag) const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Selector Functions", meta = (BlueprintThreadSafe))
	class UBlendSpace* SelectCycleAnimation(const ESamuraiMovementDirection movementDirection,
											const ESamuraiStance stance, const FGameplayTag locomotionStateTag) const;
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Standing")
	FSamuraiIdleAndTurnInPlaceAnimSet IdleAndTurnInPlaceAnimSet_Standing;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Standing")
	FSamuraiCycleAnimationSets CycleAnimationSets_Standing;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Standing")
	FSamuraiCycleDetailAnimationSet CycleDetailAnimationSet_Standing;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Crouching")
	FSamuraiIdleAndTurnInPlaceAnimSet IdleAndTurnInPlaceAnimSet_Crouching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Standing")
	FSamuraiCycleAnimationSets CycleAnimationSets_Crouching;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Jumping")
	FSamuraiJumpAnimSet JumpAnimationSet;

protected:
	
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<class USamuraiBaseAnimInstance> BaseMainAnimInstance = nullptr;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BaseMainAnimInstance | Cache")
	FGameplayTag LocomotionStateTag = FGameplayTag::EmptyTag;

protected:

	/* Locomotion Cycle */
	
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion Cycle | BlendSpace Data")
	float BlendSpaceDirection = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion Cycle | BlendSpace Data")
	float BlendSpacePlayRate = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion Cycle | BlendSpace Data")
	float StrideBlend = 0.f;

	/* Locomotion Cycle - State Machine Data */
	
	UPROPERTY(BlueprintReadWrite, Category = "Locomotion Cycle | State Machine Data")
	uint8 bIsMovingForwardOrBackward : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "Locomotion Cycle | State Machine Data")
	ESamuraiMovementDirection MovementDirection = ESamuraiMovementDirection::EForward;
	
	UPROPERTY(BlueprintReadWrite, Category = "Locomotion Cycle | State Machine Data")
	float LocomotionCycleBlendWeight = 0.f;
	
	/* Locomotion Details */
	
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion Details | BlendSpace Data")
	float BlendSpaceDetailPlayRate = 1.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion Details | BlendSpace Data")
	float BlendSpaceDetailStartPosition = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Locomotion Details | BlendSpace Data | Pivot")
	uint8 bIsOnPivot : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "Locomotion Details | BlendSpace Data | Pivot")
	uint8 bIsFirstPivot : 1;

	UPROPERTY(BlueprintReadWrite, Category = "Locomotion Details | BlendSpace Data | Pivot")
	float TimePendingOutPivot = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Locomotion Details | BlendSpace Data | Pivot")
	float MaxPendingPivotTime = 1.5f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Locomotion Details | BlendSpace Data | Pivot")
	FSamuraiVelocityBlend VelocityBlend;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Skeletal Controls")
	float Enable_FootIK_L = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Skeletal Controls")
	float Enable_FootIK_R = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Skeletal Controls")
	float PelvisAlpha = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Skeletal Controls")
	float FootLock_L = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Skeletal Controls")
	float FootLock_R = 0.f;

protected:
	
	// Cache the 2D speed of the owner.
	float OwningSpeed2D = 0.f;

	// Cache the z scale of the owner.
	float OwningComponentZScale = 1.f;

	// Cache the local velocity direction with no offset of the owner.
	ESamuraiCardinalDirection LocalVelocityDirectionNoOffset = ESamuraiCardinalDirection::EForward;

	// Flag indicating whether the owner is currently crouching.
	uint8 bIsCrouching : 1;
};

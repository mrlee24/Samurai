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

protected: // ~ Utils ~

	bool IsMovingForwardOrBackward(const ESamuraiCardinalDirection direction) const;
	
protected: // ~ Getters ~

	UFUNCTION(BlueprintGetter, Category = "ThreadSafe - Getter Functions", meta = (BlueprintThreadSafe))
	uint8 IsOnPivot() const;
	
	UFUNCTION(BlueprintGetter, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	uint8 IsMovingForwardOrBackward() const;

protected: // ~ Setters ~
	
	UFUNCTION(BlueprintSetter, Category = "ThreadSafe - Setter Functions", meta = (BlueprintThreadSafe))
	void SetPivotMovementDirection(const ESamuraiMovementDirection pivotMovementDirection);
	
	UFUNCTION(BlueprintSetter, Category = "ThreadSafe - Setter Functions", meta = (BlueprintThreadSafe))
	void SetIsOnPivot(const bool isOnPivot);
	
	UFUNCTION(BlueprintSetter, Category = "ThreadSafe - Setter Functions", meta = (BlueprintThreadSafe))
	void SetIsMovingForwardOrBackward(const bool isForwardOrBackward);
	
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
	 * Calculates the movement direction based on the provided parameters and updates the output variables.
	 *
	 * @param directionAngle The angle representing the movement direction.
	 * @param dirMin The minimum angle for the valid movement direction range.
	 * @param dirMax The maximum angle for the valid movement direction range.
	 * @param buffer A buffer added to adjust the movement direction range.
	 * @param outMovementDirection Reference to an output parameter for the calculated movement direction.
	 * @param outBlendSpaceDir Reference to an output parameter for the calculated blend space direction.
	 */
	UFUNCTION(BlueprintCallable, Category = "ThreadSafe - Helper Functions", meta = (BlueprintThreadSafe))
	void CalculateMovementDirection(const float directionAngle, const float dirMin, const float dirMax,
	                                const float buffer, ESamuraiMovementDirection& outMovementDirection,
	                                float& outBlendSpaceDir) const;

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
	void UpdateLocomotionCycleDetailData(const bool isPivot, const float blendWeight);
	
protected:
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Selector Functions", meta = (BlueprintThreadSafe))
	class UBlendSpace* SelectCycleDetailAnimation(const ESamuraiMovementDirection movementDirection) const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Selector Functions", meta = (BlueprintThreadSafe))
	class UBlendSpace* SelectCycleAnimationPose(const ESamuraiStance stance,
	                                            const FGameplayTag& locomotionStateTag) const;
	
	UFUNCTION(BlueprintPure, Category = "ThreadSafe - Selector Functions", meta = (BlueprintThreadSafe))
	class UBlendSpace* SelectCycleAnimation(const ESamuraiMovementDirection movementDirection,
											const ESamuraiStance stance,
	                                        const FGameplayTag& locomotionStateTag) const;
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Standing")
	FSamuraiIdleAndTurnInPlaceAnimSet IdleAndTurnInPlaceAnimSet_Standing;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Standing")
	TMap<FGameplayTag, FSamuraiCycleAnimationSet> CycleAnimationSet_Standing;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Standing")
	FSamuraiCycleDetailAnimationSet CycleDetailAnimationSet_Standing;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Crouching")
	FSamuraiIdleAndTurnInPlaceAnimSet IdleAndTurnInPlaceAnimSet_Crouching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Standing")
	TMap<FGameplayTag, FSamuraiCycleAnimationSet> CycleAnimationSet_Crouching;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Set | Jumping")
	FSamuraiJumpAnimSet JumpAnimationSet;

protected:
	
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<class USamuraiBaseAnimInstance> BaseMainAnimInstance = nullptr;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "BaseMainAnimInstance | Cache")
	FGameplayTag LocomotionStateTag = FGameplayTag::EmptyTag;

protected:
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	float BlendSpaceDirection = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	float StrideBlend = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	float PlayRate = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	ESamuraiMovementDirection MovementDirection = ESamuraiMovementDirection::EForward;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	ESamuraiMovementDirection PivotMovementDirection = ESamuraiMovementDirection::EForward;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	float LocomotionCycleBlendWeight = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	float LocomotionCycleDetailPlayRate = 1.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	float LocomotionCycleDetailStartPosition = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	uint8 bIsOnPivot : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	uint8 bIsMovingForwardOrBackward : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	float PivotTime = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Layer | Cycle")
	float MaxThresholdPivotTime = 1.5f;
	
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

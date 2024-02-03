// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "Animations/Library/SamuraiAnimationStructLibrary.h"
#include "SamuraiBaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SAMURAI_API USamuraiBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected: // Native Blueprint Functions.

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float deltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float deltaSeconds) override;

protected:
	
	virtual void PreUpdateMovementData(float deltaSeconds);
	virtual void PreUpdateAbilityData();
	virtual void PreUpdateAnimationData();

protected: // Native Blueprint ThreadSafe Update Function
	
	UFUNCTION(meta = (BluerpintThreadSafe), Category = "ThreadSafe Update Functions")
	void UpdateRotationData();
	
	UFUNCTION(meta = (BluerpintThreadSafe), Category = "ThreadSafe Update Functions")
	void UpdateLocationData(const float deltaSeconds);
	
	UFUNCTION(meta = (BluerpintThreadSafe), Category = "ThreadSafe Update Functions")
	void UpdateAccelerationData();
	
	UFUNCTION(meta = (BluerpintThreadSafe), Category = "ThreadSafe Update Functions")
	void UpdateVelocityData(const float deltaSeconds);
	
	UFUNCTION(meta = (BluerpintThreadSafe), Category = "ThreadSafe Update Functions")
	void UpdatePawnStateData(const float deltaSeconds);
	
	UFUNCTION(meta = (BluerpintThreadSafe), Category = "ThreadSafe Update Functions")
	void UpdateBlendWeightData(const float deltaSeconds);
	
	UFUNCTION(meta = (BluerpintThreadSafe), Category = "ThreadSafe Update Functions")
	void UpdateAimingData();
	
	UFUNCTION(meta = (BluerpintThreadSafe), Category = "ThreadSafe Update Functions")
	void UpdateJumpFallData();
	
	UFUNCTION(meta = (BluerpintThreadSafe), Category = "ThreadSafe Update Functions")
	void UpdateWallDetectionHeuristic();
	
	UFUNCTION(meta = (BluerpintThreadSafe), Category = "ThreadSafe Update Functions")
	void UpdateRootYawOffset(const float deltaSeconds);

protected:
	
	UFUNCTION(BlueprintCallable, meta = (BluerpintThreadSafe), Category = "Turn In Place")
	void SetRootYawOffset(const float rootYawOffset);
	
	UFUNCTION(BlueprintCallable, meta = (BluerpintThreadSafe), Category = "Turn In Place")
	void ProcessTurnYawCurve();

protected:

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe), Category = "Pivot")
	bool IsMovingPerpendicularToInitialPivot() const;

public: // Getters ThreadSafe
	
	UFUNCTION(BlueprintGetter, meta = (BlueprintThreadSafe), Category = "Getters")
	const float& GetDisplacementSpeed() const;
	
	UFUNCTION(BlueprintGetter, meta = (BlueprintThreadSafe), Category = "Getters")
	float GetSpeed2D() const;
	
	UFUNCTION(BlueprintGetter, meta = (BlueprintThreadSafe), Category = "Getters")
	const FSamuraiPreUpdateData& GetPreUpdateData() const;
	
	UFUNCTION(BlueprintGetter, meta = (BlueprintThreadSafe), Category = "Getters")
	const FSamuraiRotationData& GetRotationData() const;
	
	UFUNCTION(BlueprintGetter, meta = (BlueprintThreadSafe), Category = "Getters")
	const FSamuraiVelocityData& GetVelocityData() const;
	
	UFUNCTION(BlueprintGetter, meta = (BlueprintThreadSafe), Category = "Getters")
	const FSamuraiPawnStateData& GetPawnStateDate() const;
	
protected:
	
	UPROPERTY(BlueprintReadWrite, Category = "PreUpdateData")
	FSamuraiPreUpdateData PreUpdateData;
	
	UPROPERTY(BlueprintReadWrite, Category = "Rotation Data")
	FSamuraiRotationData RotationData;
	
	UPROPERTY(BlueprintReadWrite, Category = "Location Data")
	FSamuraiLocationData LocationData;
	
	UPROPERTY(BlueprintReadWrite, Category = "Acceleration Data")
	FSamuraiAccelerationData AccelerationData;
	
	UPROPERTY(BlueprintReadWrite, Category = "Velocity Data")
	FSamuraiVelocityData VelocityData;

	UPROPERTY(BlueprintReadWrite, Category = "Pawn State Data")
	FSamuraiPawnStateData PawnStateData;

	UPROPERTY(BlueprintReadWrite, Category = "Locomotion SM Data")
	FSamuraiLocomotionSMData LocomotionSMData;

	UPROPERTY(BlueprintReadWrite, Category = "Blend Weight Data")
	FSamuraiBlendWeightData BlendWeightData;

	UPROPERTY(BlueprintReadWrite, Category = "Aiming Data")
	FSamuraiAimingData AimingData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Data")
	FSamuraiSettingsData SettingsData;

	UPROPERTY(BlueprintReadWrite, Category = "Linked Layer Data")
	FSamuraiLinkedLayerData LinkedLayerData;

	UPROPERTY(BlueprintReadWrite, Category = "Turn In Place Data")
	FSamuraiTurnInPlaceData TurnInPlaceData;

	UPROPERTY(BlueprintReadWrite, Category = "SamuraiBaseAnimInstance")
	uint8 bIsFirstUpdate : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SamuraiBaseAnimInstance")
	uint8 bEnableControlRig : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SamuraiBaseAnimInstance")
	uint8 bUseFootPlacement : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SamuraiBaseAnimInstance")
	uint8 bEnableRootYawOffset : 1;

protected: // Components
	
	TScriptInterface<class IGameplayTagAssetInterface> GameplayTagInterface;
	TScriptInterface<class ISamuraiMovableInterface> MovableInterface;

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<class APawn> OwningPawn = nullptr;
	
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<class UPawnMovementComponent> PawnMovementComponent = nullptr;
	
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<class USamuraiLocomotionComponent> LocomotionComponent = nullptr;
	
};

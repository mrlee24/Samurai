// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SamuraiAnimationEnumLibrary.h"
#include "SamuraiCurveNameLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayTagContainer.h"
#include "Animations/Components/SamuraiLocomotionEnumLibrary.h"
#include "SamuraiAnimationStructLibrary.generated.h"

#pragma region : BaseAnimInstance
USTRUCT(BlueprintType)
struct FSamuraiRotationData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiRotationData")
	FRotator WorldRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, Category = "SamuraiRotaitonData")
	float YawDeltaSinceLastUpdate = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiRotaitonData")
	float AdditiveLeanAngle = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "SamuraiRotaitonData")
	float YawDeltaSpeed = 0.f;
};

USTRUCT(BlueprintType)
struct FSamuraiLocationData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiLocationData")
	FVector WorldLocation = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiLocationData")
	float DisplacementSinceLastUpdate = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiLocationData")
	float DisplacementSpeed = 0.f;
};

USTRUCT(BlueprintType)
struct FSamuraiMovementDirectionBlending
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "SamuraiMovementDirectionBlending")
	float Forward = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiMovementDirectionBlending")
	float Backward = 0.f;
};

USTRUCT(BlueprintType)
struct FSamuraiVelocityData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiVelocityData")
	FVector WorldVelocity = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiVelocityData")
	FVector LocalVelocity2D = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiVelocityData")
	float LocalVelocityDirectionAngle = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiVelocityData")
	float LocalVelocityDirectionAngleWithOffset = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "SamuraiVelocityData")
	ESamuraiCardinalDirection LocalVelocityDirection = ESamuraiCardinalDirection::EForward;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiVelocityData")
	ESamuraiCardinalDirection LocalVelocityDirectionNoOffset = ESamuraiCardinalDirection::EForward;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiVelocityData")
	uint8 bHasVelocity : 1;
};

USTRUCT(BlueprintType)
struct FSamuraiAccelerationData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiAccelerationData")
	FVector LocalAcceleration2D = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiAccelerationData")
	uint8 bHasAcceleration : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiAccelerationData")
	FVector PivotDirection2D = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FSamuraiPawnStateData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	float TimeSinceUsedWeapon = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	float TimeToJumpApex = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	uint8 bIsOnGround : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	uint8 bIsCrouching : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	uint8 bIsAiming : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	uint8 bIsCrouchStateChanged : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	uint8 bIsAimStateChanged : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	uint8 bWasAimLastUpdate : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	uint8 bIsJumping : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	uint8 bIsFalling : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	uint8 bIsPivot : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPawnStateData")
	uint8 bIsRunningIntoWall : 1;
};

USTRUCT(BlueprintType)
struct FSamuraiLocomotionSMData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiLocomotionSMData")
	ESamuraiCardinalDirection StartDirection = ESamuraiCardinalDirection::EForward;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiLocomotionSMData")
	ESamuraiCardinalDirection PivotInitialDirection = ESamuraiCardinalDirection::EForward;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiLocomotionSMData")
	float LastPivotTime = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiLocomotionSMData")
	ESamuraiCardinalDirection CardinalDirectionFromAcceleration = ESamuraiCardinalDirection::EForward;
};

USTRUCT(BlueprintType)
struct FSamuraiBlendWeightData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiBlendWeightData")
	float UpperBodyDynamicAdditiveWeight = 0.f;
};

USTRUCT(BlueprintType)
struct FSamuraiAimingData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiAimingData")
	float AimPitch = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiAimingData")
	float AimYaw = 0.f;
};

USTRUCT(BlueprintType)
struct FSamuraiSettingsData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SamuraiSettingsData")
	float CardinalDirectionDeadZone = 5.f;
};

USTRUCT(BlueprintType)
struct FSamuraiLinkedLayerData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiLinkedLayerData")
	uint8 bIsLinkedLayerChanged : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiLinkedLayerData")
	TObjectPtr<class UAnimInstance> LastLinkedLayer = nullptr;
};

USTRUCT(BlueprintType)
struct FSamuraiTurnInPlaceData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiTurnInPlaceData")
	float RootYawOffset = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiTurnInPlaceData")
	FFloatSpringState RootYawOffsetSpringState;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiTurnInPlaceData")
	ESamuraiRootYawOffsetMode RootYawOffsetMode = ESamuraiRootYawOffsetMode::EAccumulate;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiTurnInPlaceData")
	float TurnYawCurveValue = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiTurnInPlaceData")
	FVector2D RootYawOffsetAngleClamp = FVector2D::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiTurnInPlaceData")
	FVector2D RootYawOffsetAngleClampCrouched = FVector2D::ZeroVector;
};

USTRUCT(BlueprintType)
struct FSamuraiPreUpdateAbilityData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	uint8 bIsAiming : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	uint8 bIsUsingItem : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	uint8 bIsDodging : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	uint8 bIsJumping : 1;
};

USTRUCT(BlueprintType)
struct FSamuraiPreUpdateAnimationData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateAnimationData")
	FGameplayTag LocomotionStateTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateAnimationData")
	ESamuraiRotationMode RotationMode = ESamuraiRotationMode::EVelocityDirection;
};

USTRUCT(BlueprintType)
struct FSamuraiPreUpdateData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	FRotator ActorRotation = FRotator::ZeroRotator;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	FVector ActorLocation = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	FVector ActorVelocity = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	FVector ActorAcceleration = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	float AimPitch = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	float GravityZ = 0.f;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	uint8 bIsCrouching : 1;

	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	uint8 bIsFalling : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	uint8 bIsMovingOnGround : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	uint8 bIsSwimming : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	uint8 bIsFlying : 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	FSamuraiPreUpdateAbilityData AbilityData;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	FSamuraiPreUpdateAnimationData AnimationData;
	
	UPROPERTY(BlueprintReadWrite, Category = "SamuraiPreUpdateData")
	uint8 bIsAnyMontagePlaying : 1;
};
#pragma endregion
#pragma region : MotionSet

USTRUCT(BlueprintType)
struct FSamuraiAimAnimSet
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiAimAnimSet")
	TObjectPtr<class UAnimSequence> IdleAim = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiAimAnimSet")
	TObjectPtr<class UAimOffsetBlendSpace> AimOffset = nullptr;
};

USTRUCT(BlueprintType)
struct FSamuraiRotatingAnimSet
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiTurnInPlaceAnimSet")
	TObjectPtr<class UAnimSequence> Right_90 = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiTurnInPlaceAnimSet")
	TObjectPtr<class UAnimSequence> Left_90 = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiTurnInPlaceAnimSet")
	TObjectPtr<class UAnimSequence> Right_180 = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiTurnInPlaceAnimSet")
	TObjectPtr<class UAnimSequence> Left_180 = nullptr;
};

USTRUCT(BlueprintType)
struct FSamuraiIdleAndTurnInPlaceAnimSet
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiIdleAnimSet")
	TObjectPtr<class UAnimSequence> Idle = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiIdleAnimSet")
	TArray<TObjectPtr<class UAnimSequence>> IdleBreaks;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiIdleAnimSet")
	TObjectPtr<class UAnimSequence> IdleEntry = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiIdleAnimSet")
	TObjectPtr<class UAnimSequence> IdleExit = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiIdleAnimSet")
	FSamuraiRotatingAnimSet TurnInPlaceAnimSet;
};

USTRUCT(BlueprintType)
struct FSamuraiCycleDetailAnimationSettings
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleDetailAnimationSettings")
	float PlayRate = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleDetailAnimationSettings")
	float StartPosition = 0.f;
};

USTRUCT(BlueprintType)
struct FSamuraiCycleDetailAnimationSet
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleDetailAnimationSet")
	TObjectPtr<class UBlendSpace> Forward_Details = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleDetailAnimationSet")
	TObjectPtr<class UBlendSpace> Backward_Details = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleDetailAnimationSet")
	TMap<FGameplayTag, FSamuraiCycleDetailAnimationSettings> AnimationSetSettings;
};

USTRUCT(BlueprintType)
struct FSamuraiCycleAnimationSetSettings
{
	GENERATED_BODY()

public:
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleAnimationSetSettings")
	float AnimatedSpeed = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleAnimationSetSettings")
	FVector2D StrideBlendRange = FVector2D(.2f, 1.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleAnimationSetSettings")
	FVector2D PlayRateClamp = FVector2D(0.f, 1.f);
};

USTRUCT(BlueprintType)
struct FSamuraiCycleAnimationSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleAnimationSet")
	TObjectPtr<class UBlendSpace> Poses = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleAnimationSet")
	TObjectPtr<class UBlendSpace> Forward = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleAnimationSet")
	TObjectPtr<class UBlendSpace> Backward = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiCycleAnimationSet")
	FSamuraiCycleAnimationSetSettings AnimationSetSettings;
};

USTRUCT(BlueprintType)
struct FSamuraiJumpAnimSet
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiJumpAnimSet")
	TObjectPtr<class UAnimSequence> Start = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiJumpAnimSet")
	TObjectPtr<class UAnimSequence> Apex = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiJumpAnimSet")
	TObjectPtr<class UAnimSequence> FallLand = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiJumpAnimSet")
	TObjectPtr<class UAnimSequence> RecoveryAdditive = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiJumpAnimSet")
	TObjectPtr<class UAnimSequence> StartLoop = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiJumpAnimSet")
	TObjectPtr<class UAnimSequence> FallLoop = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SamuraiJumpAnimSet")
	FName DistanceCurveName = NAME_GroundDistance;
};

#pragma endregion
#pragma region : BlendMaskSet

USTRUCT(BlueprintType)
struct FSamuraiBlendMaskAdditiveSettings
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SamuraiBlendMaskAnimSet")
	ESamuraiBlendMaskAdditiveMode BlendMaskAdditiveMode = ESamuraiBlendMaskAdditiveMode::EMeshSpace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SamuraiBlendMaskAnimSet")
	float BlendMaskAdditive = 0.f;
};

USTRUCT(BlueprintType)
struct FSamuraiBlendMaskAnimSet
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SamuraiBlendMaskAnimSet")
	TObjectPtr<class UAnimSequence> PoseOverride = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SamuraiBlendMaskAnimSet")
	float BlendMaskPoseTime = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SamuraiBlendMaskAnimSet")
	float BlendWeight = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SamuraiBlendMaskAnimSet")
	FSamuraiBlendMaskAdditiveSettings BlendMaskAdditiveSettings;
};

USTRUCT(BlueprintType)
struct FSamuraiBlendMaskSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SamuraiBlendMaskSettings")
	TMap<ESamuraiBlendMaskMode, FSamuraiBlendMaskAnimSet> BlendMaskModes;
};

#pragma endregion 
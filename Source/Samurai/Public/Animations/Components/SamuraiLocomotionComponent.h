// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SamuraiLocomotionEnumLibrary.h"
#include "SamuraiLocomotionStructLibrary.h"
#include "Animations/Library/SamuraiAnimationEnumLibrary.h"
#include "Components/ActorComponent.h"
#include "SamuraiLocomotionComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSamuraiLocomotionComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSamuraiLocomotionStateDelegate, const FSamuraiLocomotionState&, locomotionState);
DECLARE_MULTICAST_DELEGATE_OneParam(FSamuraiLocomotionStateNativeDelegate, const FSamuraiLocomotionState& /* locomotionState */);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSamuraiLocomotionStateTagDelegate, const FGameplayTag&, locomotionStateTag);
DECLARE_MULTICAST_DELEGATE_OneParam(FSamuraiLocomotionStateNativeTagDelegate, const FGameplayTag& /* locomotionStateTag */);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSamuraiStanceDelegate, const ESamuraiStance, stance);
DECLARE_MULTICAST_DELEGATE_OneParam(FSamuraiStanceNativeDelegate, const ESamuraiStance /* stance */)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSamuraiRotationModeDelegate, const ESamuraiRotationMode, rotationMode);
DECLARE_MULTICAST_DELEGATE_OneParam(FSamuraiRotationModeNativeDelegate, const ESamuraiRotationMode /* rotationMode */)

UCLASS(ClassGroup=(Samurai), meta=(BlueprintSpawnableComponent))
class SAMURAI_API USamuraiLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

    // Sets default values for this component's properties
    USamuraiLocomotionComponent();

protected: // UActorComponent Interface
    virtual void BeginPlay() override;
    virtual void TickComponent(
        float deltaTime,
        ELevelTick tickType,
        FActorComponentTickFunction* thisTickFunction) override;

    virtual void PostInitProperties() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

    UFUNCTION(BlueprintCallable, Category = "Samurai Locomotion Component | Movement System")
    FGameplayTag GetAllowedLocomotionStateTag() const;
    
    UFUNCTION(BlueprintCallable, Category = "Samurai Locomotion Component | Movement System")
    virtual FGameplayTag GetActualLocomotionStateTag(const FGameplayTag& allowedLocomotionStateTag) const;

protected:

    virtual void UpdateEssentialValues(float deltaTime);
    virtual void UpdateLocomotion();
    virtual void UpdateGroundedRotation(float deltaTime);
    virtual void ForceUpdateLocomotion();

public:

    float GetLocomotionStateSpeed(const FGameplayTag& stateTag) const;
    bool IsDefaultLocomotionState(const FGameplayTag& stateTag) const;
    bool IsSprinting() const;

protected:

    /** Replication */
    UFUNCTION(Category = "ALS|Replication")
    void OnRep_RotationMode(ESamuraiRotationMode rotationMode) const;

public:
    
    UFUNCTION(BlueprintCallable, Category = "Samurai Locomotion Component | Character States")
    void SetLocomotionStateTag(const FGameplayTag& newLocomotionStateTag, const bool shouldForce = false);
    
    UFUNCTION(BlueprintCallable, Category = "Samurai Locomotion Component | Character States")
    void SetStance(ESamuraiStance newStance, const bool shouldForce = false);
    
    UFUNCTION(BlueprintCallable, Category = "Samurai Locomotion Component | Character States")
    void SetRotationMode(ESamuraiRotationMode newRotationMode, const bool shouldForce = false);

private:

    void BroadcastAllowedLocomotionStateChanged(const FSamuraiLocomotionState& locomotionState) const;
    void BroadcastLocomotionStateChanged(const FGameplayTag& locomotionStateTag) const;
    void BroadcastCurrentStanceChanged(const ESamuraiStance stance) const;
    void BroadcastCurrentRotationMode(const ESamuraiRotationMode rotationMode) const;

public: // ~ Getters ~
    
    UFUNCTION(BlueprintGetter, Category = "Samurai Locomotion Component | Character States")
    const FGameplayTag& GetLocomotionStateTag() const;

    UFUNCTION(BlueprintGetter, Category = "Samurai Locomotion Component | Character States")
    const FGameplayTag& GetDesiredLocomotionStateTag() const;
    
    UFUNCTION(BlueprintGetter, Category = "Samurai Locomotion Component | Character States")
    ESamuraiStance GetStance() const;
    
    UFUNCTION(BlueprintGetter, Category = "Samurai Locomotion Component | Character States")
    ESamuraiStance GetDesiredStance() const;
    
    UFUNCTION(BlueprintGetter, Category = "Samurai Locomotion Component | Character States")
    ESamuraiRotationMode GetRotationMode() const;
    
    UFUNCTION(BlueprintGetter, Category = "Samurai Locomotion Component | Character States")
    ESamuraiRotationMode GetDesiredRotationMode() const;
    
    UFUNCTION(BlueprintGetter, Category = "Samurai Locomotion Component | Essential Data")
    const FVector& GetReplicatedCurrentAcceleration() const;

public: // ~ Setters ~

    UFUNCTION(BlueprintSetter, Category = "Samurai Locomotion Component | Input")
    void SetDesiredLocomotionStateTag(const FGameplayTag& newLocomotionStateTag);

    UFUNCTION(Server, Reliable, WithValidation, Category= "Samurai Locomotion Component | Input")
    void Server_SetDesiredLocomotionStateTag(const FGameplayTag& newLocomotionStateTag);

    UFUNCTION(BlueprintSetter, Category = "Samurai Locomotion Component | Input")
    void SetDesiredStance(ESamuraiStance newStance);

    UFUNCTION(Server, Reliable, WithValidation, Category = "Samurai Locomotion Component | Input")
    void Server_SetDesiredStance(ESamuraiStance newStance);
    
    UFUNCTION(Server, Reliable, WithValidation, Category = "Samurai Locomotion Component | Input")
    void Server_SetCurrentRotationMode(ESamuraiRotationMode newRotationMode, const bool shouldForce = false);
        
    UFUNCTION(BlueprintSetter, Category = "Samurai Locomotion Component | Input")
    void SetDesiredRotationMode(ESamuraiRotationMode newRotationMode);
    
    UFUNCTION(Server, Reliable, WithValidation, Category = "Samurai Locomotion Component | Input")
    void Server_SetDesiredRotationMode(ESamuraiRotationMode newRotationMode);

protected: // ~ Utils ~

    void SmoothCharacterRotation(const FRotator& target, const float targetInterpSpeed,
                                 const float actorInterpSpeed, const float deltaTime);

public:

    UPROPERTY(BlueprintAssignable, Category = "Samurai Locomotion Component | Events")
    FSamuraiLocomotionStateDelegate OnAllowedLocomotionStateChanged;
    FSamuraiLocomotionStateNativeDelegate OnAllowedLocomotionStateChangedNative;
    
    UPROPERTY(BlueprintAssignable, Category = "Samurai Locomotion Component | Events")
    FSamuraiLocomotionStateTagDelegate OnLocomotionTagChanged;
    FSamuraiLocomotionStateNativeTagDelegate OnCurrentLocomotionTagChangedNative;
    
    UPROPERTY(BlueprintAssignable, Category = "Samurai Locomotion Component | Events")
    FSamuraiStanceDelegate OnStanceChanged;
    FSamuraiStanceNativeDelegate OnStanceChangedNative;
    
    UPROPERTY(BlueprintAssignable, Category = "Samurai Locomotion Component | Events")
    FSamuraiRotationModeDelegate OnRotationModeChanged;
    FSamuraiRotationModeNativeDelegate OnRotationModeChangedNative;

protected: // Setup
    
    UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Samurai Locomotion Component | Setup")
    FGameplayTag DesiredLocomotionTag = FGameplayTag::EmptyTag;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Samurai Locomotion Component | Setup")
    TMap<FGameplayTag, FSamuraiLocomotionState> LocomotionStates;
    
    UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Samurai Locomotion Component | Setup")
    ESamuraiStance DesiredStance = ESamuraiStance::EStanding;
    
    UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Samurai Locomotion Component | Setup")
    ESamuraiRotationMode DesiredRotationMode = ESamuraiRotationMode::EVelocityDirection;


protected: // ~ Runtime Essential Data ~

    UPROPERTY(BlueprintReadonly, Category = "Samurai Locomotion Component | Runtime Essential Data")
    FRotator LastVelocityRotation;
    
    UPROPERTY(BlueprintReadonly, Replicated, Category = "Samurai Locomotion Component | Runtime Essential Data")
    FRotator ControlRotation = FRotator::ZeroRotator;
    
    UPROPERTY(BlueprintReadonly, Replicated, Category = "Samurai Locomotion Component | Runtime Essential Data")
    float Speed2D = 0.f;

    // ~ Replicated Essential Information ~
    UPROPERTY(BlueprintReadonly, Replicated, Category = "Samurai Locomotion Component | Runtime Essential Data")
    float EasedMaxAcceleration = 0.0f;

    UPROPERTY(BlueprintReadonly, Replicated, Category = "Samurai Locomotion Component | Runtime Essential Data")
    FVector ReplicatedCurrentAcceleration = FVector::ZeroVector;

    UPROPERTY(BlueprintReadonly, Replicated, Category = "Samurai Locomotion Component | Runtime Essential Data")
    FRotator ReplicatedControlRotation = FRotator::ZeroRotator;

protected: // ~ Runtime Inputs

    UPROPERTY(BlueprintReadonly, Category = "Samurai Locomotion Component | Runtime Inputs")
    FGameplayTag LocomotionStateTag = FGameplayTag::EmptyTag;
    
    UPROPERTY(BlueprintReadonly, Category = "Samurai Locomotion Component | Runtime Inputs")
    ESamuraiStance Stance = ESamuraiStance::EStanding;
    
    UPROPERTY(BlueprintReadonly,  ReplicatedUsing = OnRep_RotationMode, Category = "Samurai Locomotion Component | Runtime Inputs")
    ESamuraiRotationMode RotationMode = ESamuraiRotationMode::ELookingDirection;

protected: // ~ Rotation System 

    UPROPERTY(BlueprintReadOnly, Category = "ALS|Rotation System")
    FRotator TargetRotation = FRotator::ZeroRotator;

protected:

    UPROPERTY(Transient, DuplicateTransient)
    TObjectPtr<class APawn> OwningPawn = nullptr;

    UPROPERTY(Transient, DuplicateTransient)
    TObjectPtr<UPawnMovementComponent> PawnMovementComponent = nullptr;

    TScriptInterface<class ISamuraiMovableInterface> MovableInterface = nullptr;

protected:
    
    FGameplayTag LocomotionWalkStateTag = FGameplayTag::EmptyTag;
    FGameplayTag LocomotionRunStateTag = FGameplayTag::EmptyTag;
    FGameplayTag LocomotionSprintStateTag = FGameplayTag::EmptyTag;

    float AnimatedWalkSpeed = 0.f;
    float AnimatedRunSpeed = 0.f;
    float AnimatedSprintSpeed = 0.f;
};

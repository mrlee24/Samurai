// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/Library/SamuraiAnimationStructLibrary.h"
#include "DataAssets/SamuraiDataAsset.h"
#include "UObject/Object.h"
#include "SamuraiBlendMaskDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class SAMURAI_API USamuraiBlendMaskDataAsset : public USamuraiDataAsset
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	FSamuraiBlendMaskSettings BlendMaskSettings;

public:

	// Helper Functions
	
	UFUNCTION(BlueprintPure, Category = "Animation Selection | Helper Functions")
	FSamuraiBlendMaskAnimSet GetBlendMaskAnimSet(const ESamuraiBlendMaskMode mode) const;
	
	UFUNCTION(BlueprintPure, Category = "Animation Selection | Helper Functions")
	float GetBlendMaskPoseTime(const ESamuraiBlendMaskMode mode) const;
	
	UFUNCTION(BlueprintPure, Category = "Animation Selection | Helper Functions")
	float GetBlendWeight(const ESamuraiBlendMaskMode mode) const;
	
	UFUNCTION(BlueprintPure, Category = "Animation Selection | Helper Functions")
	FSamuraiBlendMaskAdditiveSettings GetBlendMaskAdditiveSettings(const ESamuraiBlendMaskMode mode) const;

	UFUNCTION(BlueprintPure, Category = "Animation Selection | Helper Functions")
	ESamuraiBlendMaskAdditiveMode GetBlendMaskAdditiveMode(const ESamuraiBlendMaskMode mode) const;
	
	UFUNCTION(BlueprintPure, Category = "Animation Selection | Helper Functions")
	float GetBlendMaskAdditive(const ESamuraiBlendMaskMode mode) const;
};

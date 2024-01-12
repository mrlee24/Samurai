// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/DataAssets/SamuraiBlendMaskDataAsset.h"

FSamuraiBlendMaskAnimSet USamuraiBlendMaskDataAsset::GetBlendMaskAnimSet(const ESamuraiBlendMaskMode mode) const
{
	if (mode == ESamuraiBlendMaskMode::ENone)
	{
		return FSamuraiBlendMaskAnimSet();
	}

	return BlendMaskSettings.BlendMaskModes.FindChecked(mode);
}

float USamuraiBlendMaskDataAsset::GetBlendMaskPoseTime(const ESamuraiBlendMaskMode mode) const
{
	return GetBlendMaskAnimSet(mode).BlendMaskPoseTime;
}

float USamuraiBlendMaskDataAsset::GetBlendWeight(const ESamuraiBlendMaskMode mode) const
{
	return GetBlendMaskAnimSet(mode).BlendWeight;
}

FSamuraiBlendMaskAdditiveSettings USamuraiBlendMaskDataAsset::GetBlendMaskAdditiveSettings(
	const ESamuraiBlendMaskMode mode) const
{
	return GetBlendMaskAnimSet(mode).BlendMaskAdditiveSettings;
}

ESamuraiBlendMaskAdditiveMode USamuraiBlendMaskDataAsset::GetBlendMaskAdditiveMode(
	const ESamuraiBlendMaskMode mode) const
{
	return GetBlendMaskAdditiveSettings(mode).BlendMaskAdditiveMode;
}

float USamuraiBlendMaskDataAsset::GetBlendMaskAdditive(const ESamuraiBlendMaskMode mode) const
{
	return GetBlendMaskAdditiveSettings(mode).BlendMaskAdditive;
}

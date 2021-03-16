// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSTemplate.h"
#include "BaseSingleton.h"

USoundBase * UBaseSingleton::GetFootstepSound(EPhysicalSurface SurfaceType)
{
	UBaseSingleton * Singleton = Cast<UBaseSingleton>(GEngine->GameSingleton);
	if (Singleton)
	{
		return Singleton->SurfaceTypes[SurfaceType].FootstepSound.LoadSynchronous();
	}

	return NULL;
}

void UBaseSingleton::GetImpactAssets(EPhysicalSurface SurfaceType, USoundBase *& OutSound, UParticleSystem *& OutEmitter, UMaterialInterface *& OutDecal, float & OutDecalSize)
{
	UBaseSingleton * Singleton = Cast<UBaseSingleton>(GEngine->GameSingleton);
	if (Singleton)
	{
		FSurfaceTypeProperties & Properties = Singleton->SurfaceTypes[SurfaceType];
		OutSound = Properties.ImpactSound.LoadSynchronous();
		OutEmitter = Properties.ImpactEffect.LoadSynchronous();
		OutDecal = Properties.ImpactDecal.LoadSynchronous();
		OutDecalSize = Properties.MaxDecalSize;
	}
	else
	{
		OutSound = NULL;
		OutEmitter = NULL;
		OutDecal = NULL;
		OutDecalSize = 0.f;
	}
}

void UBaseSingleton::GetKnifeImpactAssets(EPhysicalSurface SurfaceType, USoundBase *& OutSound, UParticleSystem *& OutEmitter, UMaterialInterface *& OutDecal)
{
	UBaseSingleton * Singleton = Cast<UBaseSingleton>(GEngine->GameSingleton);
	if (Singleton)
	{
		FSurfaceTypeProperties & Properties = Singleton->SurfaceTypes[SurfaceType];
		OutSound = Properties.KnifeSound.LoadSynchronous();
		OutEmitter = Properties.ImpactEffect.LoadSynchronous();
		OutDecal = Properties.KnifeDecal.LoadSynchronous();
	}
	else
	{
		OutSound = NULL;
		OutEmitter = NULL;
		OutDecal = NULL;
	}
}
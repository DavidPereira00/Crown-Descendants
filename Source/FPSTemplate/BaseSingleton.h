// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "BaseSingleton.generated.h"

USTRUCT()
struct FSurfaceTypeProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Defaults)
		TAssetPtr<USoundBase> FootstepSound;
	UPROPERTY(EditAnywhere, Category = Defaults)
		TAssetPtr<USoundBase> ImpactSound;
	UPROPERTY(EditAnywhere, Category = Defaults)
		TAssetPtr<UParticleSystem> ImpactEffect;
	UPROPERTY(EditAnywhere, Category = Defaults)
		TAssetPtr<UMaterialInterface> ImpactDecal;
	UPROPERTY(EditAnywhere, Category = Defaults)
		float MaxDecalSize;
	UPROPERTY(EditAnywhere, Category = Defaults)
		TAssetPtr<USoundCue> KnifeSound;
	UPROPERTY(EditAnywhere, Category = Defaults)
		TAssetPtr<UMaterialInterface> KnifeDecal;
};

/**
 * 
 */
UCLASS(Blueprintable)
class FPSTEMPLATE_API UBaseSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		FSurfaceTypeProperties SurfaceTypes[SurfaceType_Max];
	
	static USoundBase * GetFootstepSound(EPhysicalSurface SurfaceType);
	static void GetImpactAssets(EPhysicalSurface SurfaceType, USoundBase *& OutSound, UParticleSystem *& OutEmitter, UMaterialInterface *& OutDecal, float & OutDecalSize);
	static void GetKnifeImpactAssets(EPhysicalSurface SurfaceType, USoundBase *& OutSound, UParticleSystem *& OutEmitter, UMaterialInterface *& OutDecal);
	
};

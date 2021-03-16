// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerupActor.generated.h"

UCLASS()
class FPSTEMPLATE_API APowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerupActor();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
		float PowerupInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
		int32 Totalticksnumber;

	FTimerHandle TimerPowerup;

	int32 Tickspassed;

	UFUNCTION()
		void OnTickPowerup();

	UPROPERTY(ReplicatedUsing = OnRep_PowerupActive)
		bool bIsPowerupActive;

	UFUNCTION()
		void OnRep_PowerupActive();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnPowerupStateChanged(bool bNewIsActive);
public:	
	
	void ActivateP(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void PowerStart(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void PowerEnd();

	
	
};

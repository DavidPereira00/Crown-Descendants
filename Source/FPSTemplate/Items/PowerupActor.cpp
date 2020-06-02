// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSTemplate.h"
#include "PowerupActor.h"
#include "Net/UnrealNetwork.h"


// Sets default values
APowerupActor::APowerupActor()
{
	PowerupInterval = 0.0f;
	Totalticksnumber = 0;

	bIsPowerupActive = false;

	SetReplicates(true);

}

// Called when the game starts or when spawned
void APowerupActor::OnTickPowerup()
{
	Tickspassed++;

	OnPowerupTicked();

	if(Tickspassed >= Totalticksnumber)
	{
		PowerEnd();

		bIsPowerupActive = false;
		OnRep_PowerupActive();

		GetWorldTimerManager().ClearTimer(TimerPowerup);
	}
	
}

// Called every frame
void APowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);

}

void APowerupActor::ActivateP(AActor* ActiveFor)
{
	PowerStart(ActiveFor);

	bIsPowerupActive = true;
	OnRep_PowerupActive();

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerPowerup, this, &APowerupActor::OnTickPowerup, PowerupInterval, true);

	}
	else
	{
		OnTickPowerup();
	}
}

void APowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APowerupActor, bIsPowerupActive);
}


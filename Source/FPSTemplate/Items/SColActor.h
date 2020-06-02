// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "SColActor.generated.h"

class USphereComponent;
class UDecalComponent;
class APowerupActor;

UCLASS()
class FPSTEMPLATE_API ASColActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASColActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
		USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UDecalComponent* DecalComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "PickupActor")
		TSubclassOf<APowerupActor> PowerUpClass;

	APowerupActor* PowerUpInstance;

	UPROPERTY(EditDefaultsOnly, Category = "PickupActor")
		float Cooldown;

	FTimerHandle RespawnTimer;

	void Respawn();

public:	
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	
	
};

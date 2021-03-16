// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSTemplate.h"
#include "SColActor.h"
#include "Components/SphereComponent.h"
#include "Engine.h"
#include "PowerupActor.h"
#include "Net/UnrealNetwork.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASColActor::ASColActor()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	RootComponent = MeshComponent;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(MeshComponent);

	SetReplicates(true);

}

// Called when the game starts or when spawned
void ASColActor::BeginPlay()
{
	Super::BeginPlay();

	if(Role == ROLE_Authority)
	{
		Respawn();
	}
	
}

// Called every frame
void ASColActor::Respawn()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerUpInstance = GetWorld()->SpawnActor<APowerupActor>(PowerUpClass, GetTransform(), SpawnParams);

}

void ASColActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority && PowerUpInstance)
	{
		PowerUpInstance->ActivateP(OtherActor);
		PowerUpInstance = nullptr;

		GetWorldTimerManager().SetTimer(RespawnTimer, this, &ASColActor::Respawn, Cooldown);
	}
}
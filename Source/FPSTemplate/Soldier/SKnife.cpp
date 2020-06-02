// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSTemplate.h"
#include "SKnife.h"


// Sets default values
ASKnife::ASKnife()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void ASKnife::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASKnife::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


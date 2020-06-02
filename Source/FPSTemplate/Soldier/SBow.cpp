// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSTemplate.h"
#include "SBow.h"


// Sets default values
ASBow::ASBow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

}

// Called when the game starts or when spawned
void ASBow::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASBow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


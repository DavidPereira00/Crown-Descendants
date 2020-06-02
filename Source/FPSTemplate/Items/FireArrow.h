// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Soldier/Soldier.h"
#include "FireArrow.generated.h"
UCLASS()
class FPSTEMPLATE_API AFireArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireArrow();

public:	
	UFUNCTION()
		void OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor);

	UPROPERTY(EditAnywhere)
		ASoldier* MySoldier;

	int actives;

	int active();

	
	
};

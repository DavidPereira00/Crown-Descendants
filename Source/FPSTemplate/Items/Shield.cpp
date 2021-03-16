// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSTemplate.h"
#include "Shield.h"


// Sets default values
AShield::AShield()
{
	OnActorBeginOverlap.AddDynamic(this, &AShield::OnOverlap);

}

void AShield::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, FString::Printf(TEXT("Started overlapping"), GetWorld()->TimeSeconds));
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		MySoldier = Cast<ASoldier>(OtherActor);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("different actor"), GetWorld()->TimeSeconds));

		if (MySoldier)
		{
			MySoldier->getShield();
			MySoldier->respawnshield();
			MyOverlappedActor->SetActorHiddenInGame(true);
			MyOverlappedActor->SetActorEnableCollision(false);
			//Destroy();
		}
	}
}


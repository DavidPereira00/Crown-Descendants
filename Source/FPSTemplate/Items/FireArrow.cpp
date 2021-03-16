// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSTemplate.h"
#include "FireArrow.h"


// Sets default values
AFireArrow::AFireArrow()
{
	OnActorBeginOverlap.AddDynamic(this, &AFireArrow::OnOverlap);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Started "), GetWorld()->TimeSeconds));
	actives = 0;

}

void AFireArrow::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, FString::Printf(TEXT("Started overlapping"), GetWorld()->TimeSeconds));
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		actives = 1;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("My activation is: %d"), actives));
		MySoldier = Cast<ASoldier>(OtherActor);
		//ASoldier* Soldier = Cast<ASoldier>(GetWorld()->SpawnActor(ASoldier::StaticClass()));
		ASoldier* Soldier = Cast<ASoldier>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		Soldier->activepowerup();
		Soldier->notWorked();
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("different actor"), GetWorld()->TimeSeconds));

		if (MySoldier)
		{
			MySoldier->Collected();
			MySoldier->respawnfarrow();
			MyOverlappedActor->SetActorHiddenInGame(true);
			MyOverlappedActor->SetActorEnableCollision(false);
			//Destroy();
		}
	}
}


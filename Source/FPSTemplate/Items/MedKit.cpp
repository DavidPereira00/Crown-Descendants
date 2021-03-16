// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSTemplate.h"
#include "MedKit.h"


// Sets default values
AMedKit::AMedKit()
{
	OnActorBeginOverlap.AddDynamic(this, &AMedKit::OnOverlap);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Started "), GetWorld()->TimeSeconds));
}

void AMedKit::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, FString::Printf(TEXT("Started overlapping"), GetWorld()->TimeSeconds));
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		MySoldier = Cast<ASoldier>(OtherActor);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("different actor"), GetWorld()->TimeSeconds));

		if (MySoldier && MySoldier->GetHealth() < 99.0f)
		{
			//MyCharacter->UpdateHealth(100.0f);
			MySoldier->Healthplus();
			Destroy();
		}
	}
}

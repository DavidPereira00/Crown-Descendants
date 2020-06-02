#include "FPSTemplate.h"
#include "Firearm.h"
#include "UnrealNetwork.h"
#include "Soldier/Soldier.h"
#include "StaticFunctionLibrary.h"
#include "BaseGameInstance.h"

AFirearm::AFirearm(const FObjectInitializer & ObjectInitializer)
	: Super(ObjectInitializer)
{
	

	static ConstructorHelpers::FObjectFinder<UTexture2D> NewCrosshairTexture(TEXT("Texture2D'/Game/FPSTemplate/HUD/Textures/T_Crosshair.T_Crosshair'"));
	CrosshairTexture = NewCrosshairTexture.Object;

	bChamber = true;
	Magazine = -1;
	MagazineType = EMagazineTypeEnum::Default;
}

void AFirearm::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFirearm, bAuto);
	DOREPLIFETIME(AFirearm, bChamber);
	DOREPLIFETIME(AFirearm, Magazine);
	DOREPLIFETIME(AFirearm, Ammunition);
	DOREPLIFETIME(AFirearm, ProjectileClass);
}

void AFirearm::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	// If a auto fire mode exists, it should be selected by default
	if (bHasAutoFireMode)
	{
		bAuto = true;
	}

	// Set magazine to magazine capacity
	Magazine = MagazineCapacity;
}

void AFirearm::BeginPlay()
{
	Super::BeginPlay();

	// Destroy suppressor component if it is not needed
	/*if (SuppressorComponent && !SuppressorComponent->bVisible)
	{
		SuppressorComponent->DestroyComponent();
	}*/
}

void AFirearm::Reset()
{
	Super::Reset();

	Magazine = MagazineCapacity;
	Ammunition = GetDefault<AFirearm>(GetClass())->Ammunition;
	bChamber = true;
}

void AFirearm::SetLoadout(const struct FLoadoutSlotStruct & LoadoutSlot)
{
	Super::SetLoadout(LoadoutSlot);

	UBaseGameInstance * GameInstance = UStaticFunctionLibrary::GetBaseGameInstance(this);
	if (!GameInstance) return;

	TSubclassOf<ABaseProjectile> NewProjectileClass = GameInstance->GetProjectile(LoadoutSlot.ProjectileClassName);
	if (*NewProjectileClass)
	{
		ProjectileClass = NewProjectileClass;
	}


}

float AFirearm::GetImprecision(float AimRatio, float BulletCount, const FVector & SoldierVelocity) const
{
	// Return imprecision based on AimRatio, current BulletCount and the owning soldiers velocity
	return ((RecoilCurve ? RecoilCurve->GetVectorValue(BulletCount).Z : 1.f) + SoldierVelocity.Size() * 0.001f) * (6.f - AimRatio * 5.f);
}

void AFirearm::DrawItemHUD(APlayerController * PC, UCanvas * Canvas, FVector CameraPosition, FVector CameraDir, float AimRatio, float BulletCount, const FVector & SoldierVelocity)
{
	// Draw typical firearm crosshair
	ASoldier* Soldier = Cast<ASoldier>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (CrosshairTexture && Soldier->getcrossaim() == 1)
	{
		const float CrosshairOffset = GetImprecision(AimRatio, BulletCount, SoldierVelocity) * 20.f;
		const float CrosshairOpacity = 1.f;
		for (uint8 i = 0; i < 4; i++)
		{
			float MultiX = 0.f;
			float MultiY = -1.f;

			switch (i)
			{
			case (1) :
				MultiX = 1.f;
				MultiY = 0.f;
				break;
			case (2) :
				MultiX = 0.f;
				MultiY = 1.f;
				break;
			case (3) :
				MultiX = -1.f;
				MultiY = 0.f;
				break;
			}

			const FVector2D ScreenPosition = FVector2D(Canvas->SizeX * 0.5f - 3.f, Canvas->SizeY * 0.5f - 24.f) + FVector2D(MultiX * CrosshairOffset, MultiY * CrosshairOffset);
			Canvas->K2_DrawTexture(CrosshairTexture, ScreenPosition, FVector2D(CrosshairTexture->GetSizeX(), CrosshairTexture->GetSizeY()), FVector2D::ZeroVector, FVector2D::UnitVector, FLinearColor(1.f, 1.f, 1.f, CrosshairOpacity), EBlendMode::BLEND_Translucent, i * 90.f, FVector2D(0.5f, 1.f));
		}
	}
}

int32 AFirearm::GetMagazine() const
{
	return Magazine;
}

int32 AFirearm::GetAmmunition() const
{
	return Ammunition;
}

bool AFirearm::GetIsAuto() const
{
	return bAuto;
}

bool AFirearm::HasFullAmmo() const
{
	return Ammunition == GetDefault<AFirearm>(GetClass())->Ammunition;
}

void AFirearm::RefillAmmo()
{
	Ammunition = GetDefault<AFirearm>(GetClass())->Ammunition;
}

void AFirearm::StartFiring()
{
	Fire();
}

void AFirearm::StopFiring()
{

}


void AFirearm::Fire()
{
	if (bChamber)
	{
		if (PerformTask(EItemTaskEnum::Fire, FireAnimation.Length))
		{
			bChamber = false;
			OnFire();

			if (Magazine > 0 && (bAuto || bHasSemiFireMode))
			{
				// Instantly reload chamber if item is semi or full auto
				bChamber = true;
				//Magazine--;
			}
		}
	}
}



void AFirearm::OnDrew()
{
	if (!bChamber)
	{
	}
	else if (IsSoldierFiring())
	{
		// Start firing if soldiers is holding down fire
		Fire();
	}
}

void AFirearm::OnFired()
{
	if (!bChamber)
	{
		
	}
	else if (bAuto && IsSoldierFiring())
	{
		Fire();
	}
}




void AFirearm::OnFire()
{
	if (GetNetMode() == NM_DedicatedServer) return;

	ASoldier * Soldier = GetOwningSoldier();
	if (Soldier && Soldier->isAiming()==true)
	{
		Soldier->FireProjectile(ProjectileClass);
		Magazine--;
		const FVector MuzzleLocation = Mesh->GetSocketLocation(FName(TEXT("Muzzle")));
		// Fire sound
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLocation);
	}

	PlayAnimation(FireAnimation, 0.02f, 0.02f);

	
	

	if (GetNetMode() == NM_Client && bAuto)
	{
		Soldier->OnClientStartAutoFire(FireAnimation.Length);
	}
}


void AFirearm::moremaganize()
{
	Magazine = Magazine + 5;
}
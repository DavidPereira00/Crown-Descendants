#include "FPSTemplate.h"
#include "Bullet.h"
#include "Damage/DamageType_Bullet.h"
#include "Damage/DamageType_Headshot.h"
#include "Player/BasePlayerController.h"
#include "BaseSingleton.h"

ABullet::ABullet(const FObjectInitializer & ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.05f;
	ProjectileMovement->BounceVelocityStopSimulatingThreshold = 5000.f;
}

void ABullet::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	ProjectileMovement->MaxSpeed = ProjectileMovement->InitialSpeed;
}

void ABullet::OnProjectileHit(const FHitResult & HitResult, const FVector & ImpactVelocity)
{
	if (HitResult.GetActor())
	{
		FPointDamageEvent DamageEvent;
		DamageEvent.DamageTypeClass = HitResult.BoneName == FName(TEXT("head")) ? UDamageType_Headshot::StaticClass() : UDamageType_Bullet::StaticClass();
		DamageEvent.HitInfo = HitResult;
		DamageEvent.ShotDirection = ImpactVelocity;
		DamageEvent.Damage = CalcDamage(HitResult, ImpactVelocity);

		// Apply force to hit component
		if (HitResult.GetComponent() && HitResult.Component->IsSimulatingPhysics())
		{
			HitResult.Component->AddImpulse(ImpactVelocity * DamageEvent.Damage * 0.002f, HitResult.BoneName);
		}

		if (HitResult.Actor->GetNetMode() != ENetMode::NM_Standalone)
		{
			ABasePlayerController * PC = Cast<ABasePlayerController>(GetInstigatorController());
			if (PC)
			{
				if (PC->GetNetMode() == NM_Client)
				{
					// If the hit actor is networked and hit by local player controller, server should apply damage
					if (PC->IsLocalController())
					{
						PC->OnClientHitActor(HitResult.Actor.Get(), DamageEvent, GetOwner());
					}
				}
				else
				{
					// No damage if hit on server and PC is not local
					if (!PC->IsLocalController())
					{
						return;
					}
				}
			}
		}

		HitResult.Actor->TakeDamage(DamageEvent.Damage, DamageEvent, GetInstigatorController(), GetOwner());
	}

	if (GetNetMode() != NM_DedicatedServer)
	{
		// Spawn impact effect on clients and listen servers only
		SpawnImpactEffect(HitResult);
	}
}



float ABullet::CalcDamage(const FHitResult & HitResult, const FVector & ImpactVelocity) const
{
	return ImpactVelocity.SizeSquared() / FMath::Square(ProjectileMovement->InitialSpeed) * (UGameplayStatics::GetSurfaceType(HitResult) == SurfaceType8 ? MaxArmorDamage : MaxDamage);
}
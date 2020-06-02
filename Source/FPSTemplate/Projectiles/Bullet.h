#pragma once

#include "Projectiles/BaseProjectile.h"
#include "Bullet.generated.h"



UCLASS()
class FPSTEMPLATE_API ABullet : public ABaseProjectile
{
	GENERATED_BODY()

public:
	

	// Maxi damage this bullet will cause
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		float MaxDamage;

	// Max damage this bullet will cause against an armor physics material (used for soldier body armor)
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		float MaxArmorDamage;

	ABullet(const FObjectInitializer & ObjectInitializer);
	
	void OnConstruction(const FTransform & Transform) override;

	void OnProjectileHit(const FHitResult & HitResult, const FVector & ImpactVelocity) override;


	virtual float CalcDamage(const FHitResult & HitResult, const FVector & ImpactVelocity) const;
	
};

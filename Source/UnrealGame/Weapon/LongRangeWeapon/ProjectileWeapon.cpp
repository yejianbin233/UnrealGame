// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "../../Projectile/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire()
{
	// Super::Fire(HitTarget);

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	const USkeletalMeshSocket* MuzzleFlashSocket =  GetWeaponMeshComponent()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMeshComponent());
		
		// FVector ToTargetDirection = HitTarget - SocketTransform.GetLocation();
		// FRotator TargetRotation = ToTargetDirection.Rotation();
		
		if (ProjectileClass && InstigatorPawn)
		{
			// UWorld* World = GetWorld();
			// if (World)
			// {
			// 	FActorSpawnParameters SpawnParameters;
			// 	SpawnParameters.Owner = GetOwner();
			// 	SpawnParameters.Instigator = InstigatorPawn;
			// 	
			// 	World->SpawnActor<AProjectile>(
			// 		ProjectileClass
			// 		, SocketTransform.GetLocation()
			// 		, TargetRotation
			// 		, SpawnParameters
			// 	);
			// }
		}
				
	}
}

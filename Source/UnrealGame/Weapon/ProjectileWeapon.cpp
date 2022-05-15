// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealGame/Weapon/ProjectileWeapon.h"

#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	const USkeletalMeshSocket* MuzzleFlashSocket =  GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		
		FVector ToTargetDirection = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTargetDirection.Rotation();
		
		if (ProjectileClass && InstigatorPawn)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = GetOwner();
				SpawnParameters.Instigator = InstigatorPawn;
				
				World->SpawnActor<AProjectile>(
					ProjectileClass
					, SocketTransform.GetLocation()
					, TargetRotation
					, SpawnParameters
				);
			}
		}
				
	}
}

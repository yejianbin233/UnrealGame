// Fill out your copyright notice in the Description page of Project Settings.


#include "GunWeapon.h"

#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "UnrealGame/Component/Collimation/CollimationComponent.h"

bool AGunWeapon::IsAmmoEmpty()
{
	return LoadAmmo <= 0;
}

AGunWeapon::AGunWeapon()
{
	Init();
}

void AGunWeapon::Init()
{
	Super::Init();

	// TODO 添加组件失败
	// UActorComponent* CollimationComponent = AddComponentByClass(CollimationComponentClass, false, FTransform(), false);
	//
	// // 重要，否则无法在细节面板中看到组件
	// this->AddInstanceComponent(CollimationComponent);
	// 			
	// // 重要，必须要注册组件
	// CollimationComponent->RegisterComponent();

}

void AGunWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimtion)
	{
		WeaponMeshComponent->PlayAnimation(FireAnimtion, false);
	}
	
	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMeshComponent->GetSocketByName(FName("AmmoEject"));

		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMeshComponent);

			FActorSpawnParameters SpawnParameters;
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasing>(
					CasingClass
					, SocketTransform.GetLocation()
					, SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}
}

void AGunWeapon::AutoFire()
{
}


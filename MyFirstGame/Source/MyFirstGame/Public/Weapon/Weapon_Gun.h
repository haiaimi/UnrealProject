// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyFirstGame.h"
#include "Weapon_Gun.generated.h"

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	/**����Я������ӵ�����*/
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 AmmoPerClip;

	/**��ʼʱ�ӵ�����*/
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 InitialAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	int8 ShootSpeed;

	/**��ǹ������*/
	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	int8 WeaponType;

	FWeaponData()
	{
		MaxAmmo = 300;
		AmmoPerClip = 30;
		InitialAmmo = 60;
		ShootSpeed = 10;
		WeaponType = EWeaponType::Weapon_Projectile;
	}
};

UCLASS(Abstract, Blueprintable)
class MYFIRSTGAME_API AWeapon_Gun : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GunData", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditDefaultsOnly, Category = "GunData")
	FWeaponData WeaponData;

	/**�����ӵ�������Ӧ�ĵ㣨ǹ�Ĺ����ϣ�*/
	UPROPERTY(EditDefaultsOnly, Category = "GunData")
	FName FirePoint;

	UPROPERTY(EditDefaultsOnly, Category = "GunData")
	class UParticleSystem* FireEmitter;

protected:
	virtual void PostInitializeComponents()override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UParticleSystem* GetFireParticle() { return FireEmitter; }

	FVector GetFireLocation();

	FTransform GetFireTransform();
};

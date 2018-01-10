// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyFirstGameCharacter.generated.h"

UCLASS(config=Game)
class AMyFirstGameCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<class ABullet> ProjectileWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<TSubclassOf<class AWeapon_Gun>> Inventory;

	//������ŵ�ǰ�������ָ������
	UPROPERTY()
	TArray<class AWeapon_Gun*> InterInventory;

	/**�������Ĳ��*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocket;

	class AWeapon_Gun* CurWeapon;

	/**��������*/
	int32 WeaponNum;

	UPROPERTY(EditDefaultsOnly, Category = "Aim_Motage")
	UAnimMontage* ShootAim;

	UPROPERTY(EditDefaultsOnly, Category = "Aim_Motage")
	UAnimMontage* StandToCrounchAim;

	UPROPERTY(EditDefaultsOnly, Category = "Aim_Motage")
	UAnimMontage* CrounchToStandAim;

	/**�Ƿ�������ת*/
	uint8 IsToRight : 1;

	/**�Ƿ��ڻ���*/
	uint8 IsViewAround : 1;

	/**����*/
	uint8 OpenTheDoor : 1;

	/**����*/
	uint8 CloseTheDoor : 1;

	/**�Ƿ����¶�״̬*/
	uint8 IsInCrounch : 1;

	/**�Ƿ���վ�����¶׶���֮��*/
	uint8 IsInStandToCrounch : 1;

	/**�Ƿ����¶׵�վ������֮��*/
	uint8 IsInCrounchToStand : 1;

	/*վ�������¶���ʱ��*/
	float ActionAnimTime = 0.f;

	float CurActionTime;
	
	/**�Ƿ����ŵļ������*/
	uint8 IsInDoorCollision : 1;

	/**��������ڵ��Ŵ��ڿ���״̬*/
	uint8 IsInOpen : 1;

	/**��������ڵ��Ŵ��ڹر�״̬*/
	uint8 IsInClose : 1;

	/***/
	float AddYaw = 0.f;

	/**����Ƿ�����׼״̬*/
	uint8 IsTargeting : 1;

	/**��ҵ�ǰ����*/
	uint8 CurrentWeapon;

	/**��ҵ�ǰ�Ƿ������*/
	uint8 IsShooting : 1;

	/**��ǰ��������*/
	uint8 ShootSpeed;

	/**��������ļ��*/
	float ShootInternal;

	/**����ܷ�ǹ*/
	uint8 CanShoot : 1;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**���뻷������״̬*/
	void ViewAround();

	/**ֹͣ����*/
	void StopViewAround();

	/**��ҽ������״̬*/
	void StartAccelerate();

	/**ֹͣ����*/
	void StopAccelerate();

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	/**�������/��Ϸִ��ʱ��ִ�еĲ���*/
	virtual void PostInitializeComponents()override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable,Category = "Pawn")
	FRotator ComputeAimOffset()const;

	virtual void AddControllerYawInput(float del);

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction);

	///��Ҷ�����أ����š��۲��ӽ�
	void UpdateStandCharacter();

	bool IsCharacterViewAround()
	{
		return IsViewAround;
	}
	/**������*/
	void OperateDoor();

	/**�뿪�ż�������ֹ������Ϊ*/
	void AvoidOpenDoor();

	void ToggleCrounchStat();


	//���ڶ���״̬��
	UFUNCTION(BlueprintPure, Category = "Pawn")
	bool IsPlayerInCrounch() { return IsInCrounch; }

	FORCEINLINE	float AdjustDegree(const float in)const;
	
	///�������������������
	void StartShoot() { IsShooting = true; }

	void StopShoot() { IsShooting = false; }

	/**
	 *  ��⵱ǰ׼���Ƿ��׼Ŀ��
	 *  @Param Distance ���ľ���
	 */
	FHitResult QueryCrossHair(float Distance);

	/**���*/
	void Shoot(float AimRate);

	/**������׼״̬*/
	void Targeting();

	/**ֹͣ��׼*/
	void StopTargeting();

	bool PlayerIsTargeting() { return IsTargeting; }

	/**�����������ӵ�*/
	void FireProjectile();

	/**ѡ������*/
	void NextWeapon();

	void PreWeapon();

	void EquipWeapon(AWeapon_Gun* CurWeapon);

	///�����Ƕ������
	/**������̫�涯�������ض���ʱ��*/
	float PlayAnim(float rate, UAnimMontage* Anim);

	float ComputeSuitRate(float CurTimePerFrame,int8 CurShootSpeed);

};


// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MyFirstGameCharacter.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Classes/Animation/AnimInstance.h"
#include "Player/MyPlayerController.h"
#include "BoomActor.h"
#include "Weapon/Bullet.h"
#include "Weapon/Weapon_Gun.h"
#include "Kismet/GameplayStatics.h"
#include "MyFirstGame.h"
#include "Engine/Engine.h"

//////////////////////////////////////////////////////////////////////////
// AMyFirstGameCharacter

AMyFirstGameCharacter::AMyFirstGameCharacter(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//是否进入环顾状态
	IsViewAround = false;
	OpenTheDoor = false;
	CloseTheDoor = false;

	CanShoot = true;
	IsInCrounch = false;    //初始状态是站立状态
	IsInCrounchToStand = false; //初始状态不在动作中
	IsInStandToCrounch = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	
	CameraBoom->SetRelativeRotation(FRotator(45.0f, 0.0f, 0.0f));
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	CurrentWeapon = EWeaponType::Type::Weapon_Instant;
	ShootInternal = 0.f;

	// Create a follow camera
    //FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyFirstGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("ViewAround", IE_Pressed, this, &AMyFirstGameCharacter::ViewAround);
	PlayerInputComponent->BindAction("ViewAround", IE_Released, this, &AMyFirstGameCharacter::StopViewAround);
	PlayerInputComponent->BindAction("OpenDoor", IE_Pressed, this, &AMyFirstGameCharacter::OperateDoor);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AMyFirstGameCharacter::StartShoot);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AMyFirstGameCharacter::StopShoot);
	PlayerInputComponent->BindAction("Targeting", IE_Pressed, this, &AMyFirstGameCharacter::Targeting);
	PlayerInputComponent->BindAction("Targeting", IE_Released, this, &AMyFirstGameCharacter::StopTargeting);
	PlayerInputComponent->BindAction("ShootProjectile", IE_Released, this, &AMyFirstGameCharacter::FireProjectile);
	PlayerInputComponent->BindAction("NextWeapon", IE_Released, this, &AMyFirstGameCharacter::NextWeapon);
	PlayerInputComponent->BindAction("PreWeapon", IE_Released, this, &AMyFirstGameCharacter::PreWeapon);
	PlayerInputComponent->BindAction("Accelerate", IE_Pressed, this, &AMyFirstGameCharacter::StartAccelerate);
	PlayerInputComponent->BindAction("Accelerate", IE_Released, this, &AMyFirstGameCharacter::StopAccelerate);
	PlayerInputComponent->BindAction("Crounch", IE_Pressed, this, &AMyFirstGameCharacter::ToggleCrounchStat);
	//PlayerInputComponent->BindAction("Crounch", IE_Released, this, &AMyFirstGameCharacter::EndCrounch);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyFirstGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyFirstGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AMyFirstGameCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyFirstGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyFirstGameCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyFirstGameCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMyFirstGameCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyFirstGameCharacter::OnResetVR);
}

void AMyFirstGameCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	WeaponNum = Inventory.Num();   //武器数量
	if (WeaponNum > 0)
	{
		for (int32 i = 0; i < WeaponNum; i++)
		{
			if (Inventory[i])
			{
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;     //忽略碰撞体
				AWeapon_Gun* weapon = GetWorld()->SpawnActor<AWeapon_Gun>(Inventory[i], param);
				InterInventory.Add(weapon);
			}
		}

		CurWeapon = InterInventory[0];
		EquipWeapon(CurWeapon);
	}
}

void AMyFirstGameCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMyFirstGameCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AMyFirstGameCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AMyFirstGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	//AddYaw= Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds();
}

void AMyFirstGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyFirstGameCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (IsViewAround)
		{
			// find out which way is right
			const FRotator Rotation = GetActorRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
		//const FVector Direction1 = FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::X); 
		else
			// get forward vector
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}
}

void AMyFirstGameCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		if (!IsViewAround)
		{
			//use Controller
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);

			//use Character
		}
	}
}

void AMyFirstGameCharacter::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (CanShoot)
	{
		if (IsShooting)
		{
			if (CurrentWeapon == EWeaponType::Weapon_Instant)
			{
				ShootSpeed = 8;       //8发每秒
			}
			else if (CurrentWeapon == EWeaponType::Weapon_Projectile)
			{
				ShootSpeed = 6;       //6发每秒
			}

			float AnimRate = ComputeSuitRate(DeltaTime, ShootSpeed);

			if (ShootInternal == 0)
			{
				Shoot(AnimRate);
			}
			ShootInternal += DeltaTime;       //
			if (ShootInternal >= 1.f / ShootSpeed)
			{
				Shoot(AnimRate);
				ShootInternal = 0.01f;
			}
		}
		else
		{
			ShootInternal = 0.f;
		}
	}

	if (IsInStandToCrounch)       //96-48
	{
		if (GetWorld()->GetTimeSeconds() - CurActionTime >= ActionAnimTime)
		{
			IsInStandToCrounch = false;
		}
		
	}
	if (IsInCrounchToStand)       //48-96
	{
		if (GetWorld()->GetTimeSeconds() - CurActionTime >= ActionAnimTime)
		{
			IsInCrounchToStand = false;
		}
	}
}

void AMyFirstGameCharacter::AddControllerYawInput(float Val)
{
	Super::AddControllerYawInput(Val);
	if (Val > 0)
		IsToRight = true;

	else if(Val < 0)
		IsToRight = false;
}
void AMyFirstGameCharacter::ViewAround()
{
	CanShoot = false;       //环顾的时候不能射击
	IsViewAround = true;
}

void AMyFirstGameCharacter::StopViewAround()
{
	CanShoot = true;
	IsViewAround = false;
}

void AMyFirstGameCharacter::StartAccelerate()
{
	CanShoot = false;         //加速跑的时候也不能射击
	GetCharacterMovement()->MaxWalkSpeed = 850.f;        //把速度增加到850
}

void AMyFirstGameCharacter::StopAccelerate()
{
	CanShoot = true;
	GetCharacterMovement()->MaxWalkSpeed = 600.f; 
}

FRotator AMyFirstGameCharacter::ComputeAimOffset()const
{
	//第一种方法
	if (Controller)                    //注意这句必加，否则蓝图崩溃，一定要判断指针是否为空 GetBaseAimRotation() 函数已经判断过
	{
		const FRotator deltaR = /*GetBaseAimRotation()*/Controller->GetControlRotation() - GetActorRotation();
		const float YawDelta = AdjustDegree(deltaR.Yaw);
		const float PitchDelta = AdjustDegree(deltaR.Pitch);
		const FRotator AimOffset = FRotator(PitchDelta, YawDelta, 0.f);

		return AimOffset;
	}
	return FRotator::ZeroRotator;

	//第二种方法
	/*const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);    //获取两个旋转Vector的差，就是offset
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;*/
}

void AMyFirstGameCharacter::OperateDoor()
{
	if (IsInDoorCollision)
	{
		if (IsInOpen)
		{
			CloseTheDoor = true;
			OpenTheDoor = false;
		}
		else
		{
			OpenTheDoor = true;
			CloseTheDoor = false;
		}
	}
}

void AMyFirstGameCharacter::AvoidOpenDoor()
{
	IsInDoorCollision = false;
	OpenTheDoor = false;
	CloseTheDoor = false;
}

void AMyFirstGameCharacter::UpdateStandCharacter()
{
	float sub = 0.f;					 //Controller和Character  的Yaw轴之间的差
	if (Controller)
	{
		float ControllerCurYaw = Controller->GetControlRotation().Yaw;
		float CharacterYaw = GetActorRotation().Yaw;

		FVector ControllerVec = FRotator(0.f, ControllerCurYaw, 0.f).Vector().GetSafeNormal();
		FVector CharacterVec = GetActorRotation().Vector().GetSafeNormal();
		float SubAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ControllerVec, CharacterVec)));
	
		if (SubAngle >= 80.f)
		{
			if (!IsViewAround)
			{
				if (IsToRight)
				{
					SetActorRotation(FRotator(0.f, ControllerCurYaw - 80.f, 0.f));
				}
				else
				{
					SetActorRotation(FRotator(0.f, ControllerCurYaw + 80.f, 0.f));
				}
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, FString::Printf(TEXT("Controller Yaw:%f,Character Yaw:%f,TestYawSub:%f"), ControllerCurYaw,CharacterYaw, SubAngle), true);
	}
}

float AMyFirstGameCharacter::AdjustDegree(const float in)const
{
	if (in > 180.f)
	{
		return in - 360.f;
	}
	if (in < -180.f)
	{
		return in + 360.f;
	}
		return in;
}

void AMyFirstGameCharacter::Shoot(float AimRate)
{
	//先播放射击动画
	PlayAnim(AimRate, ShootAim);

	if (CurrentWeapon == EWeaponType::Weapon_Instant)
	{
		FHitResult Result = QueryCrossHair(10000.f);

		if (Cast<ABoomActor>(Result.GetActor()))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, FString::Printf(TEXT("检测到目标！")), true);
			ABoomActor* Aim = Cast<ABoomActor>(Result.GetActor());         //如果检测到目标就爆炸
			if (Aim->CanBoom)
			{
				Aim->Boom();
			}
		}
	}
	else if (CurrentWeapon == EWeaponType::Weapon_Projectile)
	{
		FireProjectile();
	}
	
}

FHitResult AMyFirstGameCharacter::QueryCrossHair(float Distance)
{
	AMyPlayerController* MPC;
	FVector ViewLocation;
	FRotator ViewRotation;
	FHitResult Result;

	if (Controller)
	{
		MPC = Cast<AMyPlayerController>(Controller);
		ViewLocation = MPC->PlayerCameraManager->ViewTarget.POV.Location;
		ViewRotation = MPC->GetControlRotation();
		//FVector ForwardVec = ViewRotation.Vector();                    //也使用该方法
		FVector ForwardVec = FRotationMatrix(ViewRotation).GetUnitAxis(EAxis::X);         //获取某个轴的方向也可以使用矩阵

		FCollisionQueryParams TraceParam(TEXT("QueryInstant"), false, this);
		FCollisionResponseParams ResponseParam(ECollisionResponse::ECR_Block);

		GetWorld()->LineTraceSingleByChannel(Result, ViewLocation, ViewLocation + Distance*ForwardVec, ECollisionChannel::ECC_Visibility, TraceParam, ResponseParam);
	}
	return Result;
}

void AMyFirstGameCharacter::Targeting()
{
	IsTargeting = true;
}

void AMyFirstGameCharacter::StopTargeting()
{
	IsTargeting = false;
}

void AMyFirstGameCharacter::FireProjectile()
{
	FVector ShootDir;
	FHitResult Result = QueryCrossHair(10000.f);
	if (Cast<ABoomActor>(Result.GetActor()))
	{
		ABoomActor* Aim = Cast<ABoomActor>(Result.GetActor());
		ShootDir = (Aim->GetActorLocation() - CurWeapon->GetFireLocation()).SafeNormal();
	}
	else
	{
		ShootDir = (Controller->GetControlRotation() + FRotator(2.f, 0.f, 0.f)).Vector();      //Pitch旋转轴提高2度，以适配准心位置
	}
	FTransform SpawnTrans(FRotator::ZeroRotator, CurWeapon->GetFireLocation());

	//第一种方法
	ABullet* SpawnBullet = GetWorld()->SpawnActorDeferred<ABullet>(ProjectileWeapon, SpawnTrans, this, this);
	if (SpawnBullet)
	{
		SpawnBullet->Instigator = this;
		SpawnBullet->InitBulletVelocity(ShootDir);

		UGameplayStatics::FinishSpawningActor(SpawnBullet, SpawnTrans);
	}

	//释放开火特效
	if (CurWeapon->GetFireParticle())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CurWeapon->GetFireParticle(), CurWeapon->GetFireLocation());
	}
	
	//第二种方法
	/*ABullet* SpawnBullet = Cast<ABullet>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileWeapon, SpawnTrans));
	if (SpawnBullet)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "You have Shoot!");
		SpawnBullet->Instigator = this;
		SpawnBullet->InitBulletVelocity(ShootDir);

		UGameplayStatics::FinishSpawningActor(SpawnBullet, SpawnTrans);
	}*/
}

void AMyFirstGameCharacter::NextWeapon()
{
	CurrentWeapon = (CurrentWeapon + 1) % EWeaponType::Type::Weapon_Num;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, CurrentWeapon == EWeaponType::Weapon_Instant ? "Current Weapon Is Weapon_Instant" : "Current Weapon Is Weapon_Projectile");
}

void AMyFirstGameCharacter::PreWeapon()
{
	CurrentWeapon = FMath::Abs((CurrentWeapon - 1)) % EWeaponType::Type::Weapon_Num;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, CurrentWeapon == EWeaponType::Weapon_Instant ? "Current Weapon Is Weapon_Instant" : "Current Weapon Is Weapon_Projectile");
}

float AMyFirstGameCharacter::PlayAnim(float rate, UAnimMontage* Anim)
{
	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if (PlayerMesh && Anim && PlayerMesh->AnimScriptInstance)
	{
		return PlayerMesh->AnimScriptInstance->Montage_Play(Anim, rate);
	}
	return 0;
}

float AMyFirstGameCharacter::ComputeSuitRate(float CurTimePerFrame, int8 CurShootSpeed)
{
	float PerShootAnimTime = 6 * CurTimePerFrame;     //每发动画的时间
	float CurPerShootTime = 1.f / CurShootSpeed;          //当前射速情况下，发射子弹间隔时间
	float rate = PerShootAnimTime / CurPerShootTime;

	return rate;
}

void AMyFirstGameCharacter::ToggleCrounchStat()
{
	if (!IsInStandToCrounch && !IsInCrounchToStand)             //如果不是在动作状态下才能进行切换
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,"In Toggle");
		IsInCrounch = IsInCrounch + 1;

		if (IsInCrounch)
		{
			ActionAnimTime = PlayAnim(1.f, StandToCrounchAim); //动画播放的时间
			CurActionTime = GetWorld()->GetTimeSeconds();          //获取动画开始时间
			IsInStandToCrounch = true;
			GetCharacterMovement()->MaxWalkSpeed = 300;

			FVector CapLocation = GetCapsuleComponent()->GetComponentLocation();
			GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
			GetCapsuleComponent()->SetWorldLocation(CapLocation - FVector(0.f, 0.f, 48.f));
			GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -48));
		}
		else
		{
			ActionAnimTime = PlayAnim(1.f, CrounchToStandAim);
			CurActionTime = GetWorld()->GetTimeSeconds();
			IsInCrounchToStand = true;
			GetCharacterMovement()->MaxWalkSpeed = 600;

			FVector CapLocation = GetCapsuleComponent()->GetComponentLocation();
			GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);
			GetCapsuleComponent()->SetWorldLocation(CapLocation + FVector(0.f, 0.f, 48.f));
			GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96));
		}
	}
}

void AMyFirstGameCharacter::EquipWeapon(AWeapon_Gun* CurWeapon)
{
	this->CurWeapon = CurWeapon;
	CurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponSocket);       //把Weapon附着到玩家身上,记住是附着在网格上，而不是整个Actor
	this->CurWeapon->SetActorRelativeRotation(FRotator(0.f, 90.f, 0.f));
}
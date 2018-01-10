// Fill out your copyright notice in the Description page of Project Settings.

#include "Door.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "MyFirstGameCharacter.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bGenerateOverlapEventsDuringLevelStreaming = true;
	DoorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorCollision"));
	DoorRoot = CreateDefaultSubobject<UArrowComponent>(TEXT("DoorRoot"));
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));

	DoorCollision->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));

	if(DoorCollision)
	{ 
		DoorCollision->SetupAttachment(RootComponent);
	}
	const FVector CollisionExtent = DoorCollision->GetScaledBoxExtent();

	DoorRoot->SetupAttachment(RootComponent);
	DoorRoot->SetRelativeLocation(FVector(-CollisionExtent.GetComponentForAxis(EAxis::X) / 10.0f, 0.0f, 0.0f));

	if (DoorMesh)
	{
		DoorMesh->SetupAttachment(DoorRoot);
	}

	//��Ĭ��Ϊ��
	IsInOpen = false;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanbOperate)
	{
		if (CurChar->OpenTheDoor)   //���Ϊtrue�Ϳ��Խ��п���
		{
			IsInOpen = true;
			IsInCloseRotate = false;
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, "Open The Door", true);
			YawRotation = FMath::FInterpTo(DoorRoot->RelativeRotation.Yaw, 120.f, DeltaTime, 4.f);
			DoorRoot->SetRelativeRotation(FRotator(0.0f, YawRotation, 0.0f));
			
			if (YawRotation == 120.f)
				IsInOpenRotate = false;
			else
				IsInOpenRotate = true;
		}
		if(CurChar->CloseTheDoor)
		{
			IsInOpen = false;
			IsInOpenRotate = false;
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, "Close The Door", true);
			YawRotation = FMath::FInterpTo(DoorRoot->RelativeRotation.Yaw, 0.f, DeltaTime, 4.f);
			DoorRoot->SetRelativeRotation(FRotator(0.0f, YawRotation, 0.0f));

			if (YawRotation == 0.f)
				IsInCloseRotate = false;
			else
				IsInCloseRotate = true;
		}
		CurChar->IsInOpen = IsInOpen;
	}
	else
	{
		if (IsInOpenRotate)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, "Is In Open Rotate!", true);
			YawRotation = FMath::FInterpTo(DoorRoot->RelativeRotation.Yaw, 120.f, DeltaTime, 4.f);
			DoorRoot->SetRelativeRotation(FRotator(0.0f, YawRotation, 0.0f));

			if (YawRotation == 120.f)
				IsInOpenRotate = false;
		}
		if (IsInCloseRotate)
		{
			YawRotation = FMath::FInterpTo(DoorRoot->RelativeRotation.Yaw, 0.f, DeltaTime, 4.f);
			DoorRoot->SetRelativeRotation(FRotator(0.0f, YawRotation, 0.0f));

			if (YawRotation == 0.f)
				IsInCloseRotate = false;
		}
	}
}

void ADoor::NotifyActorBeginOverlap(AActor* OtherActor)
{ 
	if (Cast<AMyFirstGameCharacter>(OtherActor))
	{
		CurChar = Cast<AMyFirstGameCharacter>(OtherActor);
		CurChar->IsInDoorCollision = true;
		CurChar->IsInOpen = IsInOpen;
		CanbOperate = true;
	//	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, "You Can Open The Door!", true);
	}
}

void  ADoor::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (CurChar == Cast<AMyFirstGameCharacter>(OtherActor))
	{
		CurChar->AvoidOpenDoor();
		CurChar = NULL;
		CanbOperate = false;
	//	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, "You Can't Open The Door", true);
	}
}


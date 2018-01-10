// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyFirstGameCharacter.h"
#include "Door.generated.h"

UCLASS()
class MYFIRSTGAME_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		class UBoxComponent* DoorCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
		//TSubclassOf<class UStaticMeshComponent> DoorMesh;
		class UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		class UArrowComponent* DoorRoot;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//��ҽ����ż������
	virtual void NotifyActorBeginOverlap(AActor* OtherActor)override;

	//����߳��ż������
	virtual void NotifyActorEndOverlap(AActor* OtherActor)override;

private:
	AMyFirstGameCharacter* CurChar;

	//���Ƿ���Ա���
	uint8 CanbOperate : 1;

	//���ڿ���ת��״̬
	uint8 IsInOpenRotate : 1;

	//���ڹر�ת��״̬
	uint8 IsInCloseRotate : 1;

	//���ڿ���״̬
	uint8 IsInOpen : 1;

	//
	uint8 IsInClose : 1;

	//��ǰ�ŵĽǶ�
	float YawRotation = 0.f;
	
};

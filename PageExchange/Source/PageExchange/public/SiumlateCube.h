// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SiumlateCube.generated.h"

UCLASS()
class PAGEEXCHANGE_API ASiumlateCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASiumlateCube();

public:
	int32 PageNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* SimulateCubeMesh;

	/**ҳ�������һ��*/
	ASiumlateCube* NextCube;

	/**�û�Block�е�һ��*/
	ASiumlateCube* ReplaceCube;

	/**�����ƶ���Block*/
	uint8 MoveToBlock : 1;

	/**�����Ƴ�Block*/
	uint8 DropFromBlock : 1;

	/**��Block��Ŀ��λ��*/
	FVector DestToBlock;

	FTimerHandle DestroyHandle;

	/**���Block�����о�ֱ��ɾ���ÿ�*/
	uint8 JustDestroy : 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UStaticMeshComponent* GetMesh() 
	{
		if (SimulateCubeMesh)
			return SimulateCubeMesh;
		return NULL;
	}

	void DestroyActor();

	/**������ص�����cube��Ĭ��Ϊ��*/
	void CubeRelative(FVector Dst, bool JustDestroy, ASiumlateCube* NextCube = NULL, ASiumlateCube* ReplaceCube = NULL);
};

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

	/**页表里的下一个*/
	ASiumlateCube* NextCube;

	/**置换Block中的一个*/
	ASiumlateCube* ReplaceCube;

	/**方块移动到Block*/
	uint8 MoveToBlock : 1;

	/**方块移出Block*/
	uint8 DropFromBlock : 1;

	/**到Block的目标位置*/
	FVector DestToBlock;

	FTimerHandle DestroyHandle;

	/**如果Block中已有就直接删除该块*/
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

	/**设置相关的两个cube，默认为空*/
	void CubeRelative(FVector Dst, bool JustDestroy, ASiumlateCube* NextCube = NULL, ASiumlateCube* ReplaceCube = NULL);
};

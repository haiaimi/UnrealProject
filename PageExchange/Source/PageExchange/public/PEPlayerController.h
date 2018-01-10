// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PEPlayerController.generated.h"

USTRUCT()
struct FPageInfo
{
	GENERATED_USTRUCT_BODY()

	int32 content;   //页面号
	int32 timer; //被访问标记
	int32 cubeTag;  //方块的标记
	FPageInfo()
	{
		content = -1;
		timer = 0;
		cubeTag = -1;
	}
};


UCLASS()
class PAGEEXCHANGE_API APEPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()
	
public:
	APEPlayerController();

	virtual void PostInitializeComponents()override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "PreData")
	TArray<int32> Page;

	UPROPERTY(EditDefaultsOnly, Category = "PreData")
	TArray<struct FPageInfo> Block;

	UPROPERTY()
	TArray<struct FPageInfo> PageInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Simulate")
	TSubclassOf<class ASiumlateCube> SimCube;

	//存储所有模拟方块指针的数组
	UPROPERTY()
	TArray<class ASiumlateCube*> AllPageCubePtr;

	FTimerHandle SimulateHandle;

protected:
	virtual void SetupInputComponent()override;
	 
//置换算法
public:
	void Optimal();

	void FIFO();

	void LRU();

	int32 FindSpace();

	int32 FindExist(int32 curpage);

	/**找出Timer最大的下标，将其替换*/
	int32 FindReplace();

	/**重置并生成所有的方块*/
	void SpawnAllPageCube();

	/**返回Block的容量*/
	UFUNCTION(BlueprintCallable, Category = "Data")
	int32 GetBlockNum() { return Block.Num(); }

	/**返回Page的容量*/
	UFUNCTION(BlueprintCallable, Category = "Data")
	int32 GetPageNum() { return Page.Num(); }

	/**添加的容量*/
	UFUNCTION(BlueprintCallable, Category = "Data")
	void IncreaseBlockNum();

	UFUNCTION(BlueprintCallable, Category = "Data")
	void DecreaseBlockNum();

	/**添加对应页号的Page*/
	UFUNCTION(BlueprintCallable, Category = "Data")
	void IncreasePage(int32 PageContent);

	/**删除末尾的Page*/
	UFUNCTION(BlueprintCallable, Category = "Data")
	void DecreasePage();

	/**退出游戏*/
	void QuitGame();
};

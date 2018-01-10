// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PEPlayerController.generated.h"

USTRUCT()
struct FPageInfo
{
	GENERATED_USTRUCT_BODY()

	int32 content;   //ҳ���
	int32 timer; //�����ʱ��
	int32 cubeTag;  //����ı��
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

	//�洢����ģ�ⷽ��ָ�������
	UPROPERTY()
	TArray<class ASiumlateCube*> AllPageCubePtr;

	FTimerHandle SimulateHandle;

protected:
	virtual void SetupInputComponent()override;
	 
//�û��㷨
public:
	void Optimal();

	void FIFO();

	void LRU();

	int32 FindSpace();

	int32 FindExist(int32 curpage);

	/**�ҳ�Timer�����±꣬�����滻*/
	int32 FindReplace();

	/**���ò��������еķ���*/
	void SpawnAllPageCube();

	/**����Block������*/
	UFUNCTION(BlueprintCallable, Category = "Data")
	int32 GetBlockNum() { return Block.Num(); }

	/**����Page������*/
	UFUNCTION(BlueprintCallable, Category = "Data")
	int32 GetPageNum() { return Page.Num(); }

	/**��ӵ�����*/
	UFUNCTION(BlueprintCallable, Category = "Data")
	void IncreaseBlockNum();

	UFUNCTION(BlueprintCallable, Category = "Data")
	void DecreaseBlockNum();

	/**��Ӷ�Ӧҳ�ŵ�Page*/
	UFUNCTION(BlueprintCallable, Category = "Data")
	void IncreasePage(int32 PageContent);

	/**ɾ��ĩβ��Page*/
	UFUNCTION(BlueprintCallable, Category = "Data")
	void DecreasePage();

	/**�˳���Ϸ*/
	void QuitGame();
};

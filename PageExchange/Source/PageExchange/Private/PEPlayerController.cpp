// Fill out your copyright notice in the Description page of Project Settings.

#include "PEPlayerController.h"
#include "Components/InputComponent.h"
#include "SiumlateCube.h"
#include "Classes/Engine/World.h"
#include "Engine/Engine.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"


APEPlayerController::APEPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void APEPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Optimal", EInputEvent::IE_Pressed, this, &APEPlayerController::Optimal);
	InputComponent->BindAction("FIFO", EInputEvent::IE_Pressed, this, &APEPlayerController::FIFO);
	InputComponent->BindAction("LRU", EInputEvent::IE_Pressed, this, &APEPlayerController::LRU);
	InputComponent->BindAction("QuitGame", EInputEvent::IE_Pressed, this, &APEPlayerController::QuitGame);
}

void APEPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, FString::Printf(TEXT("��Ϸ��ʼ��")), true);
	if (Page.Num() > 0)
	{
		int32 PageNum = Page.Num();
		for (int32 i = 0; i < PageNum; i++)
		{
		   //��ʼ������ҳ��
			PageInfo.SetNum(PageNum);
			PageInfo[i].content = Page[i];
			PageInfo[i].timer = 0;
			PageInfo[i].cubeTag = -1;
		}
		AllPageCubePtr.SetNum(PageNum);
		SpawnAllPageCube();
	}

	if (Block.Num() > 0)
	{
		int32 BlockNum = Block.Num();
		for (int32 i = 0; i < BlockNum; i++)
		{
			Block[i].content = -1;
			Block[i].timer = 0;
			Block[i].cubeTag = -1;
		}
	}
}

void APEPlayerController::Optimal()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("Optimal")), true);

	SpawnAllPageCube();
	int32 exist, space, position;
	for (int i = 0; i < PageInfo.Num(); i++)
	{
		exist = FindExist(i);    //�����Ƿ�ǰpage��Block���Ƿ�����ͬ��
	
		if (exist == -1)
		{
			space = FindSpace();
			if (space != -1)
			{
				Block[space] = PageInfo[i];
				Block[space].cubeTag = i;

				if (i + 1 < PageInfo.Num())
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + space*40.f), false, AllPageCubePtr[i + 1], NULL); //����û����Ҫ�滻�ķ��飬ֱ�ӽ��뼴��
				else
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + space*40.f), false, NULL, NULL);
			}
			else
			{
				for (int k = 0; k < Block.Num(); k++)
					for (int j = i; j < PageInfo.Num(); j++)
					{
						if (Block[k].content != PageInfo[j].content)   
						{
							Block[k].timer = 1000;
						}//���������ã�����TIMERΪһ���ܴ���
						else
						{
							Block[k].timer = j;
							break;
						}
					}

				position = FindReplace();      //�ó�Block���û���λ��
				int32 TempTag = Block[position].cubeTag;
				Block[position] = PageInfo[i];
				Block[position].cubeTag = i;
				if (i + 1 < PageInfo.Num())
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + position*40.f), false, AllPageCubePtr[i + 1], AllPageCubePtr[TempTag]);
				else
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + position*40.f), false, NULL, AllPageCubePtr[TempTag]);
			}
		}
		else
		{
			if (i + 1 < PageInfo.Num())
				AllPageCubePtr[i]->CubeRelative(AllPageCubePtr[i]->GetActorLocation(), true, AllPageCubePtr[i + 1], NULL);
			else
				AllPageCubePtr[i]->CubeRelative(AllPageCubePtr[i]->GetActorLocation(), true, NULL, NULL);
		}
		//FGenericPlatformProcess::Sleep(1000.f);
		//GetWorld()->GetTimerManager()->SetTimer()
	}

	AllPageCubePtr[0]->MoveToBlock = true;    //����ģ��
}

void APEPlayerController::FIFO()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("FIFO")), true);

	SpawnAllPageCube();
	int exist, space, position;
	for (int i = 0; i < PageInfo.Num(); i++)
	{
		exist = FindExist(i);
		if (exist != -1) //�ҵ��Ѵ��ڵģ���ȱҳ
		{
			if (i + 1 < PageInfo.Num())
				AllPageCubePtr[i]->CubeRelative(AllPageCubePtr[i]->GetActorLocation(), true, AllPageCubePtr[i + 1], NULL);
			else
				AllPageCubePtr[i]->CubeRelative(AllPageCubePtr[i]->GetActorLocation(), true, NULL, NULL);
		}
		else
		{
			space = FindSpace();
			if (space != -1)
			{
				Block[space] = PageInfo[i];
				Block[space].cubeTag = i;
				
				if (i + 1 < PageInfo.Num())
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + space*40.f), false, AllPageCubePtr[i + 1], NULL); //����û����Ҫ�滻�ķ��飬ֱ�ӽ��뼴��
				else
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + space*40.f), false, NULL, NULL);
			}
			else
			{
				position = FindReplace();
				int32 TempTag = Block[position].cubeTag;
				Block[position] = PageInfo[i];
				Block[position].cubeTag = i;
				
				if (i + 1 < PageInfo.Num())
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + position*40.f), false, AllPageCubePtr[i + 1], AllPageCubePtr[TempTag]);
				else
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + position*40.f), false, NULL, AllPageCubePtr[TempTag]);
			}
		}
		for (int j = 0; j < Block.Num(); j++)
			Block[j].timer++;     //BLOCK������ҳ��TIMER++
	}
	AllPageCubePtr[0]->MoveToBlock = true;
}

void APEPlayerController::LRU()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("LRU")), true);

	SpawnAllPageCube();
	int exist, space, position;
	for (int i = 0; i < PageInfo.Num(); i++)
	{
		exist = FindExist(i);
		if (exist != -1)
		{
			Block[exist].timer = -1;    //�������ʹ�õģ����ǽ���timer��Ϊ��С

			if (i + 1 < PageInfo.Num())
				AllPageCubePtr[i]->CubeRelative(AllPageCubePtr[i]->GetActorLocation(), true, AllPageCubePtr[i + 1], NULL);
			else
				AllPageCubePtr[i]->CubeRelative(AllPageCubePtr[i]->GetActorLocation(), true, NULL, NULL);
		}
		else
		{
			space = FindSpace();
			if (space != -1)
			{
				Block[space] = PageInfo[i];
				Block[space].cubeTag = i;

				if (i + 1 < PageInfo.Num())
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + space*40.f), false, AllPageCubePtr[i + 1], NULL); //����û����Ҫ�滻�ķ��飬ֱ�ӽ��뼴��
				else
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + space*40.f), false, NULL, NULL);
			}
			else
			{
				position = FindReplace();
				int32 TempTag = Block[position].cubeTag;
				Block[position] = PageInfo[i];
				Block[position].cubeTag = i;

				if (i + 1 < PageInfo.Num())
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + position*40.f), false, AllPageCubePtr[i + 1], AllPageCubePtr[TempTag]);
				else
					AllPageCubePtr[i]->CubeRelative(FVector(0.f, 0.f, 50.f + position*40.f), false, NULL, AllPageCubePtr[TempTag]);
			}
		}
		for (int j = 0; j < Block.Num(); j++)
			Block[j].timer++;
	}
	AllPageCubePtr[0]->MoveToBlock = true;
}

int32 APEPlayerController::FindSpace()
{
	for (int32 i = 0; i < Block.Num(); i++)
	{
		//�ҵ����пռ�ͷ����������±�
		if (Block[i].content == -1)
		{
			return i;       
		}
	}

	return -1;   //û�пռ�ͷ���-1
}

int32 APEPlayerController::FindExist(int32 curpage)
{
	for (int32 i = 0; i < Block.Num(); i++)
	{
		if (Block[i].content == PageInfo[curpage].content)
			return i;
	}

	return -1;     //û���Ѵ��ڵľͷ���-1
}

int32 APEPlayerController::FindReplace()
{
	int32 pos = 0;
	for (int32 i = 0; i < Block.Num(); i++)
	{
		if (Block[i].timer >= Block[pos].timer)
			pos = i;    //�ҵ�timer���ֵ���±꣬������λ��
	}

	return pos;
}

void APEPlayerController::SpawnAllPageCube()
{
	for (int32 i = 0; i < AllPageCubePtr.Num(); i++)
	{
		if (AllPageCubePtr[i] != NULL)
			AllPageCubePtr[i]->DestroyActor();   //���ٸ�ָ������Ӧ��Actor����ֹ�ڴ�й©
		AllPageCubePtr[i] = NULL;
	}
	//ͬʱ���Block
	int32 BlockNum = Block.Num();
	for (int32 i = 0; i < BlockNum; i++)
	{
		Block[i].content = -1;
		Block[i].timer = 0;
		Block[i].cubeTag = -1;
	}

	//����Page���������
	int32 PageNum = Page.Num();
	if (PageNum > 0)
	{
		for (int32 i = 0; i < PageNum; i++)
		{
			//��ʼ������ҳ��
			PageInfo.SetNum(PageNum);
			PageInfo[i].content = Page[i];
			PageInfo[i].timer = 0;
			PageInfo[i].cubeTag = -1;
		}
		AllPageCubePtr.SetNum(PageNum);

		//��������cube
		for (int32 i = 0; i < PageInfo.Num(); i++)
		{
			//��ʼ����cube
			ASiumlateCube* cube = GetWorld()->SpawnActorDeferred<ASiumlateCube>(SimCube, FVector(0.f, PageInfo.Num()*40.f / 2 - 40.f*i, 50.f + Block.Num()*40.f), FRotator(0.f, 0.f, 0.f));
			if (cube != NULL)
			{
				UStaticMeshComponent* CurMesh = cube->GetMesh();
				if (CurMesh)
				{
					//���趯̬���ʣ������������е�����
					UMaterialInstanceDynamic* MID = CurMesh->CreateAndSetMaterialInstanceDynamic(0);     //��ȡcube��������Ӧ�Ķ�̬����
					MID->SetScalarParameterValue("U_Scroll", (PageInfo[i].content - 1) % 4);    //����FName�Զ�̬�����еı������и�ֵ���Ӷ�ʵ�ַ����ϲ�ͬ������
					MID->SetScalarParameterValue("V_Scroll", (PageInfo[i].content - 1) / 4);
				}
				//��������
				UGameplayStatics::FinishSpawningActor(cube, FTransform(FVector(0.f, PageInfo.Num()*40.f / 2 - 40.f*i, 50.f + Block.Num()*40.f)));
			}
			AllPageCubePtr[i] = cube;
		}
	}
}

void APEPlayerController::IncreaseBlockNum()
{
	FPageInfo elem;
	Block.Add(elem);

	SpawnAllPageCube();
}

void APEPlayerController::DecreaseBlockNum()
{
	Block.RemoveAt(Block.Num() - 1);

	SpawnAllPageCube();
}

void APEPlayerController::IncreasePage(int32 PageContent)
{
	Page.Add(PageContent);    //���ҳ��

	SpawnAllPageCube();
}

void APEPlayerController::DecreasePage()
{
	if (Page.Num() >= 1)   //��ֹԽ�����
	{
		Page.RemoveAt(Page.Num() - 1);

		SpawnAllPageCube();
	}
}

void APEPlayerController::QuitGame()
{
	this->ConsoleCommand("quit");      //�������̨���� quit
}
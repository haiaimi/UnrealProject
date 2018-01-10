// Fill out your copyright notice in the Description page of Project Settings.

#include "SiumlateCube.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Classes/Engine/World.h"

// Sets default values
ASiumlateCube::ASiumlateCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SimulateCubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	RootComponent = SimulateCubeMesh;

	MoveToBlock = false;
	DropFromBlock = false;
	JustDestroy = false;
}

// Called when the game starts or when spawned
void ASiumlateCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame��ÿ֡����
void ASiumlateCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveToBlock)
	{
		FVector NextPos = FMath::VInterpTo(GetActorLocation(), DestToBlock, DeltaTime, 5.f);     //ʹ�����Բ�ֵ�����������һ֡λ�ã������ƶ�
		SetActorLocation(NextPos);     //����Actor��λ��

		//�����鵽������Ŀ��λ�ã��Ϳ��Ի�������һ����������ƶ�
		if (NextPos == DestToBlock)
		{
			if (NextCube != NULL)    //�����жϷǿգ���ָ����������
			{
				NextCube->MoveToBlock = true;   //������һ����������ƶ�

				if (NextCube->ReplaceCube != NULL)
					NextCube->ReplaceCube->DropFromBlock = true;  //��һ������������ķ���ҲҪ�ƶ�
			}

			//������Block���Ѵ�����Ӧҳ�飬ֱ��ɾ������
			if (JustDestroy)
				DestroyActor();   

			MoveToBlock = false;
		}
	}

	//�������Ƴ�Block
	if (DropFromBlock)
	{
		//ÿ֡��-x�����ƶ�������λ����С��-50����Ϊ��cube��������ģ�⣬ʹ�����������䣬����2���ɾ��
		SetActorLocation(GetActorLocation() + FVector(-2.f, 0.f, 0.f));
		if (GetActorLocation().X <= -50.f)
		{
			SimulateCubeMesh->SetSimulatePhysics(true);
			if(GetWorld())
				GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &ASiumlateCube::DestroyActor, 2.0f, false);     //���ö�ʱ������2���ɾ����cube
			DropFromBlock = false;
		}
	}

}

void ASiumlateCube::DestroyActor()
{
	Destroy();
}

void ASiumlateCube::CubeRelative(FVector Dst, bool JustDestroy, ASiumlateCube* NextCube, ASiumlateCube* ReplaceCube)
{
	this->NextCube = NextCube;
	this->ReplaceCube = ReplaceCube;
	this->DestToBlock = Dst;
	this->JustDestroy = JustDestroy;
}

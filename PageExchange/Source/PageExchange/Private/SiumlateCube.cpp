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

// Called every frame，每帧调用
void ASiumlateCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveToBlock)
	{
		FVector NextPos = FMath::VInterpTo(GetActorLocation(), DestToBlock, DeltaTime, 5.f);     //使用线性插值设置物体的下一帧位置，进行移动
		SetActorLocation(NextPos);     //设置Actor新位置

		//当方块到达最终目标位置，就可以唤起其下一个方块进行移动
		if (NextPos == DestToBlock)
		{
			if (NextCube != NULL)    //不许判断非空，空指针引擎会崩溃
			{
				NextCube->MoveToBlock = true;   //设置下一个方块可以移动

				if (NextCube->ReplaceCube != NULL)
					NextCube->ReplaceCube->DropFromBlock = true;  //下一个方块所替代的方块也要移动
			}

			//这是在Block中已存在相应页块，直接删除即可
			if (JustDestroy)
				DestroyActor();   

			MoveToBlock = false;
		}
	}

	//下面是移出Block
	if (DropFromBlock)
	{
		//每帧沿-x方向移动两个单位，当小于-50，就为该cube加上物理模拟，使其能正常掉落，并在2秒后删除
		SetActorLocation(GetActorLocation() + FVector(-2.f, 0.f, 0.f));
		if (GetActorLocation().X <= -50.f)
		{
			SimulateCubeMesh->SetSimulatePhysics(true);
			if(GetWorld())
				GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &ASiumlateCube::DestroyActor, 2.0f, false);     //设置定时器，在2秒后删除该cube
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

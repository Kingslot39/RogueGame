// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadTree.h"

// Sets default values
ADeadTree::ADeadTree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TreeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tree Mesh"));
	RootComponent = TreeMesh;

}

void ADeadTree::SetUpTree()
{
	GetWorldTimerManager().SetTimer(TreeTime, this, &ADeadTree::DestroyTree, 5.0f, false);
}

void ADeadTree::DestroyTree()
{
	Destroy();
}

// Called when the game starts or when spawned
void ADeadTree::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeadTree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


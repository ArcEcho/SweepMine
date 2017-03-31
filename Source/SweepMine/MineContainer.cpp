// Fill out your copyright notice in the Description page of Project Settings.

#include "SweepMine.h"
#include "MineContainer.h"


// Sets default values
AMineContainer::AMineContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMineContainer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMineContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


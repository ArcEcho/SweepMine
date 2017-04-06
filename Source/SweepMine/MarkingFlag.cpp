// Fill out your copyright notice in the Description page of Project Settings.

#include "SweepMine.h"
#include "MarkingFlag.h"
#include "SweepMineGameInstance.h"


// Sets default values
AMarkingFlag::AMarkingFlag()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshSource(TEXT("/Game/SweepMine/Meshes/SM_Flag"));

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
    MeshComp->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
    MeshComp->SetSimulatePhysics(true);
    MeshComp->SetStaticMesh(MeshSource.Object);
}

// Called when the game starts or when spawned
void AMarkingFlag::BeginPlay()
{
    Super::BeginPlay();

    auto GameInstanceRef = Cast<USweepMineGameInstance>(GetGameInstance());
    if (GameInstanceRef == nullptr)
    {
        return;
    }

    GameInstanceRef->EndGameDelegate.AddDynamic(this, &AMarkingFlag::OnEndGame);

}

// Called every frame
void AMarkingFlag::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AMarkingFlag::OnEndGame()
{
    Destroy();
}


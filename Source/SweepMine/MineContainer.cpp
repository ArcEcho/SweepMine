// Fill out your copyright notice in the Description page of Project Settings.

#include "SweepMine.h"
#include "MineContainer.h"
#include "SweepMineGameInstance.h"
#include "Components/TextRenderComponent.h"
#include "MarkingFlag.h"
#include "MineExplosionCameraShake.h"

#define LOCTEXT_NAMESPACE "MineContainer"

// Structure to hold one-time initialization
struct FConstructorStatics
{
    ConstructorHelpers::FObjectFinderOptional<UStaticMesh> SM_Cube;
    ConstructorHelpers::FObjectFinderOptional<UMaterial> M_Font;
    ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> MInst_MineContainer_HasMineInNeighborhood;
    ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> MInst_MineContainer_NoMineInNeighborhood;
    ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> MInst_MineContainer_Unchecked;
    ConstructorHelpers::FObjectFinderOptional<UParticleSystem> P_Explosion;
    ConstructorHelpers::FObjectFinderOptional<USoundBase> ExplosionSoundBase;
    ConstructorHelpers::FObjectFinderOptional<USoundBase>  ForbidMarkingSoundBase;
    ConstructorHelpers::FObjectFinderOptional<USoundBase>  MarkedSoundBase;
    ConstructorHelpers::FObjectFinderOptional<USoundBase>  SuccessfulDectectedSoundBase;

    FConstructorStatics()
        : SM_Cube(TEXT("/Game/SweepMine/Meshes/SM_Cube"))
        , M_Font(TEXT("/Game/SweepMine/Materials/M_Font"))
        , MInst_MineContainer_HasMineInNeighborhood(TEXT("/Game/SweepMine/Materials/MInst_MineContainer_HasMineInNeighborhood"))
        , MInst_MineContainer_NoMineInNeighborhood(TEXT("/Game/SweepMine/Materials/MInst_MineContainer_NoMineInNeighborhood"))
        , MInst_MineContainer_Unchecked(TEXT("/Game/SweepMine/Materials/MInst_MineContainer_Unchecked"))
        , P_Explosion(TEXT("/Game/SweepMine/Particles/P_Explosion"))
        , ExplosionSoundBase(TEXT("/Game/SweepMine/Audio/Explosion01"))
        , ForbidMarkingSoundBase(TEXT("/Game/SweepMine/Audio/Blueprints_TextPop03"))
        , MarkedSoundBase(TEXT("/Game/SweepMine/Audio/Building_Attachment_Stereo"))
        , SuccessfulDectectedSoundBase(TEXT("/Game/SweepMine/Audio/Blueprints_TextPop01"))
    {
    }
};

// Sets default values
AMineContainer::AMineContainer()
    :bHasMine{ false }, NeighborMineCount{ 0 }, bHasBeenChecked{ false }, bHasBeenMarked{ false }
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    static FConstructorStatics ConstructorStatics;

    UncheckedMaterial = ConstructorStatics.MInst_MineContainer_Unchecked.Get();
    HasMineInNeighborhoodMaterial = ConstructorStatics.MInst_MineContainer_HasMineInNeighborhood.Get();
    NoMineInNeighborhoodMaterial = ConstructorStatics.MInst_MineContainer_NoMineInNeighborhood.Get();

    ExplosionParticleSystem = ConstructorStatics.P_Explosion.Get();
    ExplosionSoundBase = ConstructorStatics.ExplosionSoundBase.Get();

    ForbidMarkingSoundBase = ConstructorStatics.ForbidMarkingSoundBase.Get();
    MarkedSoundBase = ConstructorStatics.MarkedSoundBase.Get();
    SuccessfulDectectedSoundBase = ConstructorStatics.SuccessfulDectectedSoundBase.Get();

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComponent;
    MeshComponent->SetSimulatePhysics(true);
    MeshComponent->SetCollisionProfileName(FName("PhysicActor"));
    MeshComponent->SetMaterial(0, UncheckedMaterial);
    MeshComponent->SetStaticMesh(ConstructorStatics.SM_Cube.Get());

    NeighborMineCountText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NeighborMineCountText"));
    NeighborMineCountText->SetRelativeRotation(FRotator(90.0f, 180.0f, 0.0f));
    NeighborMineCountText->SetRelativeLocation(FVector(0.0f, 0.0f, 52.0f));  //Set Z with 52 instead of 50 to avoid z conflict.
    NeighborMineCountText->SetText(FText::Format(LOCTEXT("NeighborMineCount", "{0}"), FText::AsNumber(0)));
    NeighborMineCountText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    NeighborMineCountText->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
    NeighborMineCountText->SetTextRenderColor(FColor::Red);
    NeighborMineCountText->SetWorldSize(50.0f);
    NeighborMineCountText->SetVisibility(false);
    NeighborMineCountText->SetupAttachment(RootComponent);
    NeighborMineCountText->SetTextMaterial(ConstructorStatics.M_Font.Get());
}


void AMineContainer::OnEndGame()
{
    Destroy();
}

void AMineContainer::ShowNeighborMineCount()
{
    if (NeighborMineCount != 0)
    {
        NeighborMineCountText->SetVisibility(true);
        NeighborMineCountText->SetText(FText::Format(LOCTEXT("NeighborMineCount", "{0}"), FText::AsNumber(NeighborMineCount)));

        FColor Color;
        switch (NeighborMineCount)
        {
        case 1:
            Color = FColor::Black;
            break;
        case 2:
            Color = FColor::Blue;
            break;
        case 3:
            Color = FColor::Green;
            break;
        case 4:
            Color = FColor::Orange;
            break;
        case 5:
            Color = FColor::Purple;
            break;
        case 6:
            Color = FColor::Silver;
            break;
        case 7:
            Color = FColor::Turquoise;
            break;
        case 8:
            Color = FColor::Red;
            break;
        default:
            Color = FColor::White;
            break;
        }
        NeighborMineCountText->SetTextRenderColor(Color);

        MeshComponent->SetMaterial(0, HasMineInNeighborhoodMaterial);
    }
    else
    {
        MeshComponent->SetMaterial(0, NoMineInNeighborhoodMaterial);
    }
}

void AMineContainer::ToggleMark()
{
    if (bHasBeenChecked)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), ForbidMarkingSoundBase);
        return;
    }

    auto GameInstanceRef = Cast<USweepMineGameInstance>(GetGameInstance());

    //标记满了，且即将再标记
    if (GameInstanceRef->GetPredictedReminingMineCount() == 0 && !bHasBeenMarked)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), ForbidMarkingSoundBase);
        return;
    }

    //没有标记且不想再标记了
    if (GameInstanceRef->GetPredictedReminingMineCount() ==  GameInstanceRef->GetMineCount() && bHasBeenMarked)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), ForbidMarkingSoundBase);
        return;
    }

    bHasBeenMarked = !bHasBeenMarked;
    GameInstanceRef->StatisticMarkedMineContainer(bHasBeenMarked);

    if (bHasBeenMarked && MarkingFlag == nullptr)
    {
        FVector NewLocation = GetActorLocation();
        NewLocation.Z += 52.0f;
        FTransform NewTransform;
        NewTransform.SetTranslationAndScale3D(NewLocation, FVector(0.5f, 0.5f, 0.5f));
        MarkingFlag = GetWorld()->SpawnActor<AMarkingFlag>(AMarkingFlag::StaticClass(), NewTransform);
    }


    MarkingFlag->SetActorHiddenInGame(!bHasBeenMarked);
    UGameplayStatics::PlaySound2D(GetWorld(), MarkedSoundBase);
}

void AMineContainer::Detect()
{
    auto GameInstanceRef = Cast<USweepMineGameInstance>(GetGameInstance());
    if (bHasMine)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticleSystem, GetActorLocation());
        UGameplayStatics::PlaySound2D(GetWorld(), ExplosionSoundBase);
        UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->PlayCameraShake(UMineExplosionCameraShake::StaticClass());
        GameInstanceRef->GameOver();
        return;
    }

    UGameplayStatics::PlaySound2D(GetWorld(), SuccessfulDectectedSoundBase);
    GameInstanceRef->TryDetectNeighborMineContainer(ColumnIndex, RowIndex);
}

void AMineContainer::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void AMineContainer::BeginPlay()
{
    Super::BeginPlay();

    auto GameInstanceRef = Cast<USweepMineGameInstance>(GetGameInstance());
    if (GameInstanceRef == nullptr)
    {
        return;
    }

    GameInstanceRef->EndGameDelegate.AddDynamic(this, &AMineContainer::OnEndGame);
}


// Called every frame
void AMineContainer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}


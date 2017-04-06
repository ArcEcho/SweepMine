// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MineContainer.generated.h"

UCLASS()
class SWEEPMINE_API AMineContainer : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AMineContainer();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
        void OnEndGame();

    FORCEINLINE void SetNeighborMineCount(int32 InMineCount) { NeighborMineCount = InMineCount; };

    FORCEINLINE int32 GetNeighborMineCount() { return NeighborMineCount; };

    FORCEINLINE void SetHasMine(bool InValue) { bHasMine = InValue; };

    FORCEINLINE bool GetHasMine() { return bHasMine; };

    FORCEINLINE void SetHasBeenChecked(bool InValue) { bHasBeenChecked = InValue; };

    FORCEINLINE bool GetHasBeenChecked() { return bHasBeenChecked; };

    FORCEINLINE void SetHasBeenMarked(bool InValue) { bHasBeenMarked = InValue; };

    FORCEINLINE bool GetHasBeenMarked() { return bHasBeenMarked; };

    FORCEINLINE void SetColumnIndex(int32 InValue) { ColumnIndex = InValue; };

    FORCEINLINE int32 GetColumnIndex() { return ColumnIndex; };

    FORCEINLINE void SetRowIndex(int32 InValue) { RowIndex = InValue; };

    FORCEINLINE int32 GetRowIndex() { return RowIndex; };

    void ShowNeighborMineCount();

    void ToggleMark();

    void Detect();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(VisibleDefaultsOnly)
        UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleDefaultsOnly)
        UTextRenderComponent* NeighborMineCountText;


private:
    bool bHasMine;

    bool bHasBeenMarked;

    bool bHasBeenChecked;

    int32 ColumnIndex;
    int32 RowIndex;

    int32 NeighborMineCount;

    UMaterialInstance* UncheckedMaterial;
    UMaterialInstance* HasMineInNeighborhoodMaterial;
    UMaterialInstance* NoMineInNeighborhoodMaterial;

    FDelegateHandle EndGameDelegateHandle;

    class AMarkingFlag* MarkingFlag;

    UParticleSystem* ExplosionParticleSystem;

    USoundBase* ExplosionSoundBase;
    USoundBase* ForbidMarkingSoundBase;
    USoundBase* MarkedSoundBase;
    USoundBase* SuccessfulDectectedSoundBase;

};

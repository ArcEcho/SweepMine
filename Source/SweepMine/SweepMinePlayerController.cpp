// Fill out your copyright notice in the Description page of Project Settings.

#include "SweepMine.h"
#include "SweepMinePlayerController.h"
#include "SweepMineGameInstance.h"
#include "MineContainer.h"




ASweepMinePlayerController::ASweepMinePlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
}

void ASweepMinePlayerController::OnLeftMouseButtonClicked()
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, false, HitResult);

    auto MineContainerRef = Cast<AMineContainer>(HitResult.GetActor());
    if (MineContainerRef == nullptr)
    {
        return;
    }

    MineContainerRef->Detect();
}

void ASweepMinePlayerController::OnRightMouseButtonClicked()
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, false, HitResult);

    auto MineContainerRef = Cast<AMineContainer>(HitResult.GetActor());
    if (MineContainerRef == nullptr)
    {
        return;
    }

    MineContainerRef->ToggleMark();
}

void ASweepMinePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction(FName("LMB"), IE_Pressed, this, &ASweepMinePlayerController::OnLeftMouseButtonClicked);
    InputComponent->BindAction(FName("RMB"), IE_Pressed, this, &ASweepMinePlayerController::OnRightMouseButtonClicked);
}

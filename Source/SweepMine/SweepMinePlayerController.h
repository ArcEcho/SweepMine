// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SweepMinePlayerController.generated.h"

/**
 *
 */
UCLASS()
class SWEEPMINE_API ASweepMinePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASweepMinePlayerController(); 

    UFUNCTION()
        void OnLeftMouseButtonClicked();

    UFUNCTION()
        void OnRightMouseButtonClicked();

protected:
    virtual void SetupInputComponent() override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "SweepMine.h"
#include "SweepMineGameInstance.h"
#include "MainLevelScriptActor.h"


void AMainLevelScriptActor::BeginPlay()
{
    auto GameInstanceRef = Cast<USweepMineGameInstance>(GetGameInstance());
    if (GameInstanceRef == nullptr)
    {
        return;
    }

    GameInstanceRef->ShowStartMenu();
}

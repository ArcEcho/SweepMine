// Fill out your copyright notice in the Description page of Project Settings.

#include "SweepMine.h"
#include "SweepMinePlayerController.h"
#include "SweepMineGameModeBase.h"




ASweepMineGameModeBase::ASweepMineGameModeBase()
{
    DefaultPawnClass = nullptr;
    HUDClass = nullptr;
    PlayerControllerClass = ASweepMinePlayerController::StaticClass();
}

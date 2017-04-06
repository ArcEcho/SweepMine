// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "MineContainer.h"
#include "SweepMineGameInstance.generated.h"


UENUM(BlueprintType)
enum class EState : uint8
{
    Startup,
    StartMenu,
    Playing,
    GameOver,
    Victory
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndGameDelegateSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPredictedRemainingMineCountChangedDelegateSignature);

/**
 *
 */
UCLASS()
class SWEEPMINE_API USweepMineGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    USweepMineGameInstance();

    UFUNCTION(BlueprintCallable)
        void ShowStartMenu();

    UFUNCTION(BlueprintCallable)
        void SetGameDifficulty(int32 Difficulty);

    UFUNCTION(BlueprintCallable)
        void StartGame();

    UFUNCTION(BlueprintCallable)
        void ShowGameOverUI();

    UFUNCTION(BlueprintCallable)
        void ShowGameVictoryUI();

    UFUNCTION()
        void GameOver();

    UFUNCTION(BlueprintCallable)
        int32 GetPredictedReminingMineCount();

    int32 GetMineCount();

    void TryDetectNeighborMineContainer(int32 ColumnIndex, int32 RowIndex);

    bool StatisticMarkedMineContainer(bool ToggleMarkFlag);

    FEndGameDelegateSignature EndGameDelegate;

    UPROPERTY(VisibleAnywhere, BlueprintAssignable )
        FPredictedRemainingMineCountChangedDelegateSignature PredictedRemainingMineCountChangedDelegate;

private:

    void DistributeMines();

    void CreateMineFields();

    void TransitState(EState DesiredGameState);

    void SetGameParameterByDifficulty();

    int32 CheckContainer(int32 ColumnIndex, int32 RowIndex);

    bool CheckVictory();

    //以下是游戏的一些参数
    int32 GameDifficulty;

    int32 MineFieldColumn;

    int32 MineFieldRow;

    float MineContainerSpacing;

    int32 MineCount;

    TArray<TArray<class AMineContainer*>> MineContainerArray;

    TSet<int32> DistributeResult;

    int32 RemainngEmptyContainerCount;

    int32 MarkedMineContainerCount;

    EState CurrentGameState;

    TSubclassOf<class UUserWidget> MainMenuWidgetClass;
    class UUserWidget* MainMenuWidget;

    TSubclassOf<class UUserWidget> GameOverWidgetClass;
    class UUserWidget* GameOverWidget;

    TSubclassOf<class UUserWidget> VictoryWidgetClass;
    class UUserWidget* VictoryWidget;

    TSubclassOf<class UUserWidget> PlayingWidgetClass;
    class UUserWidget* PlayingWidget;
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "SweepMine.h"
#include "SweepMineGameInstance.h"
#include "MineContainer.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"


USweepMineGameInstance::USweepMineGameInstance()
    : GameDifficulty{ 0 }, MineFieldColumn{ 0 }, MineFieldRow{ 0 }, MineContainerSpacing{ 110.0f },
    MineCount{ 0 }, RemainngEmptyContainerCount{ 0 }, MarkedMineContainerCount{ 0 }, CurrentGameState{ EState::Startup }
{
    //Find and the blueprint UMG class
    static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuClassSource(TEXT("/Game/SweepMine/UI/WBP_MainMenu"));
    MainMenuWidgetClass = MainMenuClassSource.Succeeded() ? MainMenuClassSource.Class : nullptr;

    static ConstructorHelpers::FClassFinder<UUserWidget> GameOverWidgetClassSource(TEXT("/Game/SweepMine/UI/WBP_GameOver"));
    GameOverWidgetClass = GameOverWidgetClassSource.Succeeded() ? GameOverWidgetClassSource.Class : nullptr;

    static ConstructorHelpers::FClassFinder<UUserWidget> VictoryWidgetClassSource(TEXT("/Game/SweepMine/UI/WBP_Victory"));
    VictoryWidgetClass = VictoryWidgetClassSource.Succeeded() ? VictoryWidgetClassSource.Class : nullptr;

    static ConstructorHelpers::FClassFinder<UUserWidget> PlayingWidgetClassSource(TEXT("/Game/SweepMine/UI/WBP_Playing"));
    PlayingWidgetClass = PlayingWidgetClassSource.Succeeded() ? PlayingWidgetClassSource.Class : nullptr;
}

void USweepMineGameInstance::SetGameDifficulty(int32 Difficulty)
{
    GameDifficulty = Difficulty;
}

void USweepMineGameInstance::StartGame()
{
    SetGameParameterByDifficulty();
    DistributeMines();
    CreateMineFields();

    TransitState(EState::Playing);
}

void USweepMineGameInstance::ShowGameOverUI()
{
    TransitState(EState::GameOver);
}

void USweepMineGameInstance::ShowGameVictoryUI()
{
    TransitState(EState::Victory);
}

void USweepMineGameInstance::GameOver()
{
    EndGameDelegate.Broadcast();

    ShowGameOverUI();
}

int32 USweepMineGameInstance::GetPredictedReminingMineCount()
{
    return MineCount - MarkedMineContainerCount;
}

int32 USweepMineGameInstance::GetMineCount()
{
    return MineCount;
}

void USweepMineGameInstance::TryDetectNeighborMineContainer(int32 ColumnIndex, int32 RowIndex)
{
    //Determine whether it is out of range.
    if (ColumnIndex < 0 || ColumnIndex >= MineFieldColumn || RowIndex < 0 || RowIndex >= MineFieldRow)
    {
        return;
    }

    auto MineContainerRef = MineContainerArray[ColumnIndex][RowIndex];
    if (MineContainerRef->GetHasBeenChecked() || MineContainerRef->GetHasBeenMarked() || MineContainerRef->GetHasMine())
    {
        return;
    }

    MineContainerRef->ShowNeighborMineCount();
    MineContainerRef->SetHasBeenChecked(true);
    RemainngEmptyContainerCount--;

    if (CheckVictory())
    {
        EndGameDelegate.Broadcast();
        ShowGameVictoryUI();
        return;
    }

    if (MineContainerRef->GetNeighborMineCount() == 0)
    {
        //DFS 
        TryDetectNeighborMineContainer(ColumnIndex - 1, RowIndex - 1); TryDetectNeighborMineContainer(ColumnIndex, RowIndex - 1); TryDetectNeighborMineContainer(ColumnIndex + 1, RowIndex - 1);
        TryDetectNeighborMineContainer(ColumnIndex - 1, RowIndex);                                                                TryDetectNeighborMineContainer(ColumnIndex + 1, RowIndex);
        TryDetectNeighborMineContainer(ColumnIndex - 1, RowIndex + 1); TryDetectNeighborMineContainer(ColumnIndex, RowIndex + 1); TryDetectNeighborMineContainer(ColumnIndex + 1, RowIndex + 1);
    }
}

bool USweepMineGameInstance::StatisticMarkedMineContainer(bool ToggleMarkFlag)
{
    auto NewCount = MarkedMineContainerCount;
    NewCount += ToggleMarkFlag ? 1 : -1;

    if (NewCount < 0 || NewCount > MineCount)
    {
        return false;
    }

    MarkedMineContainerCount = NewCount;

    PredictedRemainingMineCountChangedDelegate.Broadcast();
    return true;
}

void USweepMineGameInstance::DistributeMines()
{
    DistributeResult.Empty(0);

    TArray<int32> TempArray;
    TempArray.Init(0, MineFieldColumn * MineFieldRow);
    for (int32 i = 0; i < TempArray.Num(); i++)
    {
        TempArray[i] = i;
    }

    for (int32 i = 0; i < MineCount; i++)
    {
        int32 TempIndex = FMath::RandHelper(TempArray.Num());
        DistributeResult.Add(TempArray[TempIndex]);
        TempArray.RemoveAt(TempIndex, 1, true);
    }
}

void USweepMineGameInstance::CreateMineFields()
{
    MineContainerArray.Empty(0);

    for (int i = 0; i < MineFieldColumn; i++)
    {
        TArray<AMineContainer *> TempRowArray;
        for (int j = 0; j < MineFieldRow; j++)
        {
            auto Index = j + i * MineFieldRow;
            auto  Location = FVector((static_cast<float>(i) - static_cast<float>(MineFieldColumn) / 2.0) * MineContainerSpacing,
                (static_cast<float>(j) - static_cast<float>(MineFieldRow) / 2.0) * MineContainerSpacing, 0);

            //Try to mock the BP's expose on spawn.
            auto SpawnedActorRef = GetWorld()->SpawnActorDeferred<AMineContainer>(AMineContainer::StaticClass(), FTransform(FRotator::ZeroRotator, Location));
            if (SpawnedActorRef != nullptr)
            {
                SpawnedActorRef->SetHasMine(DistributeResult.Contains(Index));
                SpawnedActorRef->SetColumnIndex(i);
                SpawnedActorRef->SetRowIndex(j);
                UGameplayStatics::FinishSpawningActor(SpawnedActorRef, FTransform(FRotator::ZeroRotator, Location));
            }

            //Also it can be written like in some case
            //auto SpawnedActorRef = GetWorld()->SpawnActor<AMineContainer>(AMineContainer::StaticClass(), FTransform(FRotator::ZeroRotator, Location));
            //SpawnedActorRef->bHasMine = DistributeResult.Contains(Index);

            TempRowArray.Add(SpawnedActorRef);
        }

        MineContainerArray.Add(TempRowArray);
    }

    //Statistc neighbor mine count;
    for (int i = 0; i < MineFieldColumn; i++)
    {
        for (int j = 0; j < MineFieldRow; j++)
        {
            int32 Count = 0;

            Count += CheckContainer(i - 1, j - 1); Count += CheckContainer(i, j - 1); Count += CheckContainer(i + 1, j - 1);
            Count += CheckContainer(i - 1, j);                                        Count += CheckContainer(i + 1, j);
            Count += CheckContainer(i - 1, j + 1); Count += CheckContainer(i, j + 1); Count += CheckContainer(i + 1, j + 1);

            MineContainerArray[i][j]->SetNeighborMineCount(Count);
        }
    }
}

void USweepMineGameInstance::ShowStartMenu()
{
    GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Blue, TEXT("ShowStartMenu"));
    TransitState(EState::StartMenu);
}

void  USweepMineGameInstance::TransitState(EState DesiredGameState)
{
    if (CurrentGameState == DesiredGameState)
    {
        return;
    }

    switch (CurrentGameState)
    {
    case EState::Startup:
        break;
    case EState::StartMenu:
        MainMenuWidget->RemoveFromParent();
        break;
    case EState::Playing:
        PlayingWidget->RemoveFromParent();
        break;
    case EState::GameOver:
        GameOverWidget->RemoveFromParent();
        break;
    case EState::Victory:
        VictoryWidget->RemoveFromParent();
        break;
    default:
        break;
    }

    CurrentGameState = DesiredGameState;

    //Create widget or just add it to viewport.
    auto PlayerControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    switch (CurrentGameState)
    {
    case EState::Startup:
        break;
    case EState::StartMenu:
        if (MainMenuWidget == nullptr)
        {
            MainMenuWidget = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
        }
        if (MainMenuWidget != nullptr)
        {
            MainMenuWidget->AddToViewport(0);
            PlayerControllerRef->SetInputMode(FInputModeUIOnly());
        }
        break;
    case EState::Playing:
        if (PlayingWidget == nullptr)
        {
            PlayingWidget = CreateWidget<UUserWidget>(this, PlayingWidgetClass);
        }
        if (PlayingWidget != nullptr)
        {
            PlayingWidget->AddToViewport(0);
            PlayerControllerRef->SetInputMode(FInputModeGameAndUI());
        }
        break;
    case EState::GameOver:
        if (GameOverWidget == nullptr)
        {
            GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
        }
        if (GameOverWidget != nullptr)
        {
            GameOverWidget->AddToViewport(0);
            PlayerControllerRef->SetInputMode(FInputModeUIOnly());
        }
        break;
    case EState::Victory:
        if (VictoryWidget == nullptr)
        {
            VictoryWidget = CreateWidget<UUserWidget>(this, VictoryWidgetClass);
        }
        if (VictoryWidget != nullptr)
        {
            VictoryWidget->AddToViewport(0);
            PlayerControllerRef->SetInputMode(FInputModeUIOnly());
        }
        break;
    default:
        break;
    }
}

void USweepMineGameInstance::SetGameParameterByDifficulty()
{
    MineFieldColumn = MineFieldRow = (GameDifficulty + 1) * 5;
    MineCount = MineFieldRow * MineFieldColumn / 5;
    RemainngEmptyContainerCount = MineFieldColumn * MineFieldRow - MineCount;
    MarkedMineContainerCount = 0;
}

int32 USweepMineGameInstance::CheckContainer(int32 ColumnIndex, int32 RowIndex)
{
    //Determine whether it is out of range.
    if (ColumnIndex < 0 || ColumnIndex >= MineFieldColumn || RowIndex < 0 || RowIndex >= MineFieldRow)
    {
        return 0;
    }

    return MineContainerArray[ColumnIndex][RowIndex]->GetHasMine() ? 1 : 0;
}

bool USweepMineGameInstance::CheckVictory()
{
    if (RemainngEmptyContainerCount == 0)
    {
        return true;
    }

    return false;
}

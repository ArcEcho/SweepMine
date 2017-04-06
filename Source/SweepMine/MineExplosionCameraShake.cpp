// Fill out your copyright notice in the Description page of Project Settings.

#include "SweepMine.h"
#include "MineExplosionCameraShake.h"




UMineExplosionCameraShake::UMineExplosionCameraShake()
{
    OscillationDuration = 0.2f;

    RotOscillation.Pitch.Amplitude = 50.0f;
    RotOscillation.Pitch.Frequency = 3.0f;
    RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

    RotOscillation.Yaw.Amplitude = 50.0f;
    RotOscillation.Yaw.Frequency = 3.0f;
    RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

    RotOscillation.Roll.Amplitude = 50.0f;
    RotOscillation.Roll.Frequency = 3.0f;
    RotOscillation.Roll.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;


}

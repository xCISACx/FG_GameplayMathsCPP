// Fill out your copyright notice in the Description page of Project Settings.


#include "FGGM_ChargeCameraShake.h"

UFGGM_ChargeCameraShake::UFGGM_ChargeCameraShake()
{
	// Generate Perlin noise for the camera shake
	float Rand = FMath::RandRange(15, 30);
	float NoiseX = FMath::PerlinNoise3D(FVector(Rand, 0.0f, 0.0f));
	float NoiseY = FMath::PerlinNoise3D(FVector(0.0f, Rand, 0.0f));
	float NoiseZ = FMath::PerlinNoise3D(FVector(0.0f, 0.0f, Rand));

	OscillationDuration= 0.5f;
	OscillationBlendInTime = 0.1f;
	OscillationBlendOutTime = 0.2f;
	/*RotOscillation.Pitch.Amplitude = NoiseX;
	RotOscillation.Yaw.Amplitude = NoiseY;
	RotOscillation.Roll.Amplitude = NoiseZ;*/
	/*LocOscillation.X.Amplitude = 25.0f;
	LocOscillation.X.Frequency = 1.0f;
	LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.X.Waveform = EOscillatorWaveform::PerlinNoise;
	
	LocOscillation.Y.Amplitude = 15.0f;
	LocOscillation.Y.Frequency = 5.0f;
	LocOscillation.Y.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Y.Waveform = EOscillatorWaveform::SineWave;*/
	
	LocOscillation.Z.Amplitude = 0.5f;
	LocOscillation.Z.Frequency = 10.0f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Z.Waveform = EOscillatorWaveform::SineWave;
}


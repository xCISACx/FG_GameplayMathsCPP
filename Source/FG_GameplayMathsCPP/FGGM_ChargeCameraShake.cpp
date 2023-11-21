// Fill out your copyright notice in the Description page of Project Settings.


#include "FGGM_ChargeCameraShake.h"

UFGGM_ChargeCameraShake::UFGGM_ChargeCameraShake()
{
	ShakeScale = 1.0f;
	
	// Generate Perlin noise for the camera shake
	float Rand = FMath::RandRange(0, 5);
	float NoiseX = FMath::PerlinNoise3D(FVector(Rand, 0.0f, 0.0f));
	float NoiseY = FMath::PerlinNoise3D(FVector(0.0f, Rand, 0.0f));
	float NoiseZ = FMath::PerlinNoise3D(FVector(0.0f, 0.0f, Rand));
	
	OscillationBlendInTime = 1.0f;
	OscillationBlendOutTime = 1.0f;
	/*RotOscillation.Pitch.Amplitude = NoiseX;
	RotOscillation.Yaw.Amplitude = NoiseY;
	RotOscillation.Roll.Amplitude = NoiseZ;*/
	LocOscillation.X.Amplitude = NoiseX;
	LocOscillation.Y.Amplitude = NoiseY;
	LocOscillation.Z.Amplitude = NoiseZ;
}


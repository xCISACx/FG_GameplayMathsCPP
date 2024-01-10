// Fill out your copyright notice in the Description page of Project Settings.


#include "FGGM_Projectile.h"

void AFGGM_Projectile::BeginPlay()
{
	Super::BeginPlay();
}

void AFGGM_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//UE_LOG(LogTemp, Warning, TEXT("DirectionVectorToOtherActor.Length(): %f"), DirectionVectorToOtherActor.Length());
	//UE_LOG(LogTemp, Warning, TEXT("DirectionVectorToOtherActor: %f, %f, %f"), DirectionVectorToOtherActor.X, DirectionVectorToOtherActor.Y, DirectionVectorToOtherActor.Z);

	if (DirectionVectorToOtherActor.Length() > 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("DirectionVectorToOtherActor.Length(): %f"), DirectionVectorToOtherActor.Length());
		Destroy();
	}
}

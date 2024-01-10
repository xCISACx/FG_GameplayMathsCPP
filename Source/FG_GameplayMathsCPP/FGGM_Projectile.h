// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FGGM_Actor.h"
#include "FGGM_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class FG_GAMEPLAYMATHSCPP_API AFGGM_Projectile : public AFGGM_Actor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};

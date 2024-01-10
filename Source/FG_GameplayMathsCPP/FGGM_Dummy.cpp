// Fill out your copyright notice in the Description page of Project Settings.


#include "FGGM_Dummy.h"

void AFGGM_Dummy::BeginPlay()
{
	Super::BeginPlay();
}

void AFGGM_Dummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DirectionVectorToOtherActor.Length() > 0)
	{
		bDirectionVectorIsValid = true;
	}
	else
	{
		bDirectionVectorIsValid = false;
	}

	//UE_LOG(LogTemp, Warning, TEXT("DirectionVectorToOtherActor.Length(): %f"), DirectionVectorToOtherActor.Length());

	if (bDirectionVectorIsValid)
	{
		if (DirectionVectorToOtherActor.Y == -1) // actor was hit from behind
		{
			FString ActorName = GetActorNameOrLabel();

			if (!bDynamicMaterialCreated)
			{
				UMeshComponent* CharacterMesh = StaticMeshComponent;
				if (!CharacterMesh) return;
		
				UMaterialInterface* BaseMaterial = CharacterMesh->GetMaterial(0);
			
				if (!BaseMaterial)
				{
					return;
				}
			
				DynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, NULL);
			
				if (!DynMaterial) return;

				CharacterMesh->SetMaterial(0, DynMaterial);
		
				bDynamicMaterialCreated = true;
			}

			// Go red
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString::Printf(TEXT("%s was HIT FROM BEHIND"), *ActorName));
			DynMaterial->SetVectorParameterValue("Base Color", FLinearColor::Red);
			UE_LOG(LogTemp, Warning, TEXT("setting colour to red"));
		}
	}
	else
	{
		if (DynMaterial)
		{
			// Lerp back to blue
			FLinearColor CurrentColor;
			FHashedMaterialParameterInfo ParameterInfo("Base Color");
			DynMaterial->GetVectorParameterValue(ParameterInfo, CurrentColor);

			float LerpAlpha = FMath::Clamp(DeltaTime / 1.0f, 0.0f, 1.0f);

			FLinearColor NewColour = FMath::Lerp(CurrentColor, FLinearColor::Blue, LerpAlpha);

			DynMaterial->SetVectorParameterValue("Base Color", NewColour);

			UE_LOG(LogTemp, Warning, TEXT("lerping to blue"));
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Direction vector is valid? %s"), bDirectionVectorIsValid ? TEXT("True") : TEXT("False"));
}

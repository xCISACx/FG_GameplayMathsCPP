// Fill out your copyright notice in the Description page of Project Settings.


#include "FGGM_Actor.h"

#include "DynamicMesh/MeshTransforms.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFGGM_Actor::AFGGM_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	RootComponent = StaticMeshComponent;

}

void AFGGM_Actor::DrawBounds()
{
	switch (ShapeType)
	{
		case Shape::Box:
			DrawDebugBox(GetWorld(), GetActorLocation(), AABBHalfExtents, FColor::Green, false, 0, 0, 3);
			break;

		//case Shape::Sphere:
			DrawDebugSphere(GetWorld(), GetActorLocation(), Radius + OutlineOffset, Segments, FColor::Green, false, 0, 0, 3);
			break;
	}
}

void AFGGM_Actor::DrawDetectionRadius()
{
	DrawDebugSphere(GetWorld(), GetActorLocation(), DetectionRadius, Segments, FColor::Cyan, false, 0, 0, 2);
}

// Called when the game starts or when spawned
void AFGGM_Actor::BeginPlay()
{
	Super::BeginPlay();

	//Is this cheating?
	FBoxSphereBounds MeshBounds = StaticMeshComponent->CalcBounds(GetTransform());
	
	//AABBHalfExtents = (GetActorScale() * 50) + AABBOffset;
	AABBHalfExtents = MeshBounds.BoxExtent;
	
	//AABBMin = GetActorLocation() - AABBHalfExtents - AABBOffset;
	AABBMin = MeshBounds.Origin - AABBHalfExtents - AABBOffset;
	
	//AABBMax = GetActorLocation() + AABBHalfExtents + AABBOffset;
	AABBMax = MeshBounds.Origin + AABBHalfExtents + AABBOffset;
	
	//Radius = GetActorScale().X * 150;

	switch (ShapeType)
	{
		case Shape::Box:
			DetectionRadius = AABBHalfExtents.X + 50;
			break;

		//case Shape::Sphere:
			DetectionRadius = Radius * 2;
			break;
	}
}

void AFGGM_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TArray<AActor*> FoundActors;
	TArray<AActor*> NearbyActors;

	// Get all actors of type AActor
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (Actor != this)
		{
			// Calculate the distance between the current actor and your reference actor (this)
			float Distance = FVector::Dist(Actor->GetActorLocation(), GetActorLocation());

			if (Distance <= DetectionRadius)
			{
				FString ActorName = GetActorNameOrLabel();
				FString OtherActorName = Actor->GetActorNameOrLabel();
				FString DebugMessage;
				//DebugMessage = FString::Printf(TEXT("%s: %s is inside my detection radius: %f <= %f"), *ActorName, *OtherActorName, Distance, DetectionRadius);
				float TimeToDisplay = 5.0f;
				FColor DebugTextColor = FColor::Yellow;
				GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);
		
				AFGGM_Actor* OtherActor = Cast<AFGGM_Actor>(Actor);
		
				if (OtherActor)
				{
					//GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, TEXT("other actor"));
					if (CheckCollisionWith(OtherActor))
					{
						HandleCollisionWith(OtherActor);
					}
				}
			}
		}
		
	}

	DrawBounds();
	DrawDetectionRadius();
}

bool AFGGM_Actor::CheckCollisionWith(const AFGGM_Actor* OtherActor) const
{
	FVector OtherActorAABBMin;
	FVector OtherActorAABBMax;
	
	switch (ShapeType)
	{
		case Shape::Box:
			OtherActorAABBMin = OtherActor->GetActorLocation() - OtherActor->AABBHalfExtents;
			OtherActorAABBMax = OtherActor->GetActorLocation() + OtherActor->AABBHalfExtents;
			break;

		//case Shape::Sphere:
			OtherActorAABBMin = OtherActor->GetActorLocation() - FVector(OtherActor->Radius);
			OtherActorAABBMax = OtherActor->GetActorLocation() + FVector(OtherActor->Radius);
			break;
	}

	// compare the vectors of both actors and detect intersections
	
	bool OverlapsX = AABBMin.X <= OtherActorAABBMax.X && AABBMax.X >= OtherActorAABBMin.X;
	bool OverlapsY = AABBMin.Y <= OtherActorAABBMax.Y && AABBMax.Y >= OtherActorAABBMin.Y;
	bool OverlapsZ = AABBMin.Z <= OtherActorAABBMax.Z && AABBMax.Z >= OtherActorAABBMin.Z;

	FString DebugMessage = FString::Printf(TEXT("OverlapsX: %s, OverlapsY: %s, OverlapsZ: %s"), 
	OverlapsX ? TEXT("True") : TEXT("False"), 
	OverlapsY ? TEXT("True") : TEXT("False"), 
	OverlapsZ ? TEXT("True") : TEXT("False"));
	
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Purple, DebugMessage);

	return OverlapsX || OverlapsY || OverlapsZ;
}

void AFGGM_Actor::HandleCollisionWith(AFGGM_Actor* OtherActor)
{
	FString ActorName = GetActorNameOrLabel();
	FString OtherActorName = OtherActor->GetActorNameOrLabel();

	FString DebugMessage;
	float TimeToDisplay = 3.0f;
	FColor DebugTextColor = FColor::Yellow;
	
	DebugMessage = FString::Printf(TEXT("%s: I'm colliding with %s"), *ActorName, *OtherActorName);
	//GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, DebugMessage);

	if (CheckCollisionWith(OtherActor))
	{
		CheckDirectionTo(OtherActor);
	}
	CheckDirectionTo(OtherActor);
}

void AFGGM_Actor::CheckDirectionTo(AFGGM_Actor* OtherActor)
{
	FString ActorName = GetActorNameOrLabel();
	FString OtherActorName = OtherActor->GetActorNameOrLabel();
	
	FVector ForwardVector = GetActorForwardVector();
	FVector DirectionToOtherActor = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	float DotProduct = FVector::DotProduct(ForwardVector, DirectionToOtherActor);
	FVector CrossProduct = FVector::CrossProduct(ForwardVector, DirectionToOtherActor);

	FString DebugMessage;
	float TimeToDisplay = 3.0f;
	FColor DebugTextColor = FColor::Yellow;
	FString DebugText = FString::Printf(TEXT("%s is "), *OtherActorName);

	if (DotProduct >= 0.707)
	{
		DebugText.Append(TEXT("IN FRONT"));

		if (CrossProduct.Z > 0)
		{
			DebugText.Append(TEXT(" and to the RIGHT of"));
		}
		else if (CrossProduct.Z < 0)
		{
			DebugText.Append(TEXT(" and to the LEFT of"));
		}
	}
	else if (DotProduct <= -0.707)
	{
		DebugText.Append(TEXT("BEHIND"));
		
		if (CrossProduct.Z > 0)
		{
			DebugText.Append(TEXT(" and to the RIGHT of"));
		}
		else if (CrossProduct.Z < 0)
		{
			DebugText.Append(TEXT(" and to the LEFT of"));
		}
	}
	else if (DotProduct == 0)
	{
		DebugText.Append(TEXT("INSIDE"));
	}

	DebugText.Append(FString::Printf(TEXT(" %s"), *ActorName));
	FString FormattedDebugMessage = FString::Printf(TEXT("%s"), *DebugText);
	FString FormattedDotProductMessage = FString::Printf(TEXT("%f"), DotProduct);

	if (!FormattedDebugMessage.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, FormattedDotProductMessage);
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, FormattedDebugMessage);
	}
}




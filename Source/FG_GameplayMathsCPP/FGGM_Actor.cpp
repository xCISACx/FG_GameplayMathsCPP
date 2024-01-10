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
			// Calculate the distance between the current actor and this actor
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
			else
			{
				DirectionVectorToOtherActor = FVector::ZeroVector;
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

		case Shape::Sphere:
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
	DirectionVectorToOtherActor = FVector::Zero();
	
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
	FString XText;
	FString YText;
	FString ZText;

	DirectionVectorToOtherActor = FVector::ZeroVector;

	// if the dot product is nearly zero, we aren't in front or behind. We are at the same Y value (Y/forward axis)
	if (FMath::IsNearlyZero(DotProduct))
{
    // if the cross product on the Y axis is different from 0, we are either above or below (Z/up axis)
    if (CrossProduct.Y != 0)
    {
        // not directly above or below
        if (CrossProduct.Y != 1 && CrossProduct.Y != -1)
        {
            if (CrossProduct.Y > 0)
            {
            	ZText = TEXT("BELOW");
                //DebugText.Append(TEXT("BELOW"));
                DirectionVectorToOtherActor.Z = -1;
            }
            else if (CrossProduct.Y < 0)
            {
            	ZText = TEXT("ABOVE");
                //DebugText.Append(TEXT("ABOVE"));
                DirectionVectorToOtherActor.Z = 1;
            }
        }
        // directly above or below
        else
        {
            if (CrossProduct.Y == 1)
            {
            	ZText = TEXT("BELOW");
                //DebugText.Append(TEXT("DIRECTLY BELOW"));
                DirectionVectorToOtherActor.Z = -1;
            }
            else if (CrossProduct.Y == -1)
            {
            	ZText = TEXT("ABOVE");
                //DebugText.Append(TEXT("DIRECTLY ABOVE"));
                DirectionVectorToOtherActor.Z = 1;
            }
        }
        if (CrossProduct.Z > 0)
        {
        	XText = TEXT("to the RIGHT of");
            //DebugText.Append(TEXT(" and to the RIGHT"));
            DirectionVectorToOtherActor.X = 1;
        }
        else if (CrossProduct.Z < 0)
        {
        	XText = TEXT("to the LEFT of");
            //DebugText.Append(TEXT(" and to the LEFT"));
            DirectionVectorToOtherActor.X = -1;
        }
    }
    
    // if the cross product on the Z axis is different from 0, we are either to the left or to the right (X/right axis)
    if (CrossProduct.Z != 0)
    {
        if (CrossProduct.Z == 1)
        {
        	XText = TEXT("DIRECTLY to the RIGHT of");
            //DebugText.Append(TEXT("DIRECTLY to the RIGHT"));
            DirectionVectorToOtherActor.X = 1;
        }
        else if (CrossProduct.Z == -1)
        {
        	XText = TEXT("DIRECTLY to the LEFT of");
            //DebugText.Append(TEXT("DIRECTLY to the LEFT"));
            DirectionVectorToOtherActor.X = -1;
        }
    }
}
	// if the dot product is NOT nearly zero, we are in front or behind
	else
	{
		// we are in front
		if (DotProduct > 0)
		{
			YText = TEXT("IN FRONT of");
			//DebugText.Append(TEXT("IN FRONT"));
			DirectionVectorToOtherActor.Y = 1;

			if (CrossProduct.Z > 0)
			{
				XText = TEXT("to the RIGHT of");
				//DebugText.Append(TEXT(" and to the RIGHT"));
				DirectionVectorToOtherActor.X = 1;
			}
			else if (CrossProduct.Z < 0)
			{
				XText = TEXT("to the LEFT of");
				//DebugText.Append(TEXT(" and to the LEFT"));
				DirectionVectorToOtherActor.X = -1;
			}

			if (CrossProduct.Y > 0)
			{
				ZText = TEXT("BELOW");
				//DebugText.Append(TEXT(" and BELOW"));
				DirectionVectorToOtherActor.Z = -1;
			}
			else if (CrossProduct.Y < 0)
			{
				ZText = TEXT("ABOVE");
				//DebugText.Append(TEXT(" and ABOVE"));
				DirectionVectorToOtherActor.Z = 1;
			}
		}
		// we are behind
		else if (DotProduct < 0)
		{
			YText = TEXT("BEHIND");
			//DebugText.Append(TEXT("BEHIND"));
			DirectionVectorToOtherActor.Y = -1;

			if (CrossProduct.Z > 0)
			{
				XText = TEXT("to the RIGHT of");
				//DebugText.Append(TEXT(" and to the RIGHT of"));
				DirectionVectorToOtherActor.X = 1;
			}
			else if (CrossProduct.Z < 0)
			{
				XText = TEXT("to the LEFT of");
				//DebugText.Append(TEXT(" and to the LEFT of"));
				DirectionVectorToOtherActor.X = -1;
			}

			if (CrossProduct.Y > 0)
			{
				ZText = TEXT("BELOW");
				//DebugText.Append(TEXT(" and BELOW"));
				DirectionVectorToOtherActor.Z = -1;
			}
			else if (CrossProduct.Y < 0)
			{
				ZText = TEXT("ABOVE");
				//DebugText.Append(TEXT(" and ABOVE"));
				DirectionVectorToOtherActor.Z = 1;
			}
		}
	}
	
	DebugText.Append(XText);

	if (!XText.IsEmpty())
	{
		if (!YText.IsEmpty())
		{
			DebugText.Append(!ZText.IsEmpty() ? TEXT(", ") : TEXT(" and "));
		}
		else if (!ZText.IsEmpty())
		{
			DebugText.Append(TEXT(" and "));
		}
	}

	DebugText.Append(YText);

	if (!YText.IsEmpty() && !ZText.IsEmpty())
	{
		DebugText.Append(TEXT(" and "));
	}

	DebugText.Append(ZText);
	
	DebugText.Append(FString::Printf(TEXT(" %s"), *ActorName));
	FString FormattedDebugMessage = FString::Printf(TEXT("%s"), *DebugText);
	FString FormattedDotProductMessage = FString::Printf(TEXT("Dot: %f"), DotProduct);
	FString FormattedCrossProductMessage = FString::Printf(TEXT("Cross: %f, %f, %f"), CrossProduct.X, CrossProduct.Y, CrossProduct.Z);
	FString FormattedCrossProductLengthMessage = FString::Printf(TEXT("Cross Length: %f"), CrossProduct.Length());
	FString FormattedDirectionVectorMessage = FString::Printf(TEXT("Direction Vector: X=%f, Y=%f, Z=%f"), DirectionVectorToOtherActor.X, DirectionVectorToOtherActor.Y, DirectionVectorToOtherActor.Z);

	if (!FormattedDebugMessage.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, FormattedDebugMessage);
	}

	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, FormattedDotProductMessage);
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, FormattedCrossProductMessage);
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, FormattedCrossProductLengthMessage);
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DebugTextColor, FormattedDirectionVectorMessage);

}




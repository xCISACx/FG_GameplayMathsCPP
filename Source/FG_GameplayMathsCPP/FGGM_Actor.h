// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "FGGM_Actor.generated.h"

UENUM(BlueprintType)
enum class Shape : uint8
{
	Box,
	Sphere
};

UCLASS()
class FG_GAMEPLAYMATHSCPP_API AFGGM_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFGGM_Actor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds")
	Shape ShapeType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bounds", meta = (AllowPrivateAccess = "true"))
	UMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds", meta = (AllowPrivateAccess = "true"))
	float AABBOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds", meta = (AllowPrivateAccess = "true"))
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds", meta = (AllowPrivateAccess = "true"))
	float OutlineOffset = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds", meta = (AllowPrivateAccess = "true"))
	int Segments = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds")
	float DetectionRadius = 10;

	void DrawBounds();
	void DrawDetectionRadius();

	void CheckDirectionTo(AFGGM_Actor* OtherActor);

	bool CheckCollisionWith(const AFGGM_Actor* OtherActor) const;

	UPROPERTY(EditAnywhere)
	FVector AABBHalfExtents;

	void HandleCollisionWith(AFGGM_Actor* OtherActor);

	void GetOtherActors();

	FVector LastKnownPosition;

	FVector NewPosition;

	FVector AABBMin;
	FVector AABBMax;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};

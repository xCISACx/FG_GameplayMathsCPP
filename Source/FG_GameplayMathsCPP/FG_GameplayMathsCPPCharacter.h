// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FG_GameplayMathsCPPCharacter.generated.h"


UCLASS(config=Game)
class AFG_GameplayMathsCPPCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Charge Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChargeAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	FName ParameterName = "Tint";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	FLinearColor NeutralTintColour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	FLinearColor ChargedTintColour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UFGGM_ChargeCameraShake> CameraShake;

public:
	AFG_GameplayMathsCPPCharacter();

	void StartCharging();
	void StopCharging();

private:
	bool bDynamicMaterialCreated = false;
	
	void UpdatePower(float DeltaSeconds);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	bool bIsCharging = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float CurrentPower = 0.0f;
	float MinPower = 0.0f;
	float TargetPower = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float MaxPower = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float ChargeRate = 0.2f;
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	void StartCameraShake();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


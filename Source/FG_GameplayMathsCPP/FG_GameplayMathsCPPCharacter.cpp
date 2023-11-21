// Copyright Epic Games, Inc. All Rights Reserved.

#include "FG_GameplayMathsCPPCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FGGM_ChargeCameraShake.h"
#include "Kismet/GameplayStatics.h"


//////////////////////////////////////////////////////////////////////////
// AFG_GameplayMathsCPPCharacter

AFG_GameplayMathsCPPCharacter::AFG_GameplayMathsCPPCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AFG_GameplayMathsCPPCharacter::StartCharging()
{
	//UE_LOG(LogTemp, Warning, TEXT("STARTED CHARGING"));
	bIsCharging = true;
	TargetPower = MaxPower;
	StartCameraShake();
}

void AFG_GameplayMathsCPPCharacter::StopCharging()
{
	//UE_LOG(LogTemp, Warning, TEXT("STOPPED CHARGING"));
	bIsCharging = false;
	TargetPower = 0;
}

UMaterialInstanceDynamic* DynMaterial = nullptr;
//UMaterialInstanceDynamic* DynMaterial2 = nullptr;

void AFG_GameplayMathsCPPCharacter::UpdatePower(float DeltaSeconds)
{
	if (!bDynamicMaterialCreated)
	{
		USkeletalMeshComponent* CharacterMesh = GetMesh();
		if (!CharacterMesh) return;
		
		UMaterialInterface* BaseMaterial = CharacterMesh->GetMaterial(0);
		UMaterialInterface* BaseMaterial2 = CharacterMesh->GetMaterial(1);
		if (!BaseMaterial || !BaseMaterial2)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Materials: %s, %s"), *BaseMaterial->GetName(), *BaseMaterial2->GetName());
			return;
		}

		//UE_LOG(LogTemp, Warning, TEXT("3"));
		DynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, NULL);

		//uncommenting this dynamic material instance makes the charging input stop working, unsure why...
		//DynMaterial2 = UMaterialInstanceDynamic::Create(BaseMaterial2, NULL);
		if (!DynMaterial) return;

		CharacterMesh->SetMaterial(0, DynMaterial);
		//CharacterMesh->SetMaterial(1, DynMaterial2);
		
		bDynamicMaterialCreated = true;
	}

	if (bIsCharging)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CHARGING"));
		CurrentPower = FMath::Lerp(CurrentPower, TargetPower, DeltaSeconds / ChargeRate);
	}
	else
	{
		CurrentPower = FMath::Lerp(CurrentPower, MinPower, DeltaSeconds / ChargeRate);
	}

	FLinearColor NewColour = FMath::Lerp(NeutralTintColour, ChargedTintColour, CurrentPower);

	DynMaterial->SetVectorParameterValue(ParameterName, NewColour);
	
	FLinearColor CurrentTint;
	DynMaterial->GetVectorParameterValue(ParameterName, CurrentTint);
	
	//UE_LOG(LogTemp, Warning, TEXT("Current Tint: %s"), *CurrentTint.ToString());
}

void AFG_GameplayMathsCPPCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AFG_GameplayMathsCPPCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdatePower(DeltaSeconds);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFG_GameplayMathsCPPCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFG_GameplayMathsCPPCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFG_GameplayMathsCPPCharacter::Look);

		//Charging
		EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Triggered, this, &AFG_GameplayMathsCPPCharacter::StartCharging);
		EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Canceled, this, &AFG_GameplayMathsCPPCharacter::StopCharging);
		EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Completed, this, &AFG_GameplayMathsCPPCharacter::StopCharging);

	}

}

void AFG_GameplayMathsCPPCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AFG_GameplayMathsCPPCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFG_GameplayMathsCPPCharacter::StartCameraShake()
{
	if (Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("SHAKE"));
		// Start the camera shake
		UGameplayStatics::GetPlayerCameraManager(this, 0)->StartCameraShake(UFGGM_ChargeCameraShake::StaticClass(), 1.0f);
	}
}





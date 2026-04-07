// Copyright Epic Games, Inc. All Rights Reserved.


#include "ZSPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Character/ZSPlayerCharacter.h"
#include "PlayerState/ZSPlayerState.h"

AZSPlayerController::AZSPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	// PlayerCameraManagerClass = ASKCameraManager::StaticClass();
}

void AZSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		check(DefaultMappingContext);
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void AZSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AZSPlayerController::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AZSPlayerController::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AZSPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
		                                   &AZSPlayerController::StopJumping);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this,
		                                   &AZSPlayerController::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this,
		                                   &AZSPlayerController::StopSprint);
		EnhancedInputComponent->BindAction(FireProjectileAction, ETriggerEvent::Completed, this,
		                                   &AZSPlayerController::StartFireProjectile);
	}
}

void AZSPlayerController::Move(const FInputActionValue& Value)
{
	// UE_LOG(LogZS, Error, TEXT("Move not implemented"));
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D InMoveVector = Value.Get<FVector2D>();
		const FRotator ControlrRotation = GetControlRotation();
		const FRotator ControlYawRotation(0.f, ControlrRotation.Yaw, 0.f);

		const FVector InLookVector = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
		const FVector InRightVector = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);


		ControlledPawn->AddMovementInput(InLookVector, InMoveVector.X);
		ControlledPawn->AddMovementInput(InRightVector, InMoveVector.Y);
	}
}

void AZSPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D InLookVector = Value.Get<FVector2D>();

	AddYawInput(InLookVector.X);
	AddPitchInput(InLookVector.Y);
}

void AZSPlayerController::Jump(const FInputActionValue& Value)
{
	// UE_LOG(LogZS, Error, TEXT("Jump not implemented"));
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	AZSPlayerCharacter* PlayerCharacter = Cast<AZSPlayerCharacter>(ControlledPawn);
	if (!IsValid(PlayerCharacter))
		return;

	if (ACharacter* pCharacter = Cast<ACharacter>(GetPawn()))
	{
		pCharacter->Jump();
	}
}

void AZSPlayerController::StopJumping()
{
	if (ACharacter* pCharacter = Cast<ACharacter>(GetPawn()))
	{
		pCharacter->StopJumping();
	}
}

void AZSPlayerController::StartSprint(const FInputActionValue& Value)
{
	// UE_LOG(LogZS, Error, TEXT("Start sprint not implemented"));
	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();
	if (!PS)
	{
		return;
	}

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
		return;

	FGameplayTag SprintTag = FGameplayTag::RequestGameplayTag(FName("Ability.Sprint"));
	FGameplayTagContainer SprintTagContainer;
	SprintTagContainer.AddTag(SprintTag);

	ASC->TryActivateAbilitiesByTag(SprintTagContainer);

	// 	if (!bSprintFlag)
	// 	{
	// 		bSprintFlag = true;
	// 		PlayerCharacter->SetLooseTag(TAG_State_Movement_Sprint, true);
	// 		PlayerCharacter->SetLooseTag(TAG_State_Movement_Idle, false);
	// 	}
}

void AZSPlayerController::StopSprint(const FInputActionValue& Value)
{
	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();
	if (!PS)
	{
		return;
	}

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
		return;

	FGameplayTagContainer SprintTagContainer;
	SprintTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Sprint")));

	ASC->CancelAbilities(&SprintTagContainer);

	// if (bSprintFlag)
	// {
	// 	bSprintFlag = false;
	// 	PlayerCharacter->SetLooseTag(TAG_State_Movement_Sprint, false);
	// 	PlayerCharacter->SetLooseTag(TAG_State_Movement_Idle, true);
	// }
}

void AZSPlayerController::StartFireProjectile(const FInputActionValue& Value)
{
	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();
	if (!PS)
	{
		return;
	}

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
		return;


	FGameplayTagContainer GATagContainer;
	GATagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("ATK.Left")));

	ASC->TryActivateAbilitiesByTag(GATagContainer);
}

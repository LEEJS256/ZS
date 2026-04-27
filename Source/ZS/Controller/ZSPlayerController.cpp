// Copyright Epic Games, Inc. All Rights Reserved.


#include "ZSPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Character/ZSPlayerCharacter.h"
#include "PlayerState/ZSPlayerState.h"
#include "UI/ZS_TotalInformationWidget.h"
#include "Utility/ZSNativeGameplayTag.h"

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

	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();

	if (PS)
	{
		PS->BindToAttributes(this);
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
		EnhancedInputComponent->BindAction(Fire_Right_Action, ETriggerEvent::Completed, this,
		                                   &AZSPlayerController::ATK_Right);
		EnhancedInputComponent->BindAction(Fire_Ultimate_Action, ETriggerEvent::Completed, this,
		                                   &AZSPlayerController::ATK_Ultimate);
		EnhancedInputComponent->BindAction(BuildTower_Action, ETriggerEvent::Completed, this,
		                                   &AZSPlayerController::BuildTower);
		EnhancedInputComponent->BindAction(Action_Inventory, ETriggerEvent::Completed, this,
		                                   &AZSPlayerController::Open_Inventory);
	}
}

bool AZSPlayerController::GetMouseHitLocation(FVector& OutLocation)
{
	float MouseX, MouseY;

	if (!GetMousePosition(MouseX, MouseY))
		return false;

	FVector WorldLocation;
	FVector WorldDirection;

	// 화면 → 월드 변환
	DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

	FVector Start = WorldLocation;
	FVector End = Start + (WorldDirection * 10000.f);

	FHitResult Hit;

	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = false;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility, // 또는 Ground 채널
		Params
	);

	if (bHit)
	{
		OutLocation = Hit.Location;
		DrawDebugSphere(GetWorld(), Hit.Location, 20.f, 12, FColor::Green);
		return true;
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	}

	return false;
}

bool AZSPlayerController::GetCenterHitLocation(FVector& OutLocation)
{
	int32 SizeX, SizeY;
	GetViewportSize(SizeX, SizeY);

	float CenterX = SizeX * 0.5f;
	float CenterY = SizeY * 0.5f;

	FVector WorldLoc, WorldDir;
	DeprojectScreenPositionToWorld(CenterX, CenterY, WorldLoc, WorldDir);

	FVector Start = WorldLoc;
	FVector End = Start + (WorldDir * 10000.f);

	FHitResult Hit;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility
	);

	if (bHit)
	{
		OutLocation = Hit.Location;
		return true;
	}

	return false;
}

void AZSPlayerController::OnAnyAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (TotalInformation_Widget)
	{
		TotalInformation_Widget->Update_TotalWidget();
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

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		PS,
		TAG_Input_LeftClick,
		FGameplayEventData()
	);

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
		return;

	FGameplayTagContainer GATagContainer;
	GATagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("ATK.Left")));

	ASC->TryActivateAbilitiesByTag(GATagContainer);
}

void AZSPlayerController::ATK_Right(const FInputActionValue& Value)
{
	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();
	if (!PS)
		return;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		PS,
		TAG_Input_RightClick,
		FGameplayEventData()
	);

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
		return;

	FGameplayTagContainer GATagContainer;
	GATagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("ATK.Right")));

	ASC->TryActivateAbilitiesByTag(GATagContainer);
}

void AZSPlayerController::ATK_Ultimate(const FInputActionValue& Value)
{
	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();
	if (!PS)
		return;


	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
		return;

	FGameplayTagContainer GATagContainer;
	GATagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("ATK.Ultimate")));

	ASC->TryActivateAbilitiesByTag(GATagContainer);
}

void AZSPlayerController::BuildTower(const FInputActionValue& Value)
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
	GATagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.BuildTower")));

	ASC->TryActivateAbilitiesByTag(GATagContainer);
}

void AZSPlayerController::Open_Inventory(const FInputActionValue& Value)
{
	//없으면 만들기
	if (!TotalInformation_Widget && TotalInformation_WidgetClass)
	{
		TotalInformation_Widget = CreateWidget<UZS_TotalInformationWidget>(
			this,
			TotalInformation_WidgetClass
		);
	}

	if (!TotalInformation_Widget)
		return;

	if (TotalInformation_Widget->IsInViewport())
	{
		TotalInformation_Widget->RemoveFromParent();

		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;

		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);
	}
	else
	{
		TotalInformation_Widget->AddToViewport();

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		SetInputMode(InputMode);
		bShowMouseCursor = true;

		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);

		TotalInformation_Widget->Update_TotalWidget();
	}
}

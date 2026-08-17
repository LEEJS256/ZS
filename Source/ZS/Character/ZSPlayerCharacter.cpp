// Fill out your copyright notice in the Description page of Project Settings.


#include "ZSPlayerCharacter.h"

#include "AbilitySystemInterface.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Component/ZSPreviewComponent.h"
#include "Component/ZS_InventoryComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "PlayerState/ZSPlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "UI/ZS_Crosshair.h"
#include "UI/ZS_MinimapUI.h"
#include "UI/ZS_playerHudWidget.h"
#include "Component/ZS_MinimapIcon_Component.h"
#include "Utility/ZSNativeGameplayTag.h"

// Sets default values
AZSPlayerCharacter::AZSPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	//카메라대로 캐릭터회전
	bUseControllerRotationYaw = true;
	//bUseControllerRotationYaw = false;

	bUseControllerRotationRoll = false;

	// 메시 Transform 설정
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->bEnableUpdateRateOptimizations = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	SpringArm->bDoCollisionTest = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	bReplicates = true;

	PreviewComponent = CreateDefaultSubobject<UZSPreviewComponent>(TEXT("PriviewComponent"));
	Inventory = CreateDefaultSubobject<UZS_InventoryComponent>(TEXT("Inventory"));

	//미니맵 부분
	MinimapCaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCaptureComp"));
	MinimapCaptureComp->SetupAttachment(RootComponent);

	// 플레이어 머리 위 2000 units(20m) 높이에서 아래(-90도)를 바라보도록 설정
	MinimapCaptureComp->SetRelativeLocation(FVector(0.f, 0.f, 4000.f));
	MinimapCaptureComp->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));

	MinimapCaptureComp->ProjectionType = ECameraProjectionMode::Orthographic;
	MinimapCaptureComp->OrthoWidth = 5000.f;
	MinimapCaptureComp->CaptureSource = SCS_BaseColor;

	MinimapCaptureComp->ShowFlags.SetWidgetComponents(false);
	MinimapCaptureComp->ShowFlags.SetParticles(false);
	MinimapCaptureComp->ShowFlags.SetNiagara(false);

	MinimapCaptureComp->bCaptureEveryFrame = false;
	MinimapCaptureComp->bCaptureOnMovement = false;

	// 플레이어 본인 미니맵 마커 컴포넌트
	MinimapIconComp = CreateDefaultSubobject<UZS_MinimapIcon_Component>(TEXT("MinimapIconComp"));
	MinimapIconComp->bFollowActorRotation = true; // 플레이어 시선 방향 회전 반영
	
}

UAbilitySystemComponent* AZSPlayerCharacter::GetAbilitySystemComponent() const
{
	// AZSPlayerCharacter* PS = GetPlayerState<AZSPlayerCharacter>();
	// return PS ? PS->GetAbilitySystemComponent() : nullptr;
	return AbilitySystemComponent;
}

void AZSPlayerCharacter::SetSprinting(bool bSprinting)
{
	bIsSprinting = bSprinting;
	float WalkSpeed = AttributeSet->GetSpeed();
	// UE_LOG(LogTemp, Warning, TEXT("SprintWeight: %f"), AttributeSet->GetSprintWeight());
	float SprintSpeed = AttributeSet->GetSprintWeight() * WalkSpeed;
	// UE_LOG(LogTemp, Warning, TEXT("SprintSpeed: %f"), SprintSpeed);
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void AZSPlayerCharacter::UpdateMinimapCapture()
{
	if (MinimapCaptureComp)
	{
		MinimapCaptureComp->CaptureScene();
	}


}

// Called when the game starts or when spawned
void AZSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	BaseSetting();
	BaseUI();

	Inventory->AddItem(Test_ItemData);

	Beginplay_Minimap();
}

void AZSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AttributeSet = PS->GetAttributeSet();

	InitASCFromPlayerState();
}

// Called every frame
void AZSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCrosshair();
}

// Called to bind functionality to input
void AZSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AZSPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AZSPlayerCharacter::ApplyAttributesToMovement()
{
	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();
	if (!PS)
	{
		return;
	}

	UZSAttributeSet* ZSAttributeSet = PS->GetAttributeSet();
	if (!ZSAttributeSet)
	{
		return;
	}

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp)
	{
		return;
	}

	MoveComp->MaxWalkSpeed = ZSAttributeSet->GetSpeed();
}

void AZSPlayerCharacter::BaseSetting()
{
	// GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f; // 캐릭터 점프 높이 수정 필요
	GetCharacterMovement()->AirControl = 0.35f;
	// GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetSpeed();
	// GetCharacterMovement()->MaxWalkSpeed = 700.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

void AZSPlayerCharacter::InitASCFromPlayerState()
{
	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();
	if (!PS)
		return;
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AttributeSet = PS->GetAttributeSet();

	if (!AbilitySystemComponent)
		return;

	AbilitySystemComponent->InitAbilityActorInfo(PS, this);

	// 초기 속도 적용
	OnSpeedAttributeChanged(FOnAttributeChangeData());

	PS->InitializePlayerDA();
	ApplyAttributesToMovement();
}

void AZSPlayerCharacter::OnSpeedAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (!IsValid(AttributeSet))
		return;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp == nullptr)
		return;

	// AttributeSet에서 Speed 값 읽기
	const float NewSpeed = AttributeSet->GetSpeed();

	// 이동 속도 적용
	MoveComp->MaxWalkSpeed = NewSpeed;
}

void AZSPlayerCharacter::UpdateCrosshair()
{
	bool bNowTargeting = IsTargetingEnemy();

	if (bNowTargeting != bPrevTargeting)
	{
		CrosshairWidget->UpdateCrosshairColor(bNowTargeting);
		bPrevTargeting = bNowTargeting;
	}
}
void AZSPlayerCharacter::Beginplay_Minimap()
{
	if (!MinimapCaptureComp || !MinimapRenderTarget) return;

	MinimapCaptureComp->TextureTarget = MinimapRenderTarget;
	MinimapCaptureComp->ProjectionType = ECameraProjectionMode::Orthographic;
	MinimapCaptureComp->OrthoWidth = 5000.f;

	// ★ DrawDebugLine, DrawDebugCircle 등 모든 디버그 선 완벽 차단
	MinimapCaptureComp->ShowFlags.SetCompositeDebugPrimitives(false);

	// 캐릭터/몬스터(스켈레탈 메시) 제외
	MinimapCaptureComp->ShowFlags.SetSkeletalMeshes(false);

	// 파티클/나이아가라/위젯/데칼/스플라인 제외
	MinimapCaptureComp->ShowFlags.SetParticles(false);
	MinimapCaptureComp->ShowFlags.SetNiagara(false);
	MinimapCaptureComp->ShowFlags.SetWidgetComponents(false);
	MinimapCaptureComp->ShowFlags.SetDecals(false);
	MinimapCaptureComp->ShowFlags.SetSplines(false);

	// 그림자/안개 제거
	MinimapCaptureComp->ShowFlags.SetDynamicShadows(false);
	MinimapCaptureComp->ShowFlags.SetFog(false);
	MinimapCaptureComp->ShowFlags.SetAtmosphere(false);

	// 지형과 스태틱 메시는 유지
	MinimapCaptureComp->ShowFlags.SetLandscape(true);
	MinimapCaptureComp->ShowFlags.SetStaticMeshes(true);

	// 플레이어 본인 액터 숨김
	MinimapCaptureComp->HiddenActors.AddUnique(this);

	// 주기적 캡처 타이머
	GetWorldTimerManager().SetTimer(
		MinimapTimerHandle,
		this,
		&AZSPlayerCharacter::UpdateMinimapCapture,
		0.05f,
		true
	);

	MinimapCaptureComp->CaptureScene();
}
bool AZSPlayerCharacter::IsTargetingEnemy()
{
	bool ReturnResult = false;

	FVector Start;
	FRotator Rot;
	Controller->GetPlayerViewPoint(Start, Rot);

	FVector End = Start + Rot.Vector() * 10000.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	ReturnResult = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Pawn,
		Params);

	AActor* HitActor = Hit.GetActor();

	if (ReturnResult && HitActor)
	{
		IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(HitActor);
		if (ASCInterface)
		{
			UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
			if (ASC && ASC->HasMatchingGameplayTag(TAG_Team_Monster))
			{
				// 몬스터 맞음
				return true;
			}
		}
	}
	//
	// if (ReturnResult && Hit.GetActor()->ActorHasTag("Monster"))
	// {
	// }


	return false;
}

void AZSPlayerCharacter::BaseUI()
{
	if (HUDWidgetClass)
	{
		HUDWidget = Cast<UZS_playerHudWidget>(CreateWidget(GetWorld(), HUDWidgetClass));
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			HUDWidget->Init(this);
		}
	}

	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UZS_Crosshair>(GetWorld(), CrosshairWidgetClass);
		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
		}
	}

	if (MinimapWidgetClass)
	{
		MinimapWidget = CreateWidget<UZS_MinimapUI>(GetWorld(), MinimapWidgetClass);
		if (MinimapWidget)
		{
			MinimapWidget->SetTargetCaptureComponent(MinimapCaptureComp);
			MinimapWidget->AddToViewport();
		}
	}
}

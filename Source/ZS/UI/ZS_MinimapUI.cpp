// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_MinimapUI.h"

#include "ZS_MinimapIcon.h"
#include "Character/ZSPlayerCharacter.h"
#include "Component/ZS_MinimapSubsystem.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SceneCaptureComponent2D.h"


void UZS_MinimapUI::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* pWorld = GetWorld();
	if (!IsValid(pWorld))
	{
		return;
	}

	if (UZS_MinimapSubsystem* Subsystem = pWorld->GetSubsystem<UZS_MinimapSubsystem>())
	{
		// 서브시스템 이벤트 바인딩
		Subsystem->OnIconRegistered.AddDynamic(this, &UZS_MinimapUI::HandleIconRegistered);
		Subsystem->OnIconUnregistered.AddDynamic(this, &UZS_MinimapUI::HandleIconUnregistered);

		// 이미 월드에 스폰되어 있던 마커들 일괄 생성
		for (const TWeakObjectPtr<UZS_MinimapIcon_Component>& CompPtr : Subsystem->GetActiveIconComponents())
		{
			if (UZS_MinimapIcon_Component* Comp = CompPtr.Get())
			{
				HandleIconRegistered(Comp);
			}
		}
	}
}

void UZS_MinimapUI::NativeDestruct()
{
	UWorld* pWorld = GetWorld();
	if (!IsValid(pWorld))
	{
		return;
	}
		if (UZS_MinimapSubsystem* Subsystem = pWorld->GetSubsystem<UZS_MinimapSubsystem>())
		{
			Subsystem->OnIconRegistered.RemoveDynamic(this, &UZS_MinimapUI::HandleIconRegistered);
			Subsystem->OnIconUnregistered.RemoveDynamic(this, &UZS_MinimapUI::HandleIconUnregistered);
		}
	

	ActiveIconMap.Empty();

	Super::NativeDestruct();
}

void UZS_MinimapUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	USceneCaptureComponent2D* CaptureComp = TargetCaptureComp.Get();
	
	// // 1. 상태 화면 출력 (좌측 상단 고정 디버그 텍스트)
	// if (GEngine)
	// {
	// 	FString StatusMsg = FString::Printf(
	// 		TEXT("[Minimap Debug]\n- CaptureComp Valid: %s\n- Active Icons Count: %d"),
	// 		CaptureComp ? TEXT("TRUE") : TEXT("FALSE (NULL!)"),
	// 		ActiveIconMap.Num()
	// 	);
	// 	GEngine->AddOnScreenDebugMessage(100, 0.0f, CaptureComp ? FColor::Green : FColor::Red, StatusMsg);
	// }

	if (!CaptureComp || !IconCanvas) return;

	TArray<TWeakObjectPtr<UZS_MinimapIcon_Component>> ToRemoveKeys;

	int32 Index = 0;
	for (auto& Pair : ActiveIconMap)
	{
		UZS_MinimapIcon_Component* Comp = Pair.Key.Get();
		UZS_MinimapIcon* IconWidget = Pair.Value;

		if (!Comp || !Comp->GetOwner() || !IconWidget)
		{
			if (IconWidget) IconWidget->RemoveFromParent();
			ToRemoveKeys.Add(Pair.Key);
			continue;
		}

		FVector ActorLocation = Comp->GetOwner()->GetActorLocation();
		FVector2D MinimapPos;
		bool bIsInside = WorldToMinimap(ActorLocation, CaptureComp, Comp->bClampToBorder, MinimapPos);

		// // 2. 개별 아이콘 좌표 디버그 출력
		// if (GEngine)
		// {
		// 	FString IconDebug = FString::Printf(
		// 		TEXT("Icon[%d] (%s) -> UI Pos: (%.1f, %.1f) | Inside: %s | Visible: %s"),
		// 		Index++,
		// 		*Comp->GetOwner()->GetName(),
		// 		MinimapPos.X, MinimapPos.Y,
		// 		bIsInside ? TEXT("YES") : TEXT("NO"),
		// 		Comp->bIsVisibleOnMinimap ? TEXT("YES") : TEXT("NO")
		// 	);
		// 	GEngine->AddOnScreenDebugMessage(101 + Index, 0.0f, FColor::Cyan, IconDebug);
		// }

		if (!Comp->bIsVisibleOnMinimap || (!bIsInside && !Comp->bClampToBorder))
		{
			IconWidget->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}

		IconWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(IconWidget->Slot))
		{
			CanvasSlot->SetPosition(MinimapPos);
		}

		if (Comp->bFollowActorRotation)
		{
			float RelativeYaw = Comp->GetOwner()->GetActorRotation().Yaw - CaptureComp->GetComponentRotation().Yaw;
			IconWidget->SetIconRotation(RelativeYaw);
		}
	}

	for (auto& DeadKey : ToRemoveKeys)
	{
		ActiveIconMap.Remove(DeadKey);
	}
}
void UZS_MinimapUI::HandleIconRegistered(UZS_MinimapIcon_Component* IconComp)
{
	FString OwnerName = IconComp->GetOwner() ? IconComp->GetOwner()->GetName() : TEXT("None");
	UE_LOG(LogTemp, Warning, TEXT("[MinimapUI] Icon Registered from Actor: %s"), *OwnerName);

	if (ActiveIconMap.Contains(IconComp)) return;

	if (UZS_MinimapIcon* NewIconWidget = CreateIconWidgetForComponent(IconComp))
	{
		ActiveIconMap.Add(IconComp, NewIconWidget);
		UE_LOG(LogTemp, Warning, TEXT("[MinimapUI] -> Widget Created Successfully for: %s"), *OwnerName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[MinimapUI] -> Failed to Create Widget! (Check DefaultIconClass or IconCanvas)"));
	}
}

void UZS_MinimapUI::HandleIconUnregistered(UZS_MinimapIcon_Component* IconComp)
{
	if (!IconComp) return;

	if (TObjectPtr<UZS_MinimapIcon>* FoundWidgetPtr = ActiveIconMap.Find(IconComp))
	{
		if (*FoundWidgetPtr)
		{
			(*FoundWidgetPtr)->RemoveFromParent();
		}
		ActiveIconMap.Remove(IconComp);
	}
}

UZS_MinimapIcon* UZS_MinimapUI::CreateIconWidgetForComponent(UZS_MinimapIcon_Component* IconComp)
{
	if (!IconCanvas || !DefaultIconClass || !IconComp) return nullptr;

	UZS_MinimapIcon* IconWidget = CreateWidget<UZS_MinimapIcon>(this, DefaultIconClass);
	if (!IconWidget) return nullptr;

	if (IconComp->IconTexture)
	{
		IconWidget->SetIconTexture(IconComp->IconTexture);
	}

	if (UCanvasPanelSlot* CanvasSlot = IconCanvas->AddChildToCanvas(IconWidget))
	{
		CanvasSlot->SetSize(IconComp->IconSize);
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f)); // 아이콘 중심점 기준 정렬
		// 기본 좌상단 앵커(0, 0) 유지 (WorldToMinimap에서 중심 좌표를 직접 더함)
		CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
	}

	return IconWidget;
}

void UZS_MinimapUI::SetTargetCaptureComponent(USceneCaptureComponent2D* InCaptureComp)
{
	TargetCaptureComp  =InCaptureComp;
}


bool UZS_MinimapUI::WorldToMinimap(
	const FVector& WorldLocation,
	USceneCaptureComponent2D* CaptureComp,
	bool bClampToBorder,
	FVector2D& OutMinimapPos) const
{
	if (!CaptureComp) return false;

	const float HalfWidth = CaptureComp->OrthoWidth * 0.5f;
	if (HalfWidth <= 0.0f) return false;

	// 1. 카메라(플레이어)와의 월드 수평 오프셋 (Z 무시)
	FVector CamLocation = CaptureComp->GetComponentLocation();
	FVector Delta = WorldLocation - CamLocation;
	FVector2D WorldDelta2D(Delta.X, Delta.Y);

	// 2. 카메라의 수평 회전(Yaw)만 반영
	float CamYaw = CaptureComp->GetComponentRotation().Yaw;
	FVector2D LocalDelta2D = WorldDelta2D.GetRotated(-CamYaw);

	// 3. UI 정규화 (-1.0 ~ 1.0)
	float U = LocalDelta2D.Y / HalfWidth;
	float V = -LocalDelta2D.X / HalfWidth;

	FVector2D NormalizedPos(U, V);
	float DistanceFromCenter = NormalizedPos.Size();

	bool bIsInside = DistanceFromCenter <= 1.0f;

	// 4. 테두리 클램핑
	if (bClampToBorder && DistanceFromCenter > 1.0f)
	{
		NormalizedPos = NormalizedPos.GetSafeNormal();
	}

	// 5. 미니맵 중심점(Radius, Radius)을 기준으로 픽셀 좌표 산출
	FVector2D MinimapCenter(MinimapWidgetRadius, MinimapWidgetRadius);
	OutMinimapPos = MinimapCenter + (NormalizedPos * MinimapWidgetRadius);

	return bIsInside;
}
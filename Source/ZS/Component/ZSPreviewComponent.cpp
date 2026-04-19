// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ZSPreviewComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS/GA/GA_Right_ATK.h"
#include "Kismet/GameplayStatics.h"
#include "Object/ZS_bomb.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStaticsTypes.h"

// Sets default values for this component's properties
UZSPreviewComponent::UZSPreviewComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UZSPreviewComponent::StartPreview(const FTrajectoryParams& InParams)
{
	Params = InParams;
	bIsPreviewing = true;
}

void UZSPreviewComponent::StartPreview_RightGA(UGA_Right_ATK* InGA)
{
	RightGA = InGA;
	bIsPreviewing = true;
}

void UZSPreviewComponent::StopPreview()
{
	bIsPreviewing = false;
}


// Called when the game starts
void UZSPreviewComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UZSPreviewComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsPreviewing || !RightGA)
		return;

	FVector Start = RightGA->GetSpawnLocation();
	FVector Target = RightGA->GetTargetLocation();

	

	// 실제 투사체와 동일 계산
	FVector LaunchVelocity = RightGA->CalculateLaunchVelocity(Start, Target);

	if (LaunchVelocity.IsNearlyZero())
		return;

	// 🔹 Predict 설정
	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = Start;
	PredictParams.LaunchVelocity = LaunchVelocity;
	PredictParams.ProjectileRadius = 5.f;
	PredictParams.MaxSimTime = 2.f;
	PredictParams.SimFrequency = 30.f;
	PredictParams.bTraceWithCollision = true;

	TSubclassOf<AZS_bomb> ProjectileClass = RightGA->GetProjectileClass();

	if (!ProjectileClass)
		return;

	AZS_bomb* DefaultProjectile = ProjectileClass->GetDefaultObject<AZS_bomb>();

	if (!DefaultProjectile || !DefaultProjectile->ProjectileMovement)
		return;

	// 🔥 Gravity 맞추기
	float GravityZ = GetWorld()->GetGravityZ() * DefaultProjectile->ProjectileMovement->ProjectileGravityScale;
	PredictParams.OverrideGravityZ = GravityZ;

	// 🔥 Radius도 맞추기 (선택 but 강추)
	PredictParams.ProjectileRadius = DefaultProjectile->CollisionComp->GetScaledSphereRadius();

	PredictParams.ActorsToIgnore.Add(GetOwner());

	FPredictProjectilePathResult Result;

	UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, Result);

	// ❗ Path 없으면 종료
	if (Result.PathData.Num() == 0)
		return;

	// 🔹 궤적 그리기
	for (int i = 0; i < Result.PathData.Num() - 1; i++)
	{
		DrawDebugLine(
			GetWorld(),
			Result.PathData[i].Location,
			Result.PathData[i + 1].Location,
			FColor::Blue,
			false,
			0.f,
			0,
			5.f
		);
	}

	// 🔹 낙하지점 계산
	FVector ImpactPoint;

	if (Result.HitResult.bBlockingHit)
	{
		ImpactPoint = Result.HitResult.Location;
	}
	else
	{
		ImpactPoint = Result.PathData.Last().Location;
	}

	// 🔥 원 방향 계산
	FVector X, Y;

	if (Result.HitResult.bBlockingHit)
	{
		FVector Normal = Result.HitResult.ImpactNormal;

		// Normal이 UpVector랑 평행하면 Cross가 0됨 → 예외 처리
		X = FVector::CrossProduct(Normal, FVector::UpVector);

		if (X.IsNearlyZero())
		{
			X = FVector::RightVector;
		}

		X.Normalize();
		Y = FVector::CrossProduct(Normal, X);
	}
	else
	{
		// 충돌 없으면 그냥 평평한 XY 원
		X = FVector(1, 0, 0);
		Y = FVector(0, 1, 0);
	}

	// 🔹 원 위치 살짝 띄우기 (Z-fighting 방지)
	FVector DrawLocation = ImpactPoint + FVector(0, 0, 5.f);

	// 🔹 폭발 범위 표시
	DrawDebugCircle(
		GetWorld(),
		DrawLocation,
		150.f,
		32,
		FColor::Red,
		false,
		0.f,
		0,
		2.f,
		X,
		Y,
		false
	);
}

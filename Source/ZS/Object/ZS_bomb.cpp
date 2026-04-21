// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/ZS_bomb.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AZS_bomb::AZS_bomb()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComp);

	BombMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	BombMeshComp->SetupAttachment(CollisionComp);
	BombMeshComp->SetSimulatePhysics(false);

	CollisionComp->InitSphereRadius(16.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetUseCCD(true);


	CollisionComp->OnComponentBeginOverlap.AddDynamic(
		this,
		&AZS_bomb::OnOverlap
	);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraComp->SetupAttachment(CollisionComp);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeedMax;
	ProjectileMovement->ProjectileGravityScale = 1.0f; //중력!!
	ProjectileMovement->bRotationFollowsVelocity = true;

	InitialLifeSpan = LifeSeconds;
}

TObjectPtr<UProjectileMovementComponent> AZS_bomb::Get_ProjectileComponent()
{
	return ProjectileMovement;
}

void AZS_bomb::InitProjectile(FVector InStart, FVector InTarget, float ThrowAngle)
{
	FVector LaunchVelocity;

	UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *LaunchVelocity.ToString());

	bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this,
		LaunchVelocity,
		InStart,
		InTarget,
		0.f, // 중력 override (0이면 기본 중력 사용)
		ThrowAngle
	);

	if (bSuccess)
	{
		FVector NewVelocity = LaunchVelocity;

		// NewVelocity.X *= VelocityWeight;
		// NewVelocity.Y *= VelocityWeight;

		ProjectileMovement->Velocity = NewVelocity;

		// ProjectileMovement->Velocity = LaunchVelocity * VelocityWeight;
	}
}

void AZS_bomb::InitProjectile_Velocity(FVector LaunchVelocity)
{
	if (!ProjectileMovement)
		return;

	ProjectileMovement->StopMovementImmediately();

	ProjectileMovement->Velocity = LaunchVelocity;
	ProjectileMovement->UpdateComponentVelocity();

	ProjectileMovement->Activate(true);
}

// Called when the game starts or when spawned
void AZS_bomb::BeginPlay()
{
	Super::BeginPlay();

	SpinRate = FRotator(
		FMath::RandRange(-600.f, 600.f),
		FMath::RandRange(-600.f, 600.f),
		FMath::RandRange(-600.f, 600.f)
	);

	PostInitializeComponents();
}

void AZS_bomb::Move(float DeltaTime)
{
}

void AZS_bomb::Set_GE(TSubclassOf<UGameplayEffect> ParaGE, FGameplayEffectContextHandle InContext)
{
	DamageEffect = ParaGE;
	EffectContext = InContext;
}

void AZS_bomb::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                               FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;

	ApplyDamageToTarget(OtherActor);

	Destroy(); // 맞으면 파괴
}

void AZS_bomb::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;

	ApplyDamageToTarget(OtherActor);
	SpantImpact();
	Destroy();
}

void AZS_bomb::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CollisionComp)
	{
		// CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AZS_bomb::OnOverlap);
		CollisionComp->OnComponentHit.AddDynamic(this, &AZS_bomb::OnHit);
	}
}

// Called every frame
void AZS_bomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(SpinRate * DeltaTime);
	// Move(DeltaTime);
}

void AZS_bomb::Explode()
{
	if (ExplosionEffect)
	{
		FVector Scale = FVector(1.0f);

		UNiagaraComponent* NewExplosion =
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				ExplosionEffect,
				GetActorLocation(),
				FRotator::ZeroRotator,
				Scale
			);
		if (NewExplosion)
		{
		
			
			NewExplosion->SetVariableFloat(TEXT("User.Size"), Effect_Magnification);
			NewExplosion->SetVariableFloat(TEXT("User.SpawnRate"), Effect_Magnification);
			NewExplosion->SetVariableFloat(TEXT("User.SpawnSize"), Effect_Magnification);
		}
	}

	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),
		Explosion_Radius,
		32,
		FColor::Red,
		false,
		2.0f,
		0,
		2.0f // 두께
	);

	Apply_RadialDamage();

	Destroy();
}

void AZS_bomb::Apply_RadialDamage()
{
	TArray<FOverlapResult> Overlaps;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(Explosion_Radius);

	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		Sphere
	);

	for (auto& Result : Overlaps)
	{
		AActor* HitActor = Result.GetActor();
		if (!HitActor)
			continue;

		UAbilitySystemComponent* ASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (!ASC)
			continue;

		// GameplayEffect 적용
		FGameplayEffectContextHandle Context =
			ASC->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle Spec =
			ASC->MakeOutgoingSpec(DamageEffect, 1.f, Context);

		if (Spec.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
}

void AZS_bomb::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                     FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == this || OtherActor == GetOwner())
		return;

	Explode();
}


void AZS_bomb::SpantImpact()
{
	// if (ImpactVFX)
	// {
	// 	FVector SpawnLocation = GetActorLocation();
	// 	FRotator SpawnRotation = GetActorRotation();
	//
	// 	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
	// 		GetWorld(),
	// 		ImpactVFX,
	// 		SpawnLocation,
	// 		SpawnRotation
	// 	);
	// }
}

void AZS_bomb::ApplyDamageToTarget(AActor* TargetActor)
{
	if (!TargetActor || !DamageEffect) return;

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	TargetASC->ApplyGameplayEffectToSelf(
		DamageEffect->GetDefaultObject<UGameplayEffect>(),
		1.f,
		EffectContext
	);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/ZSProjectile.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AZSProjectile::AZSProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComp);

	CollisionComp->InitSphereRadius(16.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetUseCCD(true);
	
	CollisionComp->OnComponentBeginOverlap.AddDynamic(
		this,
		&AZSProjectile::OnOverlap
	);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraComp->SetupAttachment(CollisionComp);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeedMax;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	InitialLifeSpan = 15.f;
}

// Called when the game starts or when spawned
void AZSProjectile::BeginPlay()
{
	Super::BeginPlay();

	PostInitializeComponents();
}

void AZSProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// UE_LOG(LogTemp, Warning, TEXT("HIT"));

	if (!OtherActor || OtherActor == GetOwner())
		return;

	ApplyDamageToTarget(OtherActor);

	Destroy(); // 맞으면 파괴
}

void AZSProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// UE_LOG(LogTemp, Warning, TEXT("OVERLAP"));

	if (!OtherActor || OtherActor == GetOwner())
		return;

	ApplyDamageToTarget(OtherActor);
	SpantImpact();
	Destroy();
}

void AZSProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CollisionComp)
	{
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AZSProjectile::OnOverlap);
	}
}

void AZSProjectile::SpantImpact()
{
	if (ImpactVFX)
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactVFX,
			SpawnLocation,
			SpawnRotation
		);
	}
}

void AZSProjectile::ApplyDamageToTarget(AActor* TargetActor)
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

// Called every frame
void AZSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// DrawDebugSphere(
	// 	GetWorld(),
	// 	CollisionComp->GetComponentLocation(),
	// 	CollisionComp->GetScaledSphereRadius(),
	// 	12,
	// 	FColor::Red,
	// 	false,
	// 	0.1f
	// );
}

void AZSProjectile::Set_GE(TSubclassOf<UGameplayEffect> ParaGE, FGameplayEffectContextHandle InContext)
{
	DamageEffect = ParaGE;
	EffectContext = InContext;
}

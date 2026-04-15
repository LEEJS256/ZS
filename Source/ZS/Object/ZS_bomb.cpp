// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/ZS_bomb.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AZS_bomb::AZS_bomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BombMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	SetRootComponent(BombMeshComp);
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	// SetRootComponent(CollisionComp);
	CollisionComp->SetupAttachment(BombMeshComp);

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
	ProjectileMovement->ProjectileGravityScale = 1.f; //중력!!
	ProjectileMovement->bRotationFollowsVelocity = true;

	InitialLifeSpan = LifeSeconds;
	
}

// Called when the game starts or when spawned
void AZS_bomb::BeginPlay()
{
	Super::BeginPlay();

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
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AZS_bomb::OnOverlap);
	}
}

// Called every frame
void AZS_bomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move(DeltaTime);
}

void AZS_bomb::SpantImpact()
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


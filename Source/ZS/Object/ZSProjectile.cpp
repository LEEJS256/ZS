// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/ZSProjectile.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
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
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));

	CollisionComp->OnComponentHit.AddDynamic(this, &AZSProjectile::OnProjectileHit);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraComp->SetupAttachment(CollisionComp);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	InitialLifeSpan = 15.f;

}

// Called when the game starts or when spawned
void AZSProjectile::BeginPlay()
{
	Super::BeginPlay();

}

void AZSProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp,Warning,TEXT("HIT"));
}

// Called every frame
void AZSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


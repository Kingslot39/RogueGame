#include "RainArrow.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "BossTest.h"

ARainArrow::ARainArrow()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create collision component
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(15.0f);
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    RootComponent = CollisionComp;

    // Create arrow mesh
    ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
    ArrowMesh->SetupAttachment(RootComponent);
    ArrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Create projectile movement component
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 0.0f;
    ProjectileMovement->MaxSpeed = 3000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.0f; // We'll use custom velocity

    // Set lifespan
    InitialLifeSpan = 5.0f;
}

void ARainArrow::BeginPlay()
{
    Super::BeginPlay();
    
    // Bind hit event
    CollisionComp->OnComponentHit.AddDynamic(this, &ARainArrow::OnHit);
}

void ARainArrow::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
                       UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
                       const FHitResult& Hit)
{
    // Ignore self and owner
    if (OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }

    // Check if it's an enemy
    if (Cast<AEnemy>(OtherActor) || Cast<ABossTest>(OtherActor))
    {
        // Apply damage
        UGameplayStatics::ApplyPointDamage(
            OtherActor,
            Damage,
            GetActorLocation(),
            Hit,
            nullptr,
            this,
            UDamageType::StaticClass()
        );
    }

    // Destroy the arrow
    Destroy();
}
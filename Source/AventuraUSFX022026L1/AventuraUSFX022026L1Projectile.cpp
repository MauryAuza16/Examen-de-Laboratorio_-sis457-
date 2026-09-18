// Copyright Epic Games, Inc. All Rights Reserve

#include "AventuraUSFX022026L1Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "PlataformaDestructible.h"
#include "PlataformaIndestructible.h"

AAventuraUSFX022026L1Projectile::AAventuraUSFX022026L1Projectile() 
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AAventuraUSFX022026L1Projectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	ProjectileMovement->Bounciness = 1.0f;
	ProjectileMovement->Friction = 0.0f;

	// Die after 3 seconds by default
	InitialLifeSpan = 0.0f;
}

void AAventuraUSFX022026L1Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Si chocamos contra un objeto con fisica simulada, le aplicamos un impulso (sin cambios)
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
	}

	// PlataformaDestructible: se destruye. La pelota sigue rebotando.
	APlataformaDestructible* PlataformaDestruible = Cast<APlataformaDestructible>(OtherActor);
	if (PlataformaDestruible != nullptr)
	{
		PlataformaDestruible->Destroy();
		return;
	}

	// PlataformaIndestructible: NO se destruye, solo rebota (gracias a bShouldBounce = true)
	APlataformaIndestructible* PlataformaIndestruible = Cast<APlataformaIndestructible>(OtherActor);
	if (PlataformaIndestruible != nullptr)
	{
		return;
	}
}
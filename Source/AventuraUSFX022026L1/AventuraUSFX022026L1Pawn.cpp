// Copyright Epic Games, Inc. All Rights Reserved.

#include "AventuraUSFX022026L1Pawn.h"
#include "AventuraUSFX022026L1Projectile.h"
#include "PlataformaIndestructible.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

const FName AAventuraUSFX022026L1Pawn::MoveForwardBinding("MoveForward");
const FName AAventuraUSFX022026L1Pawn::MoveRightBinding("MoveRight");
const FName AAventuraUSFX022026L1Pawn::FireForwardBinding("FireForward");
const FName AAventuraUSFX022026L1Pawn::FireRightBinding("FireRight");

AAventuraUSFX022026L1Pawn::AAventuraUSFX022026L1Pawn()
{
	// El Pawn ahora usa la misma malla que las plataformas, para parecerse a una ---
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/StarterContent/Shapes/Shape_Plane.Shape_Plane"));
	// Creamos el componente de malla
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);

	// Escalamos la malla para que se vea como una plataforma/paleta alargada (ajusta a tu gusto)
	SetActorScale3D(FVector(2.0f, 4.0f, 0.2f));

	// Guardamos el efecto de sonido
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	// Creamos un brazo de cámara
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);				//El brazo no debe rotar cuando lo hace el Pawn
	CameraBoom->TargetArmLength = 2900.f;					//Para cambiar la distancia de la cámara respecto al Pawn
	CameraBoom->TargetOffset = FVector (1500.f, 0.f, 0.f);	//Para cambiar la altura de la cámara respecto al Pawn
	CameraBoom->SetRelativeRotation(FRotator (-80.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;					// No queremos que la cámara se acerque al chocar con el nivel

	// Creamos una cámara
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// la cámara no rota respecto al brazo

	// Movimiento
	MoveSpeed = 1000.0f;
	// Arma
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
	bYaDisparo = false;					// Inicialmente, el Pawn no ha disparado
}

void AAventuraUSFX022026L1Pawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Configuramos los bindings de entrada del juego
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);
}

void AAventuraUSFX022026L1Pawn::Tick(float DeltaSeconds)
{
	/* Ya NO leemos el eje "adelante" (MoveForwardBinding).
	El Pawn ahora solo se mueve de izquierda a derecha (eje Y = MoveRightBinding),
	tal como se le pide a una plataforma/paleta dentro del miniescenario.*/

	const float RightValue = GetInputAxisValue(MoveRightBinding);
	const FVector MoveDirection = FVector(0.f, RightValue, 0.f).GetClampedToMaxSize(1.0f);

	// Calculamos el movimiento
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	// Si el movimiento es distinto de cero, movemos este actor
	if (Movement.SizeSquared() > 0.0f)
	{
		//const FVector NewLocation = Movement.Rotation;	//Hace que el Pawn rote hacia donde se mueve
		const FRotator NewRotation = GetActorRotation();	//Esto hace que el Pawn NO rote hacia donde se mueve
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);

		if (Hit.IsValidBlockingHit())
		{
			/* Si el Pawn choca con una PlataformaIndestructible
			(que ya cayó hasta la base), la destruye. Es el efecto CONTRARIO al
			de la pelota, que destruye las PlataformaDestructible y rebota en
			las PlataformaIndestructible.*/

			APlataformaIndestructible* PlataformaGolpeada = Cast<APlataformaIndestructible>(Hit.GetActor());
			if (PlataformaGolpeada != nullptr)
			{
				PlataformaGolpeada->Destroy();
			}

			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
			RootComponent->MoveComponent(Deflection, NewRotation, true);
		}
	}

	// Creamos el vector de dirección de disparo (esto NO cambia: apuntar y disparar sigue siendo libre)
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);
	const float FireRightValue = GetInputAxisValue(FireRightBinding);
	const FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);

	// Intentamos disparar
	FireShot(FireDirection);
}

void AAventuraUSFX022026L1Pawn::FireShot(FVector FireDirection)
{
	//Si ya se ha disparado, no hacemos nada
	if (bYaDisparo == true)
	{
		return;
	}

	// Si ya se puede disparar de nuevo
	if (bCanFire == true)
	{
		// Si se está presionando el stick/tecla de disparo en alguna dirección
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Generamos la pelota con un desplazamiento respecto al Pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				// generamos la pelota
				World->SpawnActor<AAventuraUSFX022026L1Projectile>(SpawnLocation, FireRotation);

				bYaDisparo = true; // Marcamos que ya se disparó, para que no se pueda disparar de nuevo hasta que se reinicie el nivel
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AAventuraUSFX022026L1Pawn::ShotTimerExpired, FireRate);

			// intentamos reproducir el sonido si está especificado
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void AAventuraUSFX022026L1Pawn::ShotTimerExpired()
{
	bCanFire = true;
}
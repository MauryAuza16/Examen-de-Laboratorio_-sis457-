// Fill out your copyright notice in the Description page of Project Settings.

#include "Miniescenario.h"
#include "PlataformaIndestructible.h"
#include "PlataformaDestructible.h"
#include "TimerManager.h"

AMiniescenario::AMiniescenario()
{
	PrimaryActorTick.bCanEverTick = false; // este actor no necesita Tick, solo un timer

	IntervaloSpawn = 5.0f;
}

void AMiniescenario::BeginPlay()
{
	Super::BeginPlay();

	// Repite SpawnearPlataformas cada IntervaloSpawn segundos, indefinidamente (true = looping)
	GetWorldTimerManager().SetTimer(TimerSpawn, this, &AMiniescenario::SpawnearPlataformas, IntervaloSpawn, true);
}

void AMiniescenario::SpawnearPlataformas()
{
	UWorld* Mundo = GetWorld();
	if (Mundo == nullptr)
	{
		return;
	}

	// Por cada área configurada, decidimos al azar qué tipo de plataforma aparece
	for (const FVector& Posicion : PosicionesDeSpawn)
	{
		const FRotator Rotacion = FRotator::ZeroRotator;
		const bool bEsIndestructible = FMath::RandBool();

		if (bEsIndestructible)
		{
			Mundo->SpawnActor<APlataformaIndestructible>(Posicion, Rotacion);
		}
		else
		{
			Mundo->SpawnActor<APlataformaDestructible>(Posicion, Rotacion);
		}
	}
}
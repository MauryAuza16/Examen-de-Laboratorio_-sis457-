// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Miniescenario.generated.h"

/*
 Se coloca una sola vez en el nivel del miniescenario.
 Cada "IntervaloSpawn" segundos, genera una plataforma aleatoria
 (Indestructible o Destructible) en cada posicion de PosicionesDeSpawn.
 Configura en el Details panel una posicion por cada una de las 3 areas
 (la X debe ser la parte SUPERIOR de esa area).
 */
UCLASS()
class AVENTURAUSFX022026L1_API AMiniescenario : public AActor
{
	GENERATED_BODY()

public:
	AMiniescenario();

protected:
	virtual void BeginPlay() override;

public:
	/** Una posicion de spawn por cada area (normalmente 3) */
	UPROPERTY(EditAnywhere, Category = "Miniescenario")
	TArray<FVector> PosicionesDeSpawn;

	/** Cada cuantos segundos aparece una plataforma nueva en cada area */
	UPROPERTY(EditAnywhere, Category = "Miniescenario")
	float IntervaloSpawn;

private:
	FTimerHandle TimerSpawn;

	void SpawnearPlataformas();
};
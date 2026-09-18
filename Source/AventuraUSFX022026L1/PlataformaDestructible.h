// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plataforma.h"
#include "PlataformaDestructible.generated.h"

/*
 Plataforma que SI puede ser destruida por la pelota (se destruye al chocar con ella).
 Nace en la parte superior de su area y cae en linea recta hasta la base del miniescenario.
 NO puede ser destruida por el Pawn en la base (efecto contrario al de PlataformaIndestructible).
 */
UCLASS()
class AVENTURAUSFX022026L1_API APlataformaDestructible : public APlataforma
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	// Tiempo acumulado en segundos
	float TiempoAcumulado = 0.0f;

	// Intervalo de tiempo para cada bajada (5 segundos)
	float IntervaloBajada = 5.0f;

	// Distancia fija en unidades de Unreal que bajará cada 5 segundos
	float DistanciaBajada = 500.0f;

	FVector PosicionInicial;
public:
	APlataformaDestructible();

	virtual void Tick(float DeltaTime) override;

	/* Velocidad de caida hacia la base del miniescenario (unidades por segundo) */
	UPROPERTY(EditAnywhere, Category = "Miniescenario")
	float VelocidadDeCaida;

	/* Coordenada X que representa la base del miniescenario (ahi se detiene al caer) */
	UPROPERTY(EditAnywhere, Category = "Miniescenario")
	float PosicionBaseEjeX;	
	/* true cuando la plataforma ya llego a la base y dejo de moverse */
	bool bLlegoAlFinal;
};
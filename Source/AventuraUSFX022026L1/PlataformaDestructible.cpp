// Fill out your copyright notice in the Description page of Project Settings.

#include "PlataformaDestructible.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

void APlataformaDestructible::BeginPlay()
{
	Super::BeginPlay();
	// Guardamos la posición con la que la plataforma aparece en el mapa
	PosicionInicial = GetActorLocation();
}

APlataformaDestructible::APlataformaDestructible()
{
	PrimaryActorTick.bCanEverTick = true;

	// Malla: una pirámide, para que se note distinta (y más "frágil") frente al cubo de la Indestructible
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlataformaMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_TriPyramid.Shape_TriPyramid'"));
	if (mallaPlataforma != nullptr)
	{
		mallaPlataforma->SetStaticMesh(PlataformaMeshAsset.Object);

		// Color distintivo (naranja). Mismo comentario que en
		// PlataformaIndestructible sobre el nombre del parámetro "Color".
		UMaterialInstanceDynamic* MaterialDinamico = mallaPlataforma->CreateDynamicMaterialInstance(0);
		if (MaterialDinamico != nullptr)
		{
			MaterialDinamico->SetVectorParameterValue(TEXT("Color"), FLinearColor (0.8f, 0.4f, 0.1f));
		}
	}

	tipoPlataforma = ETipoPlataforma::PLATAFORMA_AEREA; // no participa en la lógica de este laboratorio

	VelocidadDeCaida = 500.0f;
	PosicionBaseEjeX = -1500.0f;
	bLlegoAlFinal = false;
}

void APlataformaDestructible::Tick(float DeltaTime)
{
	// Igual que en PlataformaIndestructible: saltamos APlataforma::Tick a propósito
	AActor::Tick(DeltaTime);

	/*if (bLlegoAlFinal)
	{
		return;
	}

	FVector PosicionActual = GetActorLocation();

	if (PosicionActual.X <= PosicionBaseEjeX)
	{
		bLlegoAlFinal = true;
		return;
	}*/

	TiempoAcumulado += DeltaTime;

	if (TiempoAcumulado >= IntervaloBajada)
	{
		FVector PosicionActual = GetActorLocation();

		// Si ya llegó o superó el límite inferior, regresa a la posición inicial (arriba)
		if (PosicionActual.X <= PosicionBaseEjeX)
		{
			SetActorLocation(PosicionInicial);
		}
		else
		{
			// Si no ha llegado abajo, baja un paso en X
			PosicionActual.X -= DistanciaBajada;
			SetActorLocation(PosicionActual);
		}

		// Reinicia el temporizador conservando el residuo
		TiempoAcumulado -= IntervaloBajada;
	}
	/*PosicionActual.X -= VelocidadDeCaida * DeltaTime;
	SetActorLocation(PosicionActual);*/
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "PlataformaIndestructible.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

void APlataformaIndestructible::BeginPlay()
{
	Super::BeginPlay();
	PosicionInicial = GetActorLocation();
}

APlataformaIndestructible::APlataformaIndestructible()
{
	PrimaryActorTick.bCanEverTick = true;

	// Malla: un cubo, para diferenciarla visualmente del resto de plataformas
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlataformaMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (mallaPlataforma != nullptr)
	{
		mallaPlataforma->SetStaticMesh(PlataformaMeshAsset.Object);

		/* Esto asume que el material base del Shape tiene un parámetro de color llamado "Color"; si en tu proyecto
		se llama distinto, abrelo en el editor de materiales y ajusta el nombre aqui. */
		UMaterialInstanceDynamic* MaterialDinamico = mallaPlataforma->CreateDynamicMaterialInstance(0);
		if (MaterialDinamico != nullptr)
		{
			MaterialDinamico->SetVectorParameterValue(TEXT("Color"), FLinearColor (1.0f, 1.0f, 1.0f));
		}
	}

	/*El enum tipoPlataforma es obligatorio por la clase base, pero no participa
	en la lógica de este laboratorio (solo lo usa el GameMode existente para otra practica)*/
	tipoPlataforma = ETipoPlataforma::PLATAFORMA_TERRESTRE;

	VelocidadDeCaida = 500.0f;
	PosicionBaseEjeX = -1500.0f;
	bLlegoAlFinal = false;
}

void APlataformaIndestructible::Tick(float DeltaTime)
{
	/* NO llamamos a Super::Tick(DeltaTime), porque APlataforma::Tick
	trae su propia lógica de movimiento (rebote entre límites aleatorios cada 15s)
	que no queremos aquí. Llamamos directamente a AActor::Tick para saltarnos esa lógica.
	AActor::Tick(DeltaTime);

	if (bLlegoAlFinal)
	{
		return;
	}

	FVector PosicionActual = GetActorLocation();

	if (PosicionActual.X <= PosicionBaseEjeX)
	{
		// Ya llegó a la base: se detiene y queda esperando a que el Pawn la choque
		bLlegoAlFinal = true;
		return;
	}*/

	// 1. Acumular el tiempo del cuadro
	TiempoAcumulado += DeltaTime;

	// 2. Verificar si transcurrieron 5 segundos
	if (TiempoAcumulado >= IntervaloBajada)
	{
		FVector PosicionActual = GetActorLocation();

		if (PosicionActual.X <= PosicionBaseEjeX)
		{
			SetActorLocation(PosicionInicial);
		}
		else
		{
			PosicionActual.X -= DistanciaBajada;
			SetActorLocation(PosicionActual);
		}

		TiempoAcumulado -= IntervaloBajada;
	}
	// Cae en línea recta hacia la base (eje X negativo)
	/*PosicionActual.X -= VelocidadDeCaida * DeltaTime;
	SetActorLocation(PosicionActual);*/
}
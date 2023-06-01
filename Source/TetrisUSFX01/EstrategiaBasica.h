// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EstrategiasInterfaz.h"
#include "EstrategiaBasica.generated.h"

UCLASS()
class TETRISUSFX01_API AEstrategiaBasica : public AActor, public IEstrategiasInterfaz
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEstrategiaBasica();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame

	virtual void Tick(float DeltaTime) override;
	virtual std::pair<bool, bool> PosicionarPieza(APieza* pieza, bool flagMov, bool flagInicFun) override;

};

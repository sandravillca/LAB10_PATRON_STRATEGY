// Fill out your copyright notice in the Description page of Project Settings.


#include "EstrategiaBasica.h"

// Sets default values
AEstrategiaBasica::AEstrategiaBasica()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEstrategiaBasica::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEstrategiaBasica::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

std::pair<bool, bool> AEstrategiaBasica::PosicionarPieza(APieza* pieza,bool flagMov, bool flagInicFun)
{
    if (pieza) {
        auto Location = FVector(0, 0, 0);
        if (flagInicFun == true) {
            Location = FVector(0.0, 5, 175);
            pieza->SetActorLocation(Location);
            flagInicFun = false;
        }
        flagMov = true;

    }
    return std::make_pair(flagMov, flagInicFun);
}


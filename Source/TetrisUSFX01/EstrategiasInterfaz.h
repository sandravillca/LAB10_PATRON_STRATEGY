// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pieza.h"
#include <utility>
#include "EstrategiasInterfaz.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEstrategiasInterfaz : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TETRISUSFX01_API IEstrategiasInterfaz
{
	GENERATED_BODY()
public:
	virtual std::pair<bool, bool> PosicionarPieza(APieza* pieza, bool flagMov, bool flagInicFun) = 0;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};

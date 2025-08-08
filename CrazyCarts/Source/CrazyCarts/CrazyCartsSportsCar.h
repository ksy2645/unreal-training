// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CrazyCartsPawn.h"
#include "CrazyCartsSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class CRAZYCARTS_API ACrazyCartsSportsCar : public ACrazyCartsPawn
{
	GENERATED_BODY()
	
public:

	ACrazyCartsSportsCar();
};

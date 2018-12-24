// Ashley M Sands (gizzmo123456) 2018

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseAudioActor.generated.h"

class USoundWave;

UCLASS()
class ACTIONCUE_API ABaseAudioActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseAudioActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Audio" )
		USoundWave* audioClip;
	
};

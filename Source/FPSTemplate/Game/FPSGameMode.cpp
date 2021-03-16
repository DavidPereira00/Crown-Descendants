#include "FPSTemplate.h"
#include "Game/FPSGameMode.h"
#include "Player/BasePlayerController.h"
#include "Player/BasePlayerState.h"
#include "Game/FPSGameState.h"
#include "BaseWorldSettings.h"

AFPSGameMode::AFPSGameMode(const FObjectInitializer & ObjectInitializer)
{
	LoadoutBuilderComponent = ObjectInitializer.CreateDefaultSubobject<ULoadoutBuilderComponent>(this, TEXT("LoadoutBuilderComponent"));

	static ConstructorHelpers::FObjectFinder<UClass> NewSoldierClass(TEXT("Blueprint'/Game/FPSTemplate/Soldier/BP_Soldier.BP_Soldier_C'"));
	SoldierClass = NewSoldierClass.Object;

	DefaultPawnClass = NULL;
	SpectatorClass = NULL;
	PlayerControllerClass = ABasePlayerController::StaticClass();
	PlayerStateClass = ABasePlayerState::StaticClass();
	GameStateClass = AFPSGameState::StaticClass();

	MinRespawnDelay = 5.f;

	bUseCustomLoadout = true;
	TeamMode = ETeamModeEnum::MultipleTeams;
}

void AFPSGameMode::InitGameState()
{
	Super::InitGameState();

	AFPSGameState * FPSGameState = Cast<AFPSGameState>(GameState);
	if (FPSGameState)
	{
		// Max length of match
		FPSGameState->TimeRemaining = 600;
	}
}

void AFPSGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ABasePlayerController * PC = Cast<ABasePlayerController>(NewPlayer);
	if (PC)
	{
		if (HasMatchEnded())
		{
			PC->EnterMatchEndState();
		}
		else
		{
			switch (TeamMode)
			{
			case (ETeamModeEnum::MultipleTeams) :
				PC->EnterTeamSelectionState();
				break;
			case (ETeamModeEnum::OneTeam) :
				PC->SetTeam(ETeamEnum::TeamA);
				MovePlayerToEntryState(PC);
				break;
			case (ETeamModeEnum::FreeForAll) :
				// Leave player in None team, which is used as a free for all team type
				MovePlayerToEntryState(PC);
				break;
			}
		}
	}
}

void AFPSGameMode::Logout(AController * Exiting)
{
	//ABasePlayerController * PC = Cast<ABasePlayerController>(Exiting);
	if (Exiting)
	{
		ASoldier * Soldier = Cast<ASoldier>(Exiting->GetPawn());
		if (Soldier)
		{
			Soldier->Destroy();
		}
	}

	Super::Logout(Exiting);
}

void AFPSGameMode::Reset()
{
	// Do nothing
}

AActor * AFPSGameMode::FindPlayerStart(AController * Player)
{
	srandom = FMath::RandRange(1, 7);
	FString StartTag = "";
	if (srandom == 1)
	{
		StartTag = "ffa1";
	}
	if (srandom == 2)
	{
		StartTag = "ffa2";
	}
	if (srandom == 3)
	{
		StartTag = "ffa3";
	}
	if (srandom == 4)
	{
		StartTag = "ffa4";
	}
	if (srandom == 5)
	{
		StartTag = "ffa5";
	}
	if (srandom == 6)
	{
		StartTag = "ffa6";
	}
	if (srandom == 7)
	{
		StartTag = "ffa7";
	}
	ABasePlayerController * PC = Cast<ABasePlayerController>(Player);
	if (PC)
	{
		switch (PC->GetTeam())
		{
		case (ETeamEnum::TeamA) :
			StartTag = "TeamA";
			break;
		case (ETeamEnum::TeamB) :
			StartTag = "TeamB";
			break;
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("My tag is: %d"), srandom));
	return FindPlayerStart_Implementation(Player, StartTag);
}

//AActor * AFPSGameMode::ChoosePlayerStart_Implementation(AController * Player)
//{
//	return nullptr;
//}

void AFPSGameMode::MovePlayerToEntryState(ABasePlayerController * PC)
{
	PC->EnterSpawnState();
}

void AFPSGameMode::OnTimeExpired()
{
	// By default no team wins when match time expires
	EndMatch();
}

void AFPSGameMode::EndMatch()
{
	Super::EndMatch();

	// Transition all players to match end state
	MoveAllPlayersToState(EPlayerStateEnum::MatchEnd);
}

void AFPSGameMode::EndMatch(ETeamEnum WinnerTeam)
{
	if (IsMatchInProgress())
	{
		AFPSGameState * FPSGameState = Cast<AFPSGameState>(GameState);
		if (FPSGameState)
		{
			FPSGameState->MatchWinner = WinnerTeam;
		}

		EndMatch();
	}
}

void AFPSGameMode::EndMatch(ABasePlayerState * WinnerPS)
{
	if (IsMatchInProgress())
	{
		AFPSGameState * FPSGameState = Cast<AFPSGameState>(GameState);
		if (FPSGameState)
		{
			FPSGameState->WinnerPlayerState = WinnerPS;
		}

		EndMatch();
	}
}

void AFPSGameMode::MoveAllPlayersToState(EPlayerStateEnum NewState)
{
	// Save enter state function pointer so we don't have to do the switch for every player
	typedef void (ABasePlayerController::*FunctionPtr)(void);
	FunctionPtr Function = &ABasePlayerController::EnterSpectatingState;
	switch (NewState)
	{
	case (EPlayerStateEnum::TeamSelection) :
		Function = &ABasePlayerController::EnterTeamSelectionState;
		break;
	case (EPlayerStateEnum::Spawn) :
		Function = &ABasePlayerController::EnterSpawnState;
		break;
	case (EPlayerStateEnum::Playing) :
		Function = &ABasePlayerController::EnterPlayingState;
		break;
	case (EPlayerStateEnum::Death) :
		Function = &ABasePlayerController::EnterDeathState;
		break;
	case (EPlayerStateEnum::Spectating) :
		Function = &ABasePlayerController::EnterSpectatingState;
		break;
	case (EPlayerStateEnum::MatchEnd) :
		Function = &ABasePlayerController::EnterMatchEndState;
		break;
	}

	for (FConstPlayerControllerIterator PlayerItr = GetWorld()->GetPlayerControllerIterator(); PlayerItr; ++PlayerItr)
	{
		ABasePlayerController * PC = Cast<ABasePlayerController>(*PlayerItr);
		if (PC)
		{
			(PC->*Function)();
		}
	}
}

void AFPSGameMode::CalcDamage(float & OutDamageAmount, ABasePlayerController * PC, ABasePlayerController * OtherPC)
{
	if (OtherPC && OtherPC != PC && !bFriendlyFire && !PC->IsEnemyFor(OtherPC))
	{
		OutDamageAmount = 0.f;
	}
}

void AFPSGameMode::OnSoldierDied(ASoldier * Soldier, const FDamageEvent & DamageEvent, ABasePlayerController * EventInstigator, AActor * DamageCauser)
{
	ABasePlayerState * PS = Cast<ABasePlayerState>(Soldier->PlayerState);
	if (PS)
	{
		ABasePlayerState * OtherPS = Cast<ABasePlayerState>(EventInstigator != NULL ? EventInstigator->PlayerState : NULL);

		// Notify all players that a soldier was killed
		for (FConstPlayerControllerIterator PlayerItr = GetWorld()->GetPlayerControllerIterator(); PlayerItr; ++PlayerItr)
		{
			ABasePlayerController * BasePlayer = Cast<ABasePlayerController>(*PlayerItr);
			if (BasePlayer)
			{
				BasePlayer->ClientOnPlayerDied(TSubclassOf<UBaseDamageType>(*DamageEvent.DamageTypeClass), PS, OtherPS);
			}
		}

		// Dead soldier player controller
		ABasePlayerController * PC = Cast<ABasePlayerController>(Soldier->Controller);
		if (PC)
		{
			OnPlayerDied(PC);

			if (EventInstigator)
			{
				if (PS != OtherPS && PS->IsEnemyFor(OtherPS))
				{
					OnPlayerKilledEnemy(DamageEvent, EventInstigator, PC);
				}
				else
				{
					OnPlayerKilledTeammate(DamageEvent, EventInstigator, PC);
				}
			}

			// Give kill assist points
			for (const FDamageInfo & DamageInfo : Soldier->GetDamageInfoArray())
			{
				if (DamageInfo.Instigator && DamageInfo.Instigator != EventInstigator && DamageInfo.Instigator->GetTeam() != PC->GetTeam())
				{
					ABasePlayerController * AssistPC = Cast<ABasePlayerController>(DamageInfo.Instigator);
					if (AssistPC)
					{
						AssistPC->AddScore(DamageInfo.DamageAmount);
						AssistPC->ClientOnAssistedKill(DamageInfo.DamageAmount);
					}
				}
			}
		}
	}

	// Set life span of soldier to let it disappear
	Soldier->SetLifeSpan(10.f);
}

void AFPSGameMode::OnPlayerDied(ABasePlayerController * PC)
{
	PC->AddDeath();
	PC->EnterDeathState();
}

void AFPSGameMode::OnPlayerKilledEnemy(const FDamageEvent & DamageEvent, ABasePlayerController * PC, ABasePlayerController * OtherPC)
{
	PC->AddKill();
	UBaseDamageType * DamageTypeCDO = Cast<UBaseDamageType>(DamageEvent.DamageTypeClass.GetDefaultObject());
	PC->AddScore(DamageTypeCDO ? DamageTypeCDO->KillReward : 100);
}

void AFPSGameMode::OnPlayerKilledTeammate(const FDamageEvent & DamageEvent, ABasePlayerController * PC, ABasePlayerController * OtherPC)
{
	// Could apply point penalty here
}

int32 AFPSGameMode::GetNumPlayersInTeam(ETeamEnum Team) const
{
	if (GameState)
	{
		int32 Num = 0;
		for (APlayerState * PS : GameState->PlayerArray)
		{
			ABasePlayerState * BasePS = Cast<ABasePlayerState>(PS);
			if (BasePS && BasePS->GetTeam() == Team)
			{
				Num++;
			}
		}

		return Num;
	}

	return 0;
}

void AFPSGameMode::PlayerJoinTeam(ABasePlayerController * PC, ETeamEnum InTeam)
{
	if (CanPlayerJoinTeam(PC, InTeam))
	{
		PC->SetTeam(InTeam);
		MovePlayerToEntryState(PC);
	}
}

void AFPSGameMode::PlayerAutoJoinTeam(ABasePlayerController * PC)
{
	// Auto select team based on number of players
	PlayerJoinTeam(PC, GetNumPlayersInTeam(ETeamEnum::TeamA) > GetNumPlayersInTeam(ETeamEnum::TeamB) ? ETeamEnum::TeamB : ETeamEnum::TeamA);
}

bool AFPSGameMode::CanPlayerJoinTeam(ABasePlayerController * PC, ETeamEnum InTeam) const
{
	if (!PC || PC->GetState() == EPlayerStateEnum::Playing || TeamMode != ETeamModeEnum::MultipleTeams || HasMatchEnded()) return false;

	uint16 NumA = GetNumPlayersInTeam(ETeamEnum::TeamA);
	uint16 NumB = GetNumPlayersInTeam(ETeamEnum::TeamB);

	// Remove PC from number of player controllers if already joined a team
	switch (PC->GetTeam())
	{
	case (ETeamEnum::TeamA) :
		NumA--;
		break;
	case (ETeamEnum::TeamB) :
		NumB--;
		break;
	}

	if (InTeam == ETeamEnum::TeamA)
	{
		return NumA <= NumB + 1;
	}
	else
	{
		return NumB <= NumA + 1;
	}

	return false;
}

void AFPSGameMode::PlayerSpawn(ABasePlayerController * PC)
{
	if (CanPlayerSpawn(PC))
	{
		AActor * PlayerStart = FindPlayerStart(PC);
		if (PlayerStart)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// Spawn soldier
			ASoldier * Soldier = GetWorld()->SpawnActor<ASoldier>(SoldierClass, PlayerStart->GetActorLocation(), FRotator(0.f, PlayerStart->GetActorRotation().Yaw, 0.f), SpawnParams);
			if (Soldier)
			{
				USkeletalMesh * SoldierMesh = NULL;
				ABaseWorldSettings * WorldSettings = Cast<ABaseWorldSettings>(GetWorldSettings());
				if (WorldSettings)
				{
					const ETeamEnum PlayerTeam = PC->GetTeam();
					const bool bTeamAMeshes = PlayerTeam == ETeamEnum::TeamA || PlayerTeam == ETeamEnum::None;
					const TArray<USkeletalMesh *> & MeshesArray = bTeamAMeshes ? WorldSettings->TeamASoldierMeshes : WorldSettings->TeamBSoldierMeshes;
					if (WorldSettings->bUseRandomSoldierMeshes)
					{
						if (MeshesArray.Num())
							SoldierMesh = MeshesArray[FMath::RandRange(0, MeshesArray.Num() - 1)];
					}
					else
					{
						const uint8 LoadoutSlot = PC->GetLoadoutSlot();
						if (MeshesArray.IsValidIndex(LoadoutSlot))
							SoldierMesh = MeshesArray[LoadoutSlot];
					}
				}

				Soldier->SetLoadout(GetPlayerLoadout(PC), SoldierMesh);
				PC->Possess(Soldier);
				PC->EnterPlayingState();
			}
		}
	}
}

bool AFPSGameMode::CanPlayerSpawn(ABasePlayerController * PC) const
{
	// Player can only spawn when match is still in progress
	return PC != NULL && !HasMatchEnded();
}


const FLoadoutStruct & AFPSGameMode::GetPlayerLoadout(ABasePlayerController * PC) const
{
	return bUseCustomLoadout ? PC->GetLoadout() : DefaultLoadout;
}

void AFPSGameMode::PlayerSetLoadout(ABasePlayerController * PC, const FLoadoutStruct & Loadout, uint8 NewLoadoutSlot)
{
	if (PC)
	{
		// Rebuild loadout to validate it and apply it to PC
		LoadoutBuilderComponent->SetLoadoutChecked(Loadout);
		PC->SetLoadout(LoadoutBuilderComponent->Loadout, NewLoadoutSlot);
	}
}


void AFPSGameMode::ServerTravel(const FString & MapName)
{
	GetWorld()->ServerTravel("/Game/FPSTemplate/Maps/" + MapName);
}

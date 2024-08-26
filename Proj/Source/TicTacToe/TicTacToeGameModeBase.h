// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TicTacToeGameModeBase.generated.h"

UENUM(BlueprintType)
enum class ETurnState :uint8
{
	Player1 = 0,	// 主控玩家
	Player2 = 1,	// 对方
	None = 2,
};

UENUM(BlueprintType)
enum class EResultState :uint8
{
	Win = 0,		// 胜利
	NoMore = 1,		// 平局
	Loss = 2,		// 失败，运行时没有用，计算分数的时候有用
	PlayerNext = 3	// 下一步，运行时有用，计算分数时无用
};

/**
 * 
 */
UCLASS()
class TICTACTOE_API ATicTacToeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	ATicTacToeGameModeBase();

public:
	//DECLARE_DELEGATE_TwoParams(FOnCellStateChanged, ETurnState /*Player*/, int /*Id*/);

	struct CellInfo
	{
		uint8 CellNumber;
		uint8 CellStepMin;		// 最佳走法
		uint8 CellStepMax;		// 最差走法
		EResultState CellResult;
		TArray<TUniquePtr<CellInfo>> Strategy;		// 策略数组
	};

	UFUNCTION(BlueprintCallable)
	ETurnState GetCurrentPlayer()const;

	UFUNCTION(BlueprintCallable)
	void SetPlayerMode(bool RobotMode);

	UFUNCTION(BlueprintCallable)
	bool IsRobotMode()const;

	UFUNCTION(BlueprintCallable)
	void TogglePlayer(bool Player1);

	UFUNCTION(BlueprintCallable)
	EResultState PlayerPlay(int Id);

	UFUNCTION(BlueprintCallable)
	int GetRobotNum(int DefaultId = -1);

	UFUNCTION(BlueprintCallable)
	void GameInit();

	UFUNCTION(BlueprintPure)
	EResultState CheckRobotResult(int Id);

	UFUNCTION(BlueprintPure)
	EResultState CheckPlayerResult(int Id);

private:
	uint8 Calculator(TUniquePtr<CellInfo>& Cell, uint8 Number, bool Player);
	bool CheckWin()const;
	bool CheckNoMore()const;
	int Pending(const TUniquePtr<CellInfo>& Node)const;

private:
	static const uint8 MaxSize{ 10 };
	static const uint8 MaxStep{ 128 };
	ETurnState ChessBoard[MaxSize];
	TUniquePtr<CellInfo> Strategy;
	TUniquePtr<CellInfo> CurrentCell;
	TUniquePtr<CellInfo> NextCell;
	bool bCurrentPlayer;
	bool bRobotMode;
};

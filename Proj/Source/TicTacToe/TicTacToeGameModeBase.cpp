// Fill out your copyright notice in the Description page of Project Settings.


#include "TicTacToeGameModeBase.h"

ATicTacToeGameModeBase::ATicTacToeGameModeBase()
{
	GameInit();
}

void ATicTacToeGameModeBase::GameInit()
{
	memset(ChessBoard, (int)ETurnState::None, MaxSize);
	Strategy.Release();
	Strategy = MakeUnique<CellInfo>();
	// 计算数据
	Calculator(Strategy, 0, true);
	bCurrentPlayer = true;
	CurrentCell.Release();
	NextCell.Release();
}

// bfs
uint8 ATicTacToeGameModeBase::Calculator(TUniquePtr<CellInfo>& Node, uint8 Id, bool Player)
{
	if (Id)
	{
		// 记录棋盘步骤
		ChessBoard[Id] = ETurnState(Player);
		Node->CellNumber = Id;
		Node->CellStepMin = MaxStep;
		Node->CellStepMax = 0;
		// 判定是否胜利，落下一子只有胜利或平局
		if (CheckWin())
		{
			Node->CellResult = EResultState::Win;
			Node->CellStepMin = (int)EResultState::Win * 10 + 1;
			Node->CellStepMax = (int)EResultState::Win * 10 + 1;
			return 1;
		}
		// 判定是否平局，落下一子只有胜利或平局
		if (CheckNoMore())
		{
			Node->CellResult = EResultState::NoMore;
			Node->CellStepMin = int(EResultState::NoMore) * 10 + 1;
			Node->CellStepMax = int(EResultState::NoMore) * 10 + 1;
			return 1;
		}
	}

	for (uint8 Index = 1; Index < MaxSize; ++Index)
	{
		if (ChessBoard[Index] == ETurnState::None)
		{
			TUniquePtr<CellInfo> Next{ MakeUnique<CellInfo>() };
			uint8 StepCur = Calculator(Next, Index, !Player);
			++StepCur;
			ChessBoard[Index] = ETurnState::None;
			// 计算分数权重，刷新最优最差策略
			switch (Next->CellResult)
			{
			case EResultState::Win:
				StepCur += (int)EResultState::Loss * 10;
				if (StepCur < Node->CellStepMin)
				{
					Node->CellResult = EResultState::Loss;
					Node->CellStepMin = StepCur;
				}
				if (StepCur > Node->CellStepMax)
				{
					Node->CellStepMax = StepCur;
				}
				break;
			case EResultState::NoMore:
				StepCur += (int)EResultState::NoMore * 10;
				if (StepCur < Node->CellStepMin)
				{
					Node->CellResult = EResultState::NoMore;
					Node->CellStepMin = StepCur;
				}
				if (StepCur > Node->CellStepMax)
				{
					Node->CellStepMax = StepCur;
				}
				break;
			case EResultState::Loss:
				StepCur += (int)EResultState::Win * 10;
				if (StepCur < Node->CellStepMin)
				{
					Node->CellResult = EResultState::Win;
					Node->CellStepMin = StepCur;
				}
				if (StepCur > Node->CellStepMax)
				{
					Node->CellStepMax = StepCur;
				}
				break;
			default:
				break;
			}
			Node->Strategy.Add(std::move(Next));
		}
	}
	return Node->CellStepMin % 10;
}

int ATicTacToeGameModeBase::Pending(const TUniquePtr<CellInfo>& Node)const
{
	if (Node->CellStepMin % 10 == 1)
	{
		return (int)Node->CellResult;
	}
	return -1;
}

EResultState ATicTacToeGameModeBase::PlayerPlay(int Id)
{
	ChessBoard[Id] = ETurnState(bCurrentPlayer);
	if (CheckWin())
	{
		return EResultState::Win;
	}
	else if (CheckNoMore())
	{
		return EResultState::NoMore;
	}
	else
	{
		return EResultState::PlayerNext;
	}
}

EResultState ATicTacToeGameModeBase::CheckPlayerResult(int Id)
{
	if (!NextCell)
	{
		NextCell = std::move(Strategy);
	}

	ChessBoard[Id] = ETurnState(bCurrentPlayer);
	for (auto& it : NextCell->Strategy)
	{
		if (it->CellNumber == Id)
		{
			CurrentCell = std::move(it);
			break;
		}
	}
	if (CheckWin())
	{
		return EResultState::Win;
	}
	if (CheckNoMore())
	{
		return EResultState::NoMore;
	}
	return EResultState::PlayerNext;
}

ETurnState ATicTacToeGameModeBase::GetCurrentPlayer()const
{
	return (ETurnState)bCurrentPlayer;
}

void ATicTacToeGameModeBase::TogglePlayer(bool Player1)
{
	bCurrentPlayer = !Player1;
}

void ATicTacToeGameModeBase::SetPlayerMode(bool RobotMode)
{
	bRobotMode = RobotMode;
}

bool ATicTacToeGameModeBase::IsRobotMode()const
{
	return bRobotMode;
}

bool ATicTacToeGameModeBase::CheckWin()const
{
	return
		(
			((ChessBoard[1] == ChessBoard[2]) && (ChessBoard[2] == ChessBoard[3]) && (ChessBoard[1] != ETurnState::None))
			||
			((ChessBoard[4] == ChessBoard[5]) && (ChessBoard[5] == ChessBoard[6]) && (ChessBoard[4] != ETurnState::None))
			||
			((ChessBoard[7] == ChessBoard[8]) && (ChessBoard[8] == ChessBoard[9]) && (ChessBoard[7] != ETurnState::None))
			||
			((ChessBoard[1] == ChessBoard[5]) && (ChessBoard[5] == ChessBoard[9]) && (ChessBoard[1] != ETurnState::None))
			||
			((ChessBoard[3] == ChessBoard[5]) && (ChessBoard[5] == ChessBoard[7]) && (ChessBoard[3] != ETurnState::None))
			||
			((ChessBoard[1] == ChessBoard[4]) && (ChessBoard[4] == ChessBoard[7]) && (ChessBoard[1] != ETurnState::None))
			||
			((ChessBoard[2] == ChessBoard[5]) && (ChessBoard[5] == ChessBoard[8]) && (ChessBoard[2] != ETurnState::None))
			||
			((ChessBoard[3] == ChessBoard[6]) && (ChessBoard[6] == ChessBoard[9]) && (ChessBoard[3] != ETurnState::None))
		);
}

bool ATicTacToeGameModeBase::CheckNoMore()const
{
	for (int Index = 1; Index < MaxSize; ++Index)
	{
		if (ChessBoard[Index] == ETurnState::None)
		{
			return false;
		}
	}
	return true;
}

EResultState ATicTacToeGameModeBase::CheckRobotResult(int Id)
{
	ChessBoard[Id] = ETurnState(bCurrentPlayer);
	UE_LOG(LogTemp, Warning, TEXT("机器人的回合：%d"), NextCell->CellNumber);
	ChessBoard[NextCell->CellNumber] = ETurnState(bCurrentPlayer);
	if (CheckWin())
	{
		return EResultState::Win;
	}
	if (CheckNoMore())
	{
		return EResultState::NoMore;
	}
	return EResultState::PlayerNext;
}

int ATicTacToeGameModeBase::GetRobotNum(int Id)
{
	if (!NextCell)
	{
		if (bCurrentPlayer)
		{
			NextCell = std::move(Strategy->Strategy[Id]);
			UE_LOG(LogTemp, Warning, TEXT("机器人的回合：%d"), Id);
			return NextCell->CellNumber;
		}
	}
	// 选择更优策略，且避免最差策略
	NextCell = std::move(CurrentCell->Strategy[0]);
	for (int i = 1; i < CurrentCell->Strategy.Num(); ++i)
	{
		if (CurrentCell->Strategy[i]->CellStepMin < NextCell->CellStepMin)
		{
			NextCell = std::move(CurrentCell->Strategy[i]);
		}
		else if((CurrentCell->Strategy[i]->CellStepMin == NextCell->CellStepMin) && (CurrentCell->Strategy[i]->CellStepMax < NextCell->CellStepMax))
		{
			NextCell = std::move(CurrentCell->Strategy[i]);
		}
	}
	return NextCell->CellNumber;
}
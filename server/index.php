<?php

	require_once("database.php");
	
	/*
	 *	User action
	*/
	
	$mode = $_GET["mode"];
	
	switch ($mode)
	{
	case "getusernum":
		echo GetUserNum();
		break;
		
	case "getmap":
		echo GetMap();
		break;
		
	case "getstate":
		echo GetState();
		break;
		
	case "getusername":
		echo GetUserName();
		break;
		
	case "getround":
		echo GetRound();
		break;
	
	case "join":
		if (JoinGame($_GET["name"]))
		{
			echo "true";
		}
		else
		{
			echo "false";
		}
		break;
		
	case "start":
		if (Start())
		{
			echo "true";
		}
		else
		{
			echo "false";
		}
		break;
		
	case "set":
		if (Set($_GET["x"], $_GET["y"], $_GET["flag"]))
		{
			echo "true";
		}
		else
		{
			echo "false";
		}
		break;	
	
	case "over":
		if (Over($_GET["winner"]))
		{
			echo "true";
		}
		else
		{
			echo "false";
		}
		break;
	
	// 重置游戏状态
	case "reset":

		if (
			Over("white") &&// 清空玩家列表
			ResetState()	// 重置游戏状态
		)
		{
			echo "true";
		}
		else
		{
			echo "false";
		}
		
		break;
	
	
	// 调试选项
	case "debug":
	
		Over("black");
		JoinGame("TesterA");
	
		break;
	
	}


	/*
	 *	functions
	*/

	function GetUserNum()
	{
		return mysqli_num_rows(mysqli_query($GLOBALS["g_dbConnect"],"select * from {$GLOBALS["g_dbTablePrefix"]}user"));
	}
	
	function GetMap()
	{
		$res = mysqli_query($GLOBALS["g_dbConnect"],"select * from {$GLOBALS["g_dbTablePrefix"]}map");
		
		if (empty($res))
		{
			return "";
		}
		
		$rownum = mysqli_num_rows($res);
		$content = "";
		for($i = 0; $i < $rownum; $i++)
		{
			$row = mysqli_fetch_array($res);
			$content .= $row["x"]." ".$row["y"]." ".$row["flag"]." ";
		}
		return $content;
	}
	
	function GetState()
	{
		$res = mysqli_query($GLOBALS["g_dbConnect"],"select state from {$GLOBALS["g_dbTablePrefix"]}state");
		
		if (empty($res))
		{
			return "waitting";
		}
		
		$state = mysqli_fetch_array($res);
		
		switch ($state["state"])
		{
		case 0: return "waitting";
		case 1: return "gaming";
		case 2: return "white_win";
		case 3: return "black_win";
		}
	}
	
	function GetUserName()
	{
		$res = mysqli_query($GLOBALS["g_dbConnect"],"select * from {$GLOBALS["g_dbTablePrefix"]}user");
		
		if (empty($res))
		{
			return "";
		}
		
		$rownum = mysqli_num_rows($res);
		$content = "";
		for($i = 0; $i < $rownum; $i++)
		{
			$row = mysqli_fetch_array($res);
			$content .= $row["name"]."\n";
		}
		return $content;
	}
	
	function GetRound()
	{
		$res = mysqli_query($GLOBALS["g_dbConnect"],"select * from {$GLOBALS["g_dbTablePrefix"]}map order by id desc");
		
		// 第一个子，白色先走
		if (empty($res))
		{
			return "white";
		}
		
		// 获取最后一个落子
		$row = mysqli_fetch_array($res);
		
		// 最后是白色走的，那么当前是黑方的回合
		if($row["flag"] == true)
		{
			return "black";
		}
		// 如果正好相反，则为白方的回合
		else
		{
			return "white";
		}
	}
	
	function JoinGame($name)
	{
		$user_num = GetUserNum();
		
		if (empty($name))
		{
			return false;
		}
		
		// 只能在未满两人时加入
		if ($user_num < 2)
		{
			$flag = "true";
			
			// 已有一个玩家在房间，设为黑子
			if($user_num)
			{
				$flag = "false";
			}
			
			// 第一个到达此房间，重设游戏状态为等待中
			else
			{
				if(!ResetState())
				{
					return false;
				}
			}
			
			if (mysqli_query_array("insert into {$GLOBALS["g_dbTablePrefix"]}user (name, flag) values ('{$name}', {$flag})"))
			{
				return true;
			}
		}
		
		return false;
	}

	function Start()
	{
		// 人数不到无法开始
		if (GetUserNum() < 2)
		{
			return false;
		}
		
		if (mysqli_query_array(array("truncate table {$GLOBALS["g_dbTablePrefix"]}state", "insert into {$GLOBALS["g_dbTablePrefix"]}state (state) values (1)")))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	// $flag 为 "white" 或 "black"
	function Set($x, $y, $flag)
	{
		$flag_bool = "true";
		if($flag == "black")
		{
			$flag_bool = "false";
		}
		
		if (mysqli_query_array("insert into {$GLOBALS["g_dbTablePrefix"]}map (x, y, flag) values ({$x}, {$y}, {$flag_bool})"))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	// $winner_flag 胜利者，只能为 "white" 或 "black"
	function Over($winner_flag)
	{
		$flag = 2;
		if($winner_flag == "black")
		{
			$flag = 3;
		}
		
		if (mysqli_query_array(array(
			"truncate table {$GLOBALS["g_dbTablePrefix"]}user",
			"truncate table {$GLOBALS["g_dbTablePrefix"]}map",
			"truncate table {$GLOBALS["g_dbTablePrefix"]}state",
			"insert into {$GLOBALS["g_dbTablePrefix"]}state (state) values ({$flag})"
		)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	// 重置游戏状态
	function ResetState()
	{
		if (mysqli_query_array(array("truncate table {$GLOBALS["g_dbTablePrefix"]}state", "insert into {$GLOBALS["g_dbTablePrefix"]}state (state) values (0)")))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

?>
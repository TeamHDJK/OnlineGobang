<?php
	
	require_once("database.php");
	
	echo "<br/>";
	echo "huidong 2021-8-30<br/>";
	echo "开始安装 Online Gobang Web Server<br/>";

	if (
		mysqli_query($g_dbConnect,"
			/*
			 * 用户
			 * flag 为 true 表示白，false 表示黑
			*/
			create table {$g_dbTablePrefix}user (
				id int NOT NULL auto_increment,
				name text,
				flag bool,
				PRIMARY KEY (id)
			);
		") &&
		mysqli_query($g_dbConnect,"
			/*
			 * 棋盘，包含棋子的坐标和占有方
			 * flag 为 true 表示白，false 表示黑
			*/
			create table {$g_dbTablePrefix}map (
				id int NOT NULL auto_increment,
				x int,
				y int,
				flag bool,
				PRIMARY KEY (id)
			);
		") &&
		mysqli_query($g_dbConnect,"
			/*
			 * 游戏状态
			 * 0 未开始
			 * 1 进行中
			 * 2 白胜
			 * 3 黑胜
			*/
			create table {$g_dbTablePrefix}state (
				id int NOT NULL auto_increment,
				state int,
				PRIMARY KEY (id)
			);
		")
	)
	{
		echo "<br/>安装成功";
	}
	else
	{
		die("<br/>安装失败，详细信息：".mysqli_error($g_dbConnect));
	}

?>
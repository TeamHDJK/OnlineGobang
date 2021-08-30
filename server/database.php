<?php
	
	$g_dbServerName = "localhost";
	$g_dbUserName = "root";
	$g_dbPassword = "";
	$g_dbName = "OnlineGobang";
	$g_dbTablePrefix = "OnlineGobang_";	// 表名前缀
	
	// 连接数据库
	$g_dbConnect = mysqli_connect($g_dbServerName, $g_dbUserName, $g_dbPassword, $g_dbName);
	if (!$g_dbConnect) {
		echo "连接失败: " . mysqli_connect_error();
	}

	// sql 批量查询，返回是否查询成功
	// 可传入查询字符串数组或单个查询字符串
	// 只适用于无返回值的查询
	function mysqli_query_array($sql_array)
	{
		global $g_dbConnect;
		if(!is_array($sql_array))
		{
			$sql_array = array($sql_array);
		}
		foreach($sql_array as $value)
		{
			if(!mysqli_query($g_dbConnect, $value))
			{
				echo mysqli_error($g_dbConnect);
				return false;
			}
		}
		return true;
	}

?>
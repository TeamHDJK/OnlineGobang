<?php
	
	$g_dbServerName = "localhost";
	$g_dbUserName = "root";
	$g_dbPassword = "";
	$g_dbName = "OnlineGobang";
	$g_dbTablePrefix = "OnlineGobang_";	// ����ǰ׺
	
	// �������ݿ�
	$g_dbConnect = mysqli_connect($g_dbServerName, $g_dbUserName, $g_dbPassword, $g_dbName);
	if (!$g_dbConnect) {
		echo "����ʧ��: " . mysqli_connect_error();
	}

	// sql ������ѯ�������Ƿ��ѯ�ɹ�
	// �ɴ����ѯ�ַ�������򵥸���ѯ�ַ���
	// ֻ�������޷���ֵ�Ĳ�ѯ
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
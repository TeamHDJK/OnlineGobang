<?php
	
	require_once("database.php");
	
	echo "<br/>";
	echo "huidong 2021-8-30<br/>";
	echo "��ʼ��װ Online Gobang Web Server<br/>";

	if (
		mysqli_query($g_dbConnect,"
			/*
			 * �û�
			 * flag Ϊ true ��ʾ�ף�false ��ʾ��
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
			 * ���̣��������ӵ������ռ�з�
			 * flag Ϊ true ��ʾ�ף�false ��ʾ��
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
			 * ��Ϸ״̬
			 * 0 δ��ʼ
			 * 1 ������
			 * 2 ��ʤ
			 * 3 ��ʤ
			*/
			create table {$g_dbTablePrefix}state (
				id int NOT NULL auto_increment,
				state int,
				PRIMARY KEY (id)
			);
		")
	)
	{
		echo "<br/>��װ�ɹ�";
	}
	else
	{
		die("<br/>��װʧ�ܣ���ϸ��Ϣ��".mysqli_error($g_dbConnect));
	}

?>
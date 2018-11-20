<script language='javascript'>
	window.setTimeout('window.location.reload()',1000); </script>
	<?php $connect = mysql_connect("localhost", "root", "root") or die("Connect Fail: " . mysql_error());
	echo "Connecting";
	mysql_select_db("project") or die("Select DB Fail!");
	$query = "select * from project3 order by time desc limit 1";
	$result = mysql_query($query) or die("Query Fail: " . mysql_error());
	echo "<table>\n";
	echo "<tr><td align=center><font style=font-size:18pt>Time</td><td><font style=font-size:18pt>TEM</td><td><font style=font-size:18pt>ILL</td><td><font style=font-size:18pt>FAN</td><td><font style=font-size:18pt>LED</td><td><font style=font-size:18pt>SMODE</td></tr>";
	while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
		echo "\t<tr>\n";
		foreach ($line as $col_value) {
			if ($col_value == "3"){
			echo "\t\t<td align=center>Full on</td>\n";
			}
			else if ($col_value == "2"){
			echo "\t\t<td align=center>SleeP on</td>\n";
			}
			else if ($col_value == "9"){
			echo "\t\t<td align=center>Light</td>\n";
			}
			else if ($col_value == "8"){
			echo "\t\t<td align=center>Dark</td>\n";
			}
			else if ($col_value == "1"){
			echo "\t\t<td align=center>on</td>\n";
			}
			else if ($col_value == "0"){
			echo "\t\t<td align=center>off</td>\n";
			}
			else{
			echo "\t\t<td align=center>$col_value</td>\n";
			}
		}
		echo "\t</tr>\n";
	} echo "</table>\n"; mysql_free_result($result); mysql_close($connect);
	?>


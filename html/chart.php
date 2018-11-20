<?php
 

ini_set('display_errors', 'On');
error_reporting(E_ALL|E_STRICT);
 

$mysql_host = 'localhost';
$mysql_user = 'root';
$mysql_password = 'root';
$mysql_db = 'project';
$conn = @mysql_connect($mysql_host, $mysql_user, $mysql_password);
$dbconn = mysql_select_db($mysql_db, $conn);
mysql_query("set names utf8");


$sql="
select * from (

SELECT DATE_FORMAT( time,  '%m-%d %H:%i' )  mdh , COUNT( * ) cnt, SUM( temp ) , 
round(SUM( temp ) / COUNT( * ),1)  atemper,
round(SUM( light ) / COUNT( * ),1)  alight,
round(SUM( fanon ) / COUNT( * ),1)  afanon,
round(SUM( ledon ) / COUNT( * ),1)  aledon,
round(SUM( mode ) / COUNT( * ),1)  amode
FROM  `project3`
GROUP BY DATE_FORMAT( time,  '%Y%m%d%H%i' )
order by time desc

limit 24
) t_a
order by t_a.mdh
";

$result = mysql_query($sql) ;

$str_mdh="";
$str_atemper="";
$str_alight="";
$str_afanon="";
$str_aledon="";
$str_amode="";
 
while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
 $str_mdh .="'".$row['mdh']."',";
 $str_atemper .="".$row['atemper'].",";
 $str_alight .="".$row['alight'].",";
 $str_afanon .="".$row['afanon'].",";
 $str_aledon .="".$row['aledon'].",";
 $str_amode .="".$row['amode'].",";
}

$str_mdh= substr($str_mdh,0,-1);
$str_atemper= substr($str_atemper,0,-1);
$str_alight= substr($str_alight,0,-1);
$str_afanon= substr($str_afanon,0,-1);
$str_aledon= substr($str_aledon,0,-1);
$str_amode= substr($str_amode,0,-1);

 
?><!DOCTYPE HTML>
<html>
<link rel="stylesheet" href="css/style.css">
<head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
        <title>House Monitor</title>
        <script type="text/javascript" src="https://ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js"></script>
        <style type="text/css">${demo.css}</style>
        <script type="text/javascript">
 
$(function () {
    $('#temp').highcharts({
        chart: {
            type: 'line'
        },
        title: {
            text: 'Temperature'
        },
        subtitle: {
            text: 'IOT System Design Group E'
        },
        xAxis: {
            categories: [<?php echo $str_mdh?>]
        },
        yAxis: {
            title: {
                text: 'Temperature (C)'
            }
        },
        plotOptions: {
            line: {
                dataLabels: {
                    enabled: true
                },
                enableMouseTracking: false
            }
        },
        series: [{
            name: 'HOUSE',
            data: [<?php echo $str_atemper?>]
        }]
      });

    $('#fanon').highcharts({
        chart: {
            type: 'line'
   },
        title: {
            text: 'Fan-ratio'
        },
        xAxis: {
            categories: [<?php echo $str_mdh?>]
        },
        yAxis: {
            title: {
                text: 'fan-ratio (1/60sec)'
            }
        },
        plotOptions: {
            line: {
                dataLabels: {
                    enabled: true
                },
                enableMouseTracking: false
            }
        },
        series: [{
            name: 'HOUSE',
            data: [<?php echo $str_afanon?>]
        }]
    });

    $('#ledon').highcharts({
        chart: {
            type: 'line'
   },
        title: {
            text: 'LED-ratio'
        },
        xAxis: {
            categories: [<?php echo $str_mdh?>]
        },
        yAxis: {
            title: {
                text: 'led-ratio (1/60sec)'
            }
        },
        plotOptions: {
            line: {
                dataLabels: {
                    enabled: true
                },
                enableMouseTracking: false
            }
        },
        series: [{
            name: 'HOUSE',
            data: [<?php echo $str_aledon?>]
        }]
    });

});
        </script>
</head>
<body>
        <script src="https://code.highcharts.com/highcharts.js"></script>
        <script src="https://code.highcharts.com/modules/exporting.js"></script>
        <div id="temp" style="width: 900px; height: 440px; margin: 30px auto"></div>
        <div id="fanon" style="width: 900px; height: 440px; margin: 30px auto"></div>
        <div id="ledon" style="width: 900px; height: 440px; margin: 30px auto"></div>
</body>
</html>



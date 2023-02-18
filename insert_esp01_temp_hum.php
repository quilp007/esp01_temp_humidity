   1 <?php
   2 echo "file name: insert_t1.php<br>";
   3
   4 $servername = "localhost";
   5 $username = "testuser";
   6 $password = "1234";
   7 $dbname = "nodemcu";
   8
   9 $conn = mysqli_connect($servername, $username, $password, $dbname);
  10
  11 $CD_LOC         = $_GET['CD_LOC'];
  12 $QT_TEMP        = $_GET['QT_TEMP'];
  13 $QT_HUMI        = $_GET['QT_HUMI'];
  14 $QT_PRESSURE    = $_GET['QT_PRESSURE'];
  15 $YN_ABNORMAL    = $_GET['YN_ABNORMAL'];
  16 $DC_RMK         = $_GET['DC_RMK'];
  17 $DT_REG         = $_GET['DT_REG'];
  18 $TM_REG         = $_GET['TM_REG'];
  19
  20 $date = date('Ymd');
  21 echo "$date<br>";
  22 $datetime = date('Y-m-d H:i:s');
  23 echo "$datetime<br>";
  24
  25
  26 echo "$CD_LOC      <br>";
  27 echo "$QT_TEMP     <br>";
  28 echo "$QT_HUMI     <br>";
  29 echo "$QT_PRESSURE <br>";
  30 echo "$YN_ABNORMAL <br>";
  31 echo "$DC_RMK      <br>";
  32 echo "$DT_REG      <br>";
  33 echo "$TM_REG      <br>";
  34
  35 //$sql = "INSERT INTO nodemcu.PRD_TEMP_HST(CD_LOC, QT_TEMP, QT_HUMI, QT_PRESSURE, YN_ABNORMAL, DC_RMK, DT_REG, TM_REG) VALUES($CD_LOC, $QT_TEMP, $QT_HUMI, $QT_PRESSURE, $YN_ABNORMAL, $DC_RMK, $DT_REG, $TM_REG)";
  36
  37 //$sql = "insert into nodemcu.PRD_TEMP_HST(CD_LOC, QT_TEMP, QT_HUMI, QT_PRESSURE, YN_ABNORMAL, DC_RMK, DT_REG, TM_REG) values(1,2,3,4,5,6, '$DT_REG', '$TM_REG')";
  38
  39 $sql = "insert into nodemcu.PRD_TEMP_HST(CD_LOC, QT_TEMP, QT_HUMI, QT_PRESSURE, YN_ABNORMAL, DC_RMK, DT_REG, TM_REG) values(1,$QT_TEMP, $QT_HUMI, 4, 5, '$DC_RMK', '$date', '$datetime')";
  40
  41 mysqli_query($conn, $sql);
  42 mysqli_close($conn);
  43 ?>

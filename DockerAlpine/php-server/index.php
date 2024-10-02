<?php
$temp = filter_input(INPUT_GET, 'temp', FILTER_SANITIZE_NUMBER_FLOAT);
$humid = filter_input(INPUT_GET, 'humid', FILTER_SANITIZE_NUMBER_FLOAT);
if (is_null($temp) || is_null($humid)) {
     //Gravar log de erros
     die("Dados inv�lidos");
}
$servername = "192.168.1.198";
$username = "root";
$password = "password";
$dbname = "iot";
$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {

     //Gravar log de erros
     die("N�o foi poss�vel estabelecer conex�o com o BD: " . $conn->connect_error);
}
$sql = "INSERT INTO sensor (wea_temp, wea_humid) VALUES ($temp,$humid)";

if (!$conn->query($sql)) {
     //Gravar log de erros
     die("Erro na grava��o dos dados no BD");
}
$conn->close();
?>
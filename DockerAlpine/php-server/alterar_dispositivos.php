<?php
header('Content-Type: application/json; charset=utf-8');
$servername = "192.168.1.198";
$username = "root";
$password = "password";
$dbname = "iot";
$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    // Gravar log de erros
    echo json_encode(['error' => 'Não foi possível estabelecer conexão com o BD: ']);
    exit();
}

$iddisp = $_POST['iddisp'];
$name = $_POST['name'];
$pin = $_POST['pin'];
$imgon = $_POST['imgon'];
$imgoff = $_POST['imgoff'];
$widthimg = $_POST['widthimg'];
$heightimg = $_POST['heightimg'];
$paddingimg = $_POST['paddingimg'];
$identifier = $_POST['identifier'];

$sql = "UPDATE dispositivo SET pin='$pin', identifier='$identifier', name='$name', imgon='$imgon', imgoff='$imgoff', widthimg='$widthimg', heightimg='$heightimg', paddingimg='$paddingimg' WHERE id=$iddisp";

if ($conn->query($sql) === TRUE) {
  echo "Dispositivo atualizado com sucesso";
} else {
  echo "Erro: " . $sql . "<br>" . $conn->error;
}

$conn->close();
?>
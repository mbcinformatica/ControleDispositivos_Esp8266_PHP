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

$name = $_POST['name'];
$pin = $_POST['pin'];
$imgon = $_POST['imgon'];
$imgoff = $_POST['imgoff'];
$widthimg = $_POST['widthimg'];
$heightimg = $_POST['heightimg'];
$paddingimg = $_POST['paddingimg'];
$identifier = $_POST['identifier'];

$sql = "INSERT INTO dispositivo (name, pin, imgon, imgoff, widthimg, heightimg, paddingimg, identifier)
VALUES ('$name', '$pin', '$imgon', '$imgoff', '$widthimg', '$heightimg', '$paddingimg', '$identifier')";

if ($conn->query($sql) === TRUE) {
  echo "Novo dispositivo cadastrado com sucesso";
} else {
  echo "Erro: " . $sql . "<br>" . $conn->error;
}

$conn->close();
?>

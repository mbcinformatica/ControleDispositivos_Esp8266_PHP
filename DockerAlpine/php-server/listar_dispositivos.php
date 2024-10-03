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

$sql = "SELECT iddisp, name, pin, imgon, imgoff, widthimg, heightimg, paddingimg, identifier FROM dispositivo";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
  while($row = $result->fetch_assoc()) {
    echo "<div>";
    echo "<h3>Dispositivo " . $row["iddisp"] . "</h3>";
    echo "<p>Nome: " . $row["name"] . "</p>";
    echo "<p>Pino: " . $row["pin"] . "</p>";
    echo "<p>Imagem Ligado: " . $row["imgon"] . "</p>";
    echo "<p>Imagem Desligado: " . $row["imgoff"] . "</p>";
    echo "<p>Largura da Imagem: " . $row["widthimg"] . "</p>";
    echo "<p>Altura da Imagem: " . $row["heightimg"] . "</p>";
    echo "<p>Preenchimento da Imagem: " . $row["paddingimg"] . "</p>";
    echo "<p>Identificador: " . $row["identifier"] . "</p>";

    echo "<form action='alterar_dispositivos.php' method='post'>";
    echo "<input type='hidden' name='iddisp' value='" . $row["iddisp"] . "'>";
    echo "<input type='submit' value='Alterar'>";
    echo "</form>";
    echo "</div>";
  }
} else {
  echo "0 resultados";
}

$conn->close();
?>

<?php
header('Content-Type: application/json; charset=utf-8');
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: GET, POST, OPTIONS");
header("Access-Control-Allow-Headers: Content-Type");

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

$action = isset($_GET['action']) ? $_GET['action'] : 'list';

switch ($action) {
    case 'inserir-dispositivo':
        $name = filter_input(INPUT_GET, 'name', FILTER_SANITIZE_STRING);
        $pin = filter_input(INPUT_GET, 'pin', FILTER_SANITIZE_NUMBER_INT);
        $imgon = filter_input(INPUT_GET, 'imgon', FILTER_SANITIZE_STRING);
        $imgoff = filter_input(INPUT_GET, 'imgoff', FILTER_SANITIZE_STRING);
        $widthimg = filter_input(INPUT_GET, 'widthimg', FILTER_SANITIZE_NUMBER_INT);
        $heightimg = filter_input(INPUT_GET, 'heightimg', FILTER_SANITIZE_NUMBER_INT);
        $paddingimg = filter_input(INPUT_GET, 'paddingimg', FILTER_SANITIZE_STRING);
        $identifier = filter_input(INPUT_GET, 'identifier', FILTER_SANITIZE_STRING);
        error_log("Name: $name, Pin: $pin, ImgOn: $imgon, ImgOff: $imgoff, Width: $widthimg, Height: $heightimg, Padding: $paddingimg, Identifier: $identifier");

        // Verifica se os valores não são nulos
        if (is_null($name) || is_null($identifier)) {
            echo json_encode([
                'error' => 'Os parâmetros name, identifier e state são obrigatórios.',
                'post_data' => $_POST,
                'get_data' => $_GET
            ]);
            return;
        }

        // Verifica se o dispositivo já existe
        $sql = "SELECT COUNT(*) FROM dispositivo WHERE identifier = ?";
        $stmt = $conn->prepare($sql);
        $stmt->bind_param("s", $identifier);
        $stmt->execute();
        $stmt->bind_result($count);
        $stmt->fetch();
        $stmt->close();

        if ($count > 0) {
            // Atualiza o dispositivo existente
            $sql = "UPDATE dispositivo SET pin = ?, imgon = ?, imgoff = ?, widthimg = ?, heightimg = ?, paddingimg = ? WHERE identifier = ?";
            $stmt = $conn->prepare($sql);
            $stmt->bind_param("issiiss",  $pin, $imgon, $imgoff, $widthimg, $heightimg, $paddingimg, $identifier);
            if ($stmt->execute()) {
                echo json_encode(['message' => 'Dispositivo atualizado com sucesso!']);
            } else {
                echo json_encode([
                    'error' => 'Erro ao atualizar dispositivo: ' . $stmt->error,
                    'post_data' => $_POST // Retornando o corpo do POST
                ]);
            }
            $stmt->close();
        } else {
            // Insere o dispositivo
            $sql = "INSERT INTO dispositivo (name, pin, imgon, imgoff, widthimg, heightimg, paddingimg, identifier) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
            $stmt = $conn->prepare($sql);
            $stmt->bind_param("sissiiss", $name, $pin, $imgon, $imgoff, $widthimg, $heightimg, $paddingimg, $identifier);
            if ($stmt->execute()) {
                echo json_encode(['message' => 'Dispositivo inserido com sucesso!']);
            } else {
                echo json_encode([
                    'error' => 'Erro ao inserir dispositivo: ' . $stmt->error,
                    'post_data' => $_POST // Retornando o corpo do POST
                ]);
            }
            $stmt->close();
        }
        break;
    case 'listar-dispositivos':
        $sql = "SELECT iddisp, name, pin, imgon, imgoff, widthimg, heightimg, paddingimg, identifier FROM dispositivo";
        $result = $conn->query($sql);
        $dispositivos = [];
        while ($row = $result->fetch_assoc()) {
            $dispositivos[] = $row;
        }
        echo json_encode($dispositivos);
        break;
    case 'obter-dispositivo':
        $iddisp = filter_input(INPUT_GET, 'iddisp', FILTER_SANITIZE_NUMBER_INT);
        if ($iddisp) {
            $sql = "SELECT * FROM dispositivo WHERE iddisp = ?";
            $stmt = $conn->prepare($sql);
            $stmt->bind_param("i", $iddisp);
            $stmt->execute();
            $result = $stmt->get_result();
            $dispositivo = $result->fetch_assoc();
            echo json_encode($dispositivo);
            $stmt->close();
        } else {
            echo json_encode(['error' => 'ID do dispositivo não fornecido']);
        }
        break;
    case 'inserir-historico-dispositivo':
        $identifier = filter_input(INPUT_GET, 'identifier', FILTER_SANITIZE_STRING);
        $state = filter_input(INPUT_GET, 'state', FILTER_SANITIZE_NUMBER_INT);
        error_log("Identifier: $identifier, state: $state");

        // Verifica se os valores não são nulos
        if (is_null($identifier) || is_null($state)) {
            echo json_encode([
                'error' => 'Os parâmetros name, identifier e state são obrigatórios.',
                'post_data' => $_POST,
                'get_data' => $_GET
            ]);
            return;
        }

        // Obtém o idsensor da tabela sensor
        $sql = "SELECT iddisp FROM dispositivo WHERE identifier = ?";
        $stmt = $conn->prepare($sql);
        $stmt->bind_param("s", $identifier);
        $stmt->execute();
        $stmt->bind_result($iddisp);
        $stmt->fetch();
        $stmt->close();

        if (is_null($iddisp)) {
            echo json_encode(['error' => 'Sensor não encontrado.']);
            return;
        }

        // Insere o histórico do sensor
        $sql = "INSERT INTO historico_dispositivo (iddisp, state) VALUES (?, ?)";
        $stmt = $conn->prepare($sql);
        $stmt->bind_param("ii", $iddisp, $state);

        if ($stmt->execute()) {
            echo json_encode(['message' => 'Dados Inseridos com Sucesso!']);
        } else {
            echo json_encode([
                'error' => 'Erro ao Inserir ou Atualizar Aados: ' . $stmt->error,
                'post_data' => $_POST // Retornando o corpo do POST
            ]);
        }
        $stmt->close();
        break;
    case 'inserir-sensor':
        $name = filter_input(INPUT_GET, 'name', FILTER_SANITIZE_STRING);
        $unit = filter_input(INPUT_GET, 'unit', FILTER_SANITIZE_STRING);
        $imgon = filter_input(INPUT_GET, 'imgon', FILTER_SANITIZE_STRING);
        $imgoff = filter_input(INPUT_GET, 'imgoff', FILTER_SANITIZE_STRING);
        $identifier = filter_input(INPUT_GET, 'identifier', FILTER_SANITIZE_STRING);
        error_log("Name: $name, Unit: $unit, ImgOn: $imgon, ImgOff: $imgoff, Identifier: $identifier");

        // Verifica se os valores não são nulos
        if (is_null($name) || is_null($identifier)) {
            echo json_encode([
                'error' => 'Os parâmetros name e identifier são obrigatórios.',
                'post_data' => $_POST,
                'get_data' => $_GET
            ]);
            return;
        }

        // Verifica se o dispositivo já existe
        $sql = "SELECT COUNT(*) FROM sensor WHERE identifier = ?";
        $stmt = $conn->prepare($sql);
        $stmt->bind_param("s", $identifier);
        $stmt->execute();
        $stmt->bind_result($count);
        $stmt->fetch();
        $stmt->close();

        if ($count > 0) {
            // Atualiza o estado do dispositivo existente

        } else {
            // Insere o dispositivo
            $sql = "INSERT INTO sensor (name, unit, imgon, imgoff, identifier) VALUES (?, ?, ?, ?, ?)";
            $stmt = $conn->prepare($sql);
            $stmt->bind_param("sssss", $name, $unit, $imgon, $imgoff, $identifier);


            if ($stmt->execute()) {
                echo json_encode(['message' => 'Operação realizada com sucesso!']);
            } else {
                echo json_encode([
                    'error' => 'Erro ao realizar a operação: ' . $stmt->error,
                    'post_data' => $_POST // Retornando o corpo do POST
                ]);
            }
            $stmt->close();
        }
        break;
    case 'inserir-historico-sensor':
        $identifier = filter_input(INPUT_GET, 'identifier', FILTER_SANITIZE_STRING);
        $valor = filter_input(INPUT_GET, 'valor', FILTER_SANITIZE_STRING);
        error_log("identifier: $identifier, valor: $valor");

        // Verifica se os valores não são nulos
        if (is_null($identifier) || is_null($valor)) {
            echo json_encode([
                'error' => 'Os parâmetros identifier e valor são obrigatórios.',
                'post_data' => $_POST,
                'get_data' => $_GET
            ]);
            return;
        }

        // Obtém o idsensor da tabela sensor
        $sql = "SELECT idsensor FROM sensor WHERE identifier = ?";
        $stmt = $conn->prepare($sql);
        $stmt->bind_param("s", $identifier);
        $stmt->execute();
        $stmt->bind_result($idsensor);
        $stmt->fetch();
        $stmt->close();

        if (is_null($idsensor)) {
            echo json_encode(['error' => 'Sensor não encontrado.']);
            return;
        }

        // Insere o histórico do sensor
        $sql = "INSERT INTO historico_sensor (idsensor, valor) VALUES (?, ?)";
        $stmt = $conn->prepare($sql);
        $stmt->bind_param("is", $idsensor, $valor);

        if ($stmt->execute()) {
            echo json_encode(['message' => 'Dados inseridos com sucesso!']);
        } else {
            echo json_encode([
                'error' => 'Erro ao inserir dados: ' . $stmt->error,
                'post_data' => $_POST // Retornando o corpo do POST
            ]);
        }

        $stmt->close();
        break;
    default:
        echo json_encode(['error' => 'Ação não reconhecida']);
        break;
}
?>
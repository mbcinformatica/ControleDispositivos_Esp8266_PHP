#include <ArduinoJson.h> // Inclui a biblioteca ArduinoJson para manipulação de JSON

// Estrutura para armazenar informações dos dispositivos
struct Device
{
    int pin;            // Pino do Arduino ao qual o dispositivo está conectado
    int state;          // Estado atual do dispositivo (ligado/desligado)
    const char *name;   // Nome do dispositivo
    const char *imgon;  // Caminho da imagem quando o dispositivo está ligado
    const char *imgoff; // Caminho da imagem quando o dispositivo está desligado
    int widthimg;       // Largura da imagem
    int heightimg;      // Altura da imagem
    String paddingimg;  // Preenchimento da imagem
    String identifier;  // Identificador do dispositivo
};

const int ledIndex = 3;           // Índice do LED (não utilizado no código fornecido)
bool lastButtonState = false;     // Estado anterior do botão (não utilizado no código fornecido)
bool currentButtonState = false;  // Estado atual do botão (não utilizado no código fornecido)
const int nArrayDispositivos = 6; // Número de dispositivos

// Declaração das funções
void setupDevices();
void inserirDispositivo(Device *device);
void inserirHistoricoDispositivo(Device *device);

// Lista de dispositivos com suas propriedades
Device deviceList[nArrayDispositivos] = {
    //{Pino-Arduino, Estado-Pino, "Nome Dispositivo", "Imagem Disp. On", "Imagem Disp. Off", Largura, Altura, "Preenchimento"}
    {12, false, "Lâmpada Cozinha", "img/Lampada_On.png", "img/Lampada_Off.png", 80, 80, "9px 0px", "lampada_cozinha"},
    {13, false, "Lâmpada Sala", "img/Lampada_On.png", "img/Lampada_Off.png", 80, 80, "9px 0px", "lampada_sala"},
    {14, false, "Lâmpada Garagem", "img/Lampada_On.png", "img/Lampada_Off.png", 80, 80, "9px 0px", "lampada_garagem"},
    {15, false, "Lâmpada Suite Casal", "img/Lampada_On.png", "img/Lampada_Off.png", 80, 80, "9px 0px", "lampada_suite_casal"},
    {16, false, "Aquecedor", "img/Aquec_On.png", "img/Aquec_Off.png", 70, 70, "9px 0px", "aquecedor"},
    {2, false, "Ventilador", "img/Ventilador_On.gif", "img/Ventilador_Off.gif", 80, 80, "9px 0px", "ventilador"}};

// Função para obter os dados dos dispositivos em formato JSON
DynamicJsonDocument getDeviceData()
{
    DynamicJsonDocument jsonBuffer(1024);                                  // Cria um buffer JSON
    JsonArray dispositivos = jsonBuffer.createNestedArray("dispositivos"); // Cria um array JSON para os dispositivos

    for (int i = 0; i < nArrayDispositivos; i++)
    {
        JsonObject dispositivo = dispositivos.createNestedObject(); // Cria um objeto JSON para cada dispositivo
        dispositivo["pino"] = deviceList[i].pin;                    // Adiciona o pino do dispositivo
        dispositivo["estado"] = deviceList[i].state;                // Adiciona o estado do dispositivo
        dispositivo["nome"] = deviceList[i].name;                   // Adiciona o nome do dispositivo
        dispositivo["imageon"] = deviceList[i].imgon;               // Adiciona a imagem quando o dispositivo está ligado
        dispositivo["imageoff"] = deviceList[i].imgoff;             // Adiciona a imagem quando o dispositivo está desligado
        dispositivo["largura"] = deviceList[i].widthimg;            // Adiciona a largura da imagem
        dispositivo["altura"] = deviceList[i].heightimg;            // Adiciona a altura da imagem
        dispositivo["preenchimento"] = deviceList[i].paddingimg;    // Adiciona o preenchimento da imagem
    }
    return jsonBuffer; // Retorna o buffer JSON
}

// Função para codificar uma string em URL
String urlencode(const String &str)
{
    String encoded = "";
    for (size_t i = 0; i < str.length(); i++)
    {
        char c = str.charAt(i);
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            // Caracteres seguros
            encoded += c;
        }
        else if (c < 0)
        {
            // Codifica caracteres UTF-8
            encoded += "%";
            encoded += String((unsigned char)c, HEX);
        }
        else
        {
            // Para caracteres especiais
            encoded += "%";
            encoded += String((unsigned char)c, HEX);
        }
    }
    return encoded;
}

// Função para alternar o estado de um dispositivo
void toggleDeviceState(Device *device)
{
    device->state = !device->state;           // Alterna o estado do dispositivo
    digitalWrite(device->pin, device->state); // Escreve o novo estado no pino do dispositivo
}

// Função para configurar os dispositivos
void setupDevices()
{
    for (int i = 0; i < nArrayDispositivos; i++)
    {
        pinMode(deviceList[i].pin, OUTPUT);                   // Define o pino do dispositivo como saída
        digitalWrite(deviceList[i].pin, deviceList[i].state); // Define o estado inicial do dispositivo
        inserirDispositivo(&deviceList[i]);                   // Insere o dispositivo na lista
    }
}

void inserirDispositivo(Device *device)
{
    
    HTTPClient http; // Cria um objeto HTTPClient para realizar a requisição HTTP
    // Monta a URL para inserir o dispositivo, incluindo todos os parâmetros necessários
    String urlDispositivo = String(site_url) + "?action=inserir-dispositivo&name=" + String(urlencode(device->name)) +
    "&pin=" + (device->pin) + "&imgon=" + String(urlencode(device->imgon)) + "&imgoff=" + String(urlencode(device->imgoff)) +
    "&widthimg=" + (device->widthimg) + "&heightimg=" + (device->heightimg) + "&paddingimg=" + String(urlencode(device->paddingimg)) + "&identifier=" + String(device->identifier);

    http.begin(wifiClient, urlDispositivo);             // Inicia a conexão HTTP com a URL especificada
    http.addHeader("Content-Type", "application/json"); // Adiciona o cabeçalho de conteúdo JSON
    int httpResponseCode = http.GET();                  // Envia a requisição GET e armazena o código de resposta

    // Verifica a resposta do servidor
    if (httpResponseCode > 0)
    {
        String response = http.getString(); // Obtém a resposta do servidor
        Serial.println(httpResponseCode);   // Imprime o código de resposta no console
        Serial.println(response);           // Imprime a resposta do servidor no console
    }
    else
    {
        Serial.print("Erro ao enviar: "); // Imprime uma mensagem de erro no console
        Serial.println(httpResponseCode); // Imprime o código de erro no console
    }

    // Finaliza a conexão HTTP
    http.end();
}

void inserirHistoricoDispositivo(Device *device)
{
    HTTPClient http; // Cria um objeto HTTPClient para realizar a requisição HTTP
    // Monta a URL para inserir o histórico do dispositivo, incluindo o identificador e o estado do dispositivo
    String urlDispositivo = String(site_url) + "?action=inserir-historico-dispositivo&identifier=" + String(device->identifier) + "&state=" + (device->state);

    http.begin(wifiClient, urlDispositivo);             // Inicia a conexão HTTP com a URL especificada
    http.addHeader("Content-Type", "application/json"); // Adiciona o cabeçalho de conteúdo JSON
    int httpResponseCode = http.GET();                  // Envia a requisição GET e armazena o código de resposta

    // Verifica a resposta do servidor
    if (httpResponseCode > 0)
    {
        String response = http.getString(); // Obtém a resposta do servidor
        Serial.println(httpResponseCode);   // Imprime o código de resposta no console
        Serial.println(response);           // Imprime a resposta do servidor no console
    }
    else
    {
        Serial.print("Erro ao enviar: "); // Imprime uma mensagem de erro no console
        Serial.println(httpResponseCode); // Imprime o código de erro no console
    }

    // Finaliza a conexão HTTP
    http.end();
}

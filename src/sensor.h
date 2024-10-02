#include <DHT.h>         // Inclui a biblioteca DHT para sensores de temperatura e umidade
#include <DHT_U.h>       // Inclui a biblioteca DHT_U para sensores de temperatura e umidade
#include <ArduinoJson.h> // Inclui a biblioteca ArduinoJson para manipulação de JSON

#define DHTPIN 5          // Define o pino D1 como o pino de dados do sensor DHT
#define DHTTYPE DHT11     // Define o tipo de sensor DHT como DHT11
#define PinSensorChamas 4 // Define o pino D2 como o pino do sensor de chamas

DHT dht(DHTPIN, DHTTYPE); // Inicializa o sensor DHT com o pino e tipo definidos

// Estrutura para armazenar informações dos sensores
struct DeviceSensor
{
    const char *idtipo;   // Tipo de sensor
    const char *name;     // Nome do sensor
    const char *unit;     // Unidade de medida
    const char *imgon;    // Imagem quando o sensor está ativo
    const char *imgoff;   // Imagem quando o sensor está inativo
    String (*getValue)(); // Ponteiro para função que obtém o valor do sensor
    String identifier;    // Identificador do sensor
};

const int nArraySensor = 3; // Número de sensores

// Declaração das funções
void inserirSensor(DeviceSensor *devicesensor);
void inserirHistoricoSensor(DeviceSensor *devicesensor);

String getTemperatura(); // Função para obter a temperatura
String getHumidity();    // Função para obter a umidade
String getChamas();      // Função para obter o estado do sensor de chamas

// Lista de sensores com suas propriedades
DeviceSensor DeviceSensorList[nArraySensor] = {
    {"temperatura", "Sensor Temperatura", "°C", "img/Termometro_01.png", "", getTemperatura, "sensor_temperatura"},
    {"umidade", "Sensor Umidade", "%", "img/Umidade.png", "", getHumidity, "sensor_umidade"},
    {"chamas", "Sensor Chamas", "", "img/Chamas_On.gif", "img/Chamas_Off.gif", getChamas, "sensor_chamas"}};

// Função para obter a temperatura
String getTemperatura()
{
    float temperatura = dht.readTemperature(); // Lê a temperatura do sensor DHT
    if (isnan(temperatura))
    {
        return "0.00"; // Retorna 0.00 se a leitura for inválida
    }
    return String(temperatura, 2); // Retorna a temperatura com 2 casas decimais
}

// Função para obter a umidade
String getHumidity()
{
    float umidade = dht.readHumidity(); // Lê a umidade do sensor DHT
    if (isnan(umidade))
    {
        return "0"; // Retorna 0 se a leitura for inválida
    }
    return String((int)umidade); // Retorna a umidade sem casas decimais
}

// Função para obter o estado do sensor de chamas
String getChamas()
{
    int chamas = digitalRead(PinSensorChamas);   // Lê o estado do pino do sensor de chamas
    return (chamas == 1) ? "Normal" : "Anormal"; // Retorna "Normal" ou "Anormal"
}

// Função para obter a imagem correspondente ao estado do sensor
const char *getImagem(const char *valor, const char *imgon, const char *imgoff)
{
    if (strcmp(valor, "Normal") == 0)
    {
        return imgoff; // Retorna a imagem de sensor inativo
    }
    else if (strcmp(valor, "Anormal") == 0)
    {
        return imgon; // Retorna a imagem de sensor ativo
    }
    else
    {
        return imgon; // Retorna a imagem de sensor ativo para outros valores
    }
}

// Função para obter os dados dos sensores em formato JSON
DynamicJsonDocument getSensorData()
{
    DynamicJsonDocument jsonBuffer(1024);                      // Cria um buffer JSON
    JsonArray sensor = jsonBuffer.createNestedArray("sensor"); // Cria um array JSON para os sensores

    for (int i = 0; i < nArraySensor; i++)
    {
        JsonObject Sensor = sensor.createNestedObject();                                                      // Cria um objeto JSON para cada sensor
        Sensor["id"] = DeviceSensorList[i].idtipo;                                                            // Adiciona o tipo de sensor
        Sensor["nome"] = DeviceSensorList[i].name;                                                            // Adiciona o nome do sensor
        Sensor["unidade"] = DeviceSensorList[i].unit;                                                         // Adiciona a unidade de medida
        Sensor["valor"] = DeviceSensorList[i].getValue();                                                     // Adiciona o valor do sensor
        Sensor["imagen"] = getImagem(Sensor["valor"], DeviceSensorList[i].imgon, DeviceSensorList[i].imgoff); // Adiciona a imagem correspondente ao estado do sensor
    }
    return jsonBuffer; // Retorna o buffer JSON
}

// Função de configuração dos sensores
void setupSensor()
{
    dht.begin();                     // Inicializa o sensor DHT
    pinMode(PinSensorChamas, INPUT); // Define o pino do sensor de chamas como entrada
    for (int i = 0; i < nArraySensor; i++)
    {
        inserirSensor(&DeviceSensorList[i]); // Insere cada sensor na lista
    }
}

// Função para atualizar o histórico dos sensores
void atualizaHistoricoSensor()
{
    for (int i = 0; i < nArraySensor; i++)
    {
        inserirHistoricoSensor(&DeviceSensorList[i]); // Atualiza o histórico de cada sensor
    }
}

// Função para inserir um sensor
void inserirSensor(DeviceSensor *devicesensor)
{
    HTTPClient http; // Cria um objeto HTTPClient
    String urlSensor = String(site_url) + "?action=inserir-sensor&name=" + String(urlencode(devicesensor->name)) +
                       "&unit=" + String(urlencode(devicesensor->unit)) +
                       "&imgon=" + String(urlencode(devicesensor->imgon)) +
                       "&imgoff=" + String(urlencode(devicesensor->imgoff)) +
                       "&identifier=" + String(devicesensor->identifier); // Cria a URL para inserir o sensor

    http.begin(wifiClient, urlSensor);                  // Inicia a conexão HTTP
    http.addHeader("Content-Type", "application/json"); // Adiciona o cabeçalho de conteúdo JSON
    int httpResponseCode = http.GET();                  // Envia a requisição GET

    // Verifica a resposta do servidor
    if (httpResponseCode > 0)
    {
        String response = http.getString(); // Obtém a resposta do servidor
        Serial.println(httpResponseCode);   // Imprime o código de resposta
        Serial.println(response);           // Imprime a resposta do servidor
        Serial.println(urlSensor);          // Imprime a URL da requisição
    }
    else
    {
        Serial.print("Erro ao enviar: ");
        Serial.println(httpResponseCode); // Imprime o erro de envio
    }

    // Finaliza a conexão
    http.end();
}

void inserirHistoricoSensor(DeviceSensor *devicesensor)
{
    HTTPClient http; // Cria um objeto HTTPClient para realizar a requisição HTTP

    // Monta a URL para inserir o histórico do sensor, incluindo o identificador e o valor do sensor
    String urlSensor = String(site_url) + "?action=inserir-historico-sensor&identifier=" + String(devicesensor->identifier) + "&valor=" + String(urlencode(devicesensor->getValue()));
    Serial.println(urlSensor); // Imprime a URL no console para depuração

    http.begin(wifiClient, urlSensor);                  // Inicia a conexão HTTP com a URL especificada
    http.addHeader("Content-Type", "application/json"); // Adiciona o cabeçalho de conteúdo JSON
    int httpResponseCode = http.GET();                  // Envia a requisição GET e armazena o código de resposta

    // Verifica a resposta do servidor
    if (httpResponseCode > 0)
    {
        String response = http.getString(); // Obtém a resposta do servidor
        Serial.println(httpResponseCode);   // Imprime o código de resposta no console
        Serial.println(response);           // Imprime a resposta do servidor no console
        Serial.println(urlSensor);          // Imprime a URL no console
    }
    else
    {
        Serial.print("Erro ao enviar: "); // Imprime uma mensagem de erro no console
        Serial.println(httpResponseCode); // Imprime o código de erro no console
    }

    // Finaliza a conexão HTTP
    http.end();
}

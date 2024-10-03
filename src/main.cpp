#include <Arduino.h>
#include <FS.h>          // Biblioteca para o sistema de arquivos SPIFFS
#include <ESP8266WiFi.h> // Biblioteca para WiFi
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h> // Biblioteca para servidor web
#include <WiFiManager.h>      // Biblioteca para gerenciamento de WiFi
#include <ESP8266mDNS.h>      // Biblioteca para mDNS
#include <ArduinoJson.h>      // Biblioteca para manipulação de JSON

WiFiClient wifiClient;                                // Cria um objeto WiFiClient para conexões WiFi
char *site_url = "http://192.168.1.198:8080/api.php"; // URL do servidor API

#include <device.h> // Inclui o arquivo de cabeçalho para dispositivos
#include <sensor.h> // Inclui o arquivo de cabeçalho para sensores

// Configurações de rede
IPAddress staticIP(192, 168, 1, 155);  // IP estático desejado
IPAddress gateway(192, 168, 1, 100);   // Gateway
IPAddress subnet(255, 255, 255, 0);    // Máscara de sub-rede
IPAddress webServer(192, 168, 1, 198); // IP do servidor web

unsigned long previousMillis = 0; // Armazena o último tempo em que a ação foi executada
const long interval = 10000;      // Intervalo desejado (em milissegundos)
ESP8266WebServer server(80);      // Cria um servidor na porta 80

// Função para lidar com a requisição raiz
void handleRoot()
{
    server.sendHeader("Location", "/index.html"); // Redireciona para /index.html
    server.send(302, "text/plain", "");           // Envia uma resposta 302 (Found)
}

// Função para lidar com comandos recebidos via POST
void handleComando()
{
    if (server.hasArg("plain")) // Verifica se há um corpo na requisição
    {
        String body = server.arg("plain");                              // Obtém o corpo da requisição
        DynamicJsonDocument jsonBuffer(1024);                           // Cria um buffer JSON
        DeserializationError error = deserializeJson(jsonBuffer, body); // Desserializa o JSON
        if (error)
        {
            server.send(400, "application/json", "{\"status\":\"erro\",\"mensagem\":\"JSON inválido\"}"); // Responde com erro se o JSON for inválido
            return;
        }
        int pino = jsonBuffer["pino"]; // Obtém o valor do pino do JSON
        for (int i = 0; i < nArrayDispositivos; i++)
        {
            if (deviceList[i].pin == pino) // Verifica se o pino corresponde a um dispositivo
            {
                toggleDeviceState(&deviceList[i]);           // Alterna o estado do dispositivo
                inserirHistoricoDispositivo(&deviceList[i]); // Insere o histórico do dispositivo
            }
        }
        server.send(200, "application/json", "{\"status\":\"sucesso\",\"mensagem\":\"Comando recebido\"}"); // Responde com sucesso
    }
    else
    {
        server.send(400, "application/json", "{\"status\":\"erro\",\"mensagem\":\"Requisição sem corpo\"}"); // Responde com erro se não houver corpo
    }
}




// Função para lidar com requisições de dispositivos
void handleDispositivos()
{
    String response;
    DynamicJsonDocument jsonBuffer = getDeviceData(); // Obtém os dados dos dispositivos
    serializeJson(jsonBuffer, response);              // Serializa os dados em JSON
    server.send(200, "application/json", response);   // Envia a resposta JSON
}

// Função para lidar com requisições de sensores
void handleSensores()
{
    String response;
    DynamicJsonDocument jsonBuffer = getSensorData(); // Obtém os dados dos sensores
    serializeJson(jsonBuffer, response);              // Serializa os dados em JSON
    server.send(200, "application/json", response);   // Envia a resposta JSON
}

// Declaração de funções adicionais
void ReconfiguraWiFi();
void saveConfigCallback();
void configModeCallback(WiFiManager *myWiFiManager);
void atualizaHistoricoSensor();

void setup()
{
    Serial.begin(115200); // Inicializa a comunicação serial

    // Inicializa o SPIFFS
    if (!SPIFFS.begin())
    {
        Serial.println("Erro ao montar o sistema de arquivos SPIFFS");
        return;
    }

    WiFi.config(staticIP, gateway, subnet); // Configura o WiFi com IP estático
    // Inicializa o WiFiManager e tenta conectar
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(60);                // Define o tempo limite para o portal de configuração
    wifiManager.setAPCallback(configModeCallback);         // Define o callback para o modo de configuração
    wifiManager.setSaveConfigCallback(saveConfigCallback); // Define o callback para salvar a configuração
    wifiManager.autoConnect("ESP8266_AP", "123456789");    // Conecta automaticamente ao WiFi

    // Inicializa o mDNS
    if (!MDNS.begin("dispositivos"))
    {
        Serial.println("Erro ao configurar mDNS responder!");
        while (1)
        {
            delay(100); // Aguarda indefinidamente se houver erro
        }
    }
    Serial.println("Conectado à rede WiFi!");
    Serial.println("mDNS configurado. Acesse http://dispositivos.local");
    MDNS.addService("http", "tcp", 80); // Adiciona o serviço HTTP ao mDNS

    if (wifiClient.connect(webServer, 8080))
    {
        setupSensor();  // Configura os sensores
        setupDevices(); // Configura os dispositivos
    }
    // Configura o servidor web
    server.on("/", handleRoot);                                   // Define a função para a raiz
    server.on("/api/sensores", HTTP_GET, handleSensores);         // Define a função para /api/sensores
    server.on("/api/dispositivos", HTTP_GET, handleDispositivos); // Define a função para /api/dispositivos
    server.on("/api/comando", HTTP_POST, handleComando);          // Define a função para /api/comando
    server.serveStatic("/", SPIFFS, "/");                         // Serve arquivos estáticos do SPIFFS
    server.begin();                                               // Inicia o servidor
    Serial.println("Servidor HTTP iniciado");
}

void loop()
{
    MDNS.update();                     // Atualiza o serviço mDNS
    if (WiFi.status() == WL_CONNECTED) // Verifica se o WiFi está conectado
    {
        server.handleClient(); // Lida com as requisições do cliente
    }
    else
    {
        ReconfiguraWiFi(); // Reconfigura o WiFi se não estiver conectado
    }
    unsigned long currentMillis = millis();         // Obtém o tempo atual em milissegundos
    if (currentMillis - previousMillis >= interval) // Verifica se o intervalo de tempo foi atingido
    {
        previousMillis = currentMillis; // Atualiza o tempo anterior

        // Ação a ser executada
        atualizaHistoricoSensor(); // Atualiza o histórico dos sensores
    }
}

void ReconfiguraWiFi()
{
    WiFiManager wifiManager;           // Cria um objeto WiFiManager
    Serial.println(WiFi.status());     // Imprime o status do WiFi
    if (WiFi.status() != WL_CONNECTED) // Verifica se o WiFi não está conectado
    {
        if (WiFi.status() == WL_NO_SSID_AVAIL || WiFi.status() == WL_CONNECT_FAILED) // Verifica se o SSID não está disponível ou se a conexão falhou
        {
            wifiManager.setConfigPortalTimeout(60);                        // Define o tempo limite para o portal de configuração
            if (!wifiManager.startConfigPortal("ESP8266_AP", "123456789")) // Inicia o portal de configuração
            {
                Serial.println("Falha ao iniciar o portal de configuração"); // Imprime mensagem de erro se o portal não iniciar
            }
        }
    }
}

void saveConfigCallback()
{
    Serial.println("Configuração Salva"); // Imprime mensagem indicando que a configuração foi salva
    Serial.println(WiFi.softAPIP());      // Imprime o IP do ponto de acesso
}

void configModeCallback(WiFiManager *myWiFiManager)
{
    Serial.println("Entrou no Modo de Configuração!!!");  // Imprime mensagem indicando que entrou no modo de configuração
    Serial.println(WiFi.softAPIP());                      // Imprime o IP do ponto de acesso
    Serial.println(myWiFiManager->getConfigPortalSSID()); // Imprime o SSID do portal de configuração
}

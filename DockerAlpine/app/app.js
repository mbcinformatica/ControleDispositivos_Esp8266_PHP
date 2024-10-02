const express = require('express');
const path = require('path');
const app = express();
const port = 3000;
const sequelize = require('./services/databaseService');

// Middleware para servir arquivos estáticos
app.use(express.static(path.join(__dirname, 'public')));

// Importar e usar rotas da API
const apiRoutes = require('./routes/api');
app.use('/api', apiRoutes);

// Rota padrão
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

sequelize.sync()
    .then(() => {
        console.log('Banco de dados sincronizado com os modelos.');
        require('./services/mqttService');
        // Inicia o servidor depois de sincronizar o banco de dados
        app.listen(port, () => {
            console.log(`Servidor rodando na porta ${port}`);
        });
    })
    .catch(error => {
        console.error('Erro ao sincronizar o banco de dados:', error);
    });
// services/databaseService.js
const { Sequelize } = require('sequelize');

// Configure sua conexão com MySQL
const sequelize = new Sequelize('iot', 'root', 'password', {
    host: '192.168.1.198',
    dialect: 'mysql',
    logging: false
});

module.exports = sequelize;

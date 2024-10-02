const { DataTypes } = require('sequelize');
const sequelize = require('../services/databaseService');

// Definindo o modelo Dispositivo
const Dispositivo = sequelize.define('Dispositivo', {
    id: {
        type: DataTypes.INTEGER,
        autoIncrement: true,
        primaryKey: true
    },
    name: {
        type: DataTypes.STRING,
        allowNull: false
    },
    topic: {
        type: DataTypes.STRING,
        unique: true,
        allowNull: false
    }
}, {
    tableName: 'dispositivo', // Nome da tabela conforme SQL
    timestamps: false // Desativa timestamps automáticos
});

module.exports = {
    Dispositivo
};

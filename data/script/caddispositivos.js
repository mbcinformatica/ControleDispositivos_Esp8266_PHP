if (!window.caddispositivosController) {
  const caddispositivosController = {
    initialize: function () {
      const containerCadastro = document.querySelector(".js-container-caddispositivos");

      const formHTML = `
      <div class="form-container">
        <form id="formCadastroDispositivo">
          <span>Cadastro de Dispositivos</span><br><br>

          <label for="nome">Nome:</label>
          <input type="text" id="nome" name="nome" required><br>

          <label for="pino">Pino:</label>
          <input type="number" id="pino" name="pino" required min="0"><br>

          <label for="imageon">Imagem Ligado:</label>
          <input type="text" id="imageon" name="imageon" required><br>

          <label for="imageoff">Imagem Desligado:</label>
          <input type="text" id="imageoff" name="imageoff" required><br>

          <label for="largura">Largura da Imagem:</label>
          <input type="number" id="largura" name="largura" required><br>

          <label for="altura">Altura da Imagem:</label>
          <input type="number" id="altura" name="altura" required><br>

          <label for="preenchimento">Preenchimento da Imagem:</label>
          <input type="text" id="preenchimento" name="preenchimento" required><br>

          <label for="identifier">Identificador:</label>
          <input type="text" id="identifier" name="identifier" required disabled><br>

          <button type="submit" id="btnCadastrar">Cadastrar Dispositivo</button>
        </form>
        <form>
          <table id="tabelaDispositivos">  
            <span>Relação de Dispositivos</span><br><br>
            <thead>
              <tr>
                <th style="width: 60px; text-align: center;">ID</th>
                <th style="width: 250px; padding: 7px;">Nome</th>
                <th style="width: 90px; text-align: center;">Pino</th>
                <th style="width: 300px; padding: 7px;">Identificador</th>
              </tr>
            </thead>
            <tbody>
              <!-- Dispositivos serão carregados aqui -->
            </tbody>
          </table>
        </form>
      </div>`;

      containerCadastro.innerHTML = formHTML;

      const formCadastro = document.getElementById("formCadastroDispositivo");
      const tabelaDispositivos = document.getElementById("tabelaDispositivos").querySelector("tbody");

      // Função para buscar dispositivos e preencher a tabela
      function carregarDispositivos() {
        fetch(`http://192.168.1.198:8080/api.php?action=listar-dispositivos`)
          .then(response => {
            if (!response.ok) {
              throw new Error('Erro ao buscar dispositivos');
            }
            return response.json();
          })
          .then(dispositivos => {
            tabelaDispositivos.innerHTML = ''; // Limpar tabela antes de preencher

            dispositivos.forEach(dispositivo => {
              const row = document.createElement('tr');
              row.dataset.id = dispositivo.iddisp;
              row.dataset.nome = dispositivo.name;
              row.dataset.pino = dispositivo.pin;
              row.dataset.imagemLigado = dispositivo.imgon;
              row.dataset.imagemDesligado = dispositivo.imgoff;
              row.dataset.larguraImagem = dispositivo.widthimg;
              row.dataset.alturaImagem = dispositivo.heightimg;
              row.dataset.identifier = dispositivo.identifier;

              row.innerHTML = `
                <td style="text-align: center; font-size: 20px;">${dispositivo.iddisp}</td>
                <td style="font-size: 20px; padding: 7px;">${dispositivo.name}</td>
                <td style="text-align: center; font-size: 20px;">${dispositivo.pin}</td>
                <td style="font-size: 20px; padding: 7px;">${dispositivo.identifier}</td>
              `;
              tabelaDispositivos.appendChild(row);
            });
          })
          .catch(error => console.error('Erro ao buscar dispositivos:', error));
      }

      carregarDispositivos();

      formCadastro.addEventListener("submit", function (event) {
        event.preventDefault();
        const formData = new FormData(formCadastro);
        const dispositivo = {
          name: formData.get("nome"),
          pin: formData.get("pino"),
          imgon: formData.get("imageon"),
          imgoff: formData.get("imageoff"),
          widthimg: formData.get("largura"),
          heightimg: formData.get("altura"),
          paddingimg: formData.get("preenchimento"),
          identifier: formData.get("identifier")
        };

        fetch(`http://192.168.1.198:8080/api.php?action=inserir-dispositivo`, {
          method: 'POST',
          body: JSON.stringify(dispositivo),
          headers: {
            'Content-Type': 'application/json'
          }
        })
          .then(response => {
            if (!response.ok) {
              throw new Error('Erro ao realizar a operação');
            }
            return response.json();
          })
          .then(data => {
            alert(data.message || 'Operação realizada com sucesso!');
            formCadastro.reset(); // Limpar os dados preenchidos
            carregarDispositivos(); // Recarregar a lista de dispositivos
          })
          .catch(error => console.error('Erro ao realizar a operação:', error));
      });
    }
  };

  window.caddispositivosController = caddispositivosController;
  document.addEventListener('DOMContentLoaded', caddispositivosController.initialize);
}
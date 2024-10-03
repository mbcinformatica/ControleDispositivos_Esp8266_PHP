document.addEventListener("DOMContentLoaded", () => {
  const containerCadastro = document.querySelector(".js-container-caddispositivos");

  const formHTML = `
    <form id="formCadastroDispositivo">
      <label for="nome">Nome:</label>
      <input type="text" id="nome" name="nome" required><br>

      <label for="pino">Pino:</label>
      <input type="number" id="pino" name="pino" required><br>

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
      <input type="text" id="identifier" name="identifier" required><br>

      <button type="submit">Cadastrar Dispositivo</button>
    </form>
  `;

  containerCadastro.innerHTML = formHTML;

  const formCadastro = document.getElementById("formCadastroDispositivo");

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
    cadastrarDispositivo(dispositivo);
  });

  function cadastrarDispositivo(dispositivo) {
    const urlDispositivo = `http://192.168.1.198:8080/api.php?action=inserir-dispositivo&name=${encodeURIComponent(dispositivo.name)}&pin=${dispositivo.pin}&imgon=${encodeURIComponent(dispositivo.imgon)}&imgoff=${encodeURIComponent(dispositivo.imgoff)}&widthimg=${dispositivo.widthimg}&heightimg=${dispositivo.heightimg}&paddingimg=${encodeURIComponent(dispositivo.paddingimg)}&identifier=${encodeURIComponent(dispositivo.identifier)}`;

    fetch(urlDispositivo, {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json'
      }
    })
      .then(response => {
        if (!response.ok) {
          throw new Error('Erro ao cadastrar dispositivo');
        }
        return response.json();
      })
      .then(() => {
        alert('Dispositivo cadastrado com sucesso!');
        formCadastro.reset();
      })
      .catch(error => console.error('Erro ao cadastrar dispositivo:', error));
  }
});

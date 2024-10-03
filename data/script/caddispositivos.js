document.addEventListener("DOMContentLoaded", () => {
  const form = document.getElementById("deviceForm");

  form.addEventListener("submit", (event) => {
    event.preventDefault();

    const dispositivo = {
      name: document.getElementById('name').value,
      pin: document.getElementById('pin').value,
      imgon: document.getElementById('imgon').value,
      imgoff: document.getElementById('imgoff').value,
      widthimg: document.getElementById('widthimg').value,
      heightimg: document.getElementById('heightimg').value,
      paddingimg: document.getElementById('paddingimg').value,
      identifier: 'some-unique-identifier' // Adicione um identificador único aqui
    };

    fetch('/api/dispositivos?action=inserir-dispositivo', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify(dispositivo)
    })
    .then(response => response.json())
    .then(data => {
      if (data.error) {
        console.error('Erro:', data.error);
      } else {
        console.log('Sucesso:', data.message);
      }
    })
    .catch(error => console.error('Erro ao enviar dados:', error));
  });
});

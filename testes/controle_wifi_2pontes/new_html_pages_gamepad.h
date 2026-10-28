//=====================
//HTML code for webpage
//=====================
const char webpageCode[] PROGMEM =
R"=====(
<!DOCTYPE html>
<html lang="pt">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>RoboLight - Módulo Clínico</title>
  <style>
    :root {
      --bg-color: #f1f5f9;
      --panel-bg: #ffffff;
      --header-bg: #0f172a;
      --primary-blue: #0284c7;
      --primary-hover: #0369a1;
      --primary-active: #0c4a6e;
      --medical-green: #10b981;
      --medical-red: #dc2626;
      --text-dark: #1e293b;
      --text-muted: #64748b;
      --border-color: #cbd5e1;
      --btn-bg: #f8fafc;
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Arial, sans-serif;
      user-select: none;
    }

    body {
      background-color: var(--bg-color);
      color: var(--text-dark);
      display: flex;
      flex-direction: column;
      align-items: center;
      min-height: 100vh;
      padding-bottom: 20px;
    }

    /* Barra Superior Hospitalar */
    .top-bar {
      width: 100%;
      background-color: var(--header-bg);
      color: #ffffff;
      padding: 12px 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      box-shadow: 0 2px 4px rgba(0,0,0,0.1);
      margin-bottom: 20px;
    }

    .brand {
      display: flex;
      align-items: center;
      gap: 10px;
    }

    .brand-icon {
      color: var(--primary-blue);
      font-size: 1.5rem;
      font-weight: bold;
    }

    .status-badge {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      font-size: 0.85rem;
      background: rgba(255,255,255,0.1);
      padding: 4px 10px;
      border-radius: 20px;
    }

    .status-dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background-color: var(--medical-red);
      transition: background-color 0.3s;
    }

    .status-dot.online {
      background-color: var(--medical-green);
    }

    /* Layout da Dashboard (Expandido para acomodar a tela maior) */
    .dashboard {
      display: flex;
      flex-direction: column;
      gap: 20px;
      width: 95%;
      max-width: 1200px;
    }

    @media (min-width: 768px) {
      .dashboard {
        flex-direction: row;
        align-items: stretch;
      }
    }

    /* Cards/Painéis */
    .card {
      background: var(--panel-bg);
      border-radius: 12px;
      border: 1px solid var(--border-color);
      box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.05);
      flex: 1;
      display: flex;
      flex-direction: column;
      overflow: hidden;
    }

    /* Painel do vídeo expandido para ocupar maior proporção visual */
    .card-video {
      flex: 1.6;
    }

    .card-header {
      background: #f8fafc;
      padding: 12px 16px;
      font-size: 0.9rem;
      font-weight: 600;
      color: var(--text-muted);
      border-bottom: 1px solid var(--border-color);
      text-transform: uppercase;
      letter-spacing: 0.5px;
      display: flex;
      align-items: center;
      justify-content: space-between;
    }

    .card-body {
      padding: 20px;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      flex: 1;
    }

    /* Stream de Vídeo (Proporção Widescreen Expandida) */
    .video-container {
      width: 100%;
      aspect-ratio: 16 / 9;
      min-height: 320px;
      border-radius: 8px;
      overflow: hidden;
      background: #0f172a;
      border: 1px solid var(--border-color);
    }

    iframe {
      width: 100%;
      height: 100%;
      border: none;
    }

    /* Controle D-Pad (Botões de Movimentação) */
    .dpad {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      grid-template-rows: repeat(3, 1fr);
      gap: 10px;
      width: 220px;
      height: 220px;
    }

    .btn-cmd {
      background-color: var(--header-bg);
      color: #ffffff;
      border: 1px solid var(--header-bg);
      border-radius: 10px;
      font-size: 1.8rem;
      font-weight: bold;
      cursor: pointer;
      display: flex;
      justify-content: center;
      align-items: center;
      transition: all 0.15s ease;
      box-shadow: 0 2px 4px rgba(1, 24, 36, 0.3);
    }

    .btn-cmd:hover, .btn-cmd.active {
      background-color: var(--primary-hover);
      box-shadow: 0 4px 6px rgba(2, 132, 199, 0.4);
    }

    .btn-cmd:active, .btn-cmd.active-press {
      background-color: var(--primary-active);
      transform: scale(0.96);
      box-shadow: none;
    }

    .btn-stop {
      background-color: var(--medical-red);
      color: #ffffff;
      border-color: #7e1010;
      font-size: 1.3rem;
      font-weight: bold;
      box-shadow: 0 2px 4px rgba(220, 38, 38, 0.3);
    }

    .btn-stop:hover, .btn-stop.active {
      background-color: #b91c1c;
      box-shadow: 0 4px 6px rgba(220, 38, 38, 0.4);
    }

    .btn-stop:active, .btn-stop.active-press {
      background-color: #991b1b;
      transform: scale(0.96);
      box-shadow: none;
    }
    
    .kb-hint {
      margin-top: 15px;
      font-size: 0.8rem;
      color: var(--text-muted);
      text-align: center;
    }

    /* Painel de Luz Auxiliar / Iluminação */
    .light-section {
      width: 100%;
      margin-top: 20px;
      padding-top: 15px;
      border-top: 1px dashed var(--border-color);
      display: flex;
      justify-content: center;
    }

    /* Controle de velocidade */
    .speed-section {
      width: 100%;
      max-width: 260px;
      margin-top: 20px;
      padding-top: 15px;
      border-top: 1px dashed var(--border-color);
    }

    .speed-label {
      display: flex;
      justify-content: space-between;
      font-size: 0.85rem;
      font-weight: 600;
      color: var(--text-muted);
      text-transform: uppercase;
      letter-spacing: 0.5px;
      margin-bottom: 10px;
    }

    #slider-velocidade {
      width: 100%;
      cursor: pointer;
      accent-color: var(--primary-blue);
    }

    .btn-diag {
      font-size: 1.5rem;
    }

    #btn-led {
      width: 100%;
      max-width: 220px;
      padding: 12px;
      border-radius: 8px;
      border: 1px solid var(--border-color);
      font-weight: 600;
      font-size: 0.9rem;
      color: var(--text-dark);
      background: var(--btn-bg);
      cursor: pointer;
      transition: all 0.25s ease;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
    }

    #btn-led.on {
      background: #ecfdf5;
      color: var(--medical-green);
      border-color: #a7f3d0;
      box-shadow: 0 0 10px rgba(16, 185, 129, 0.15);
    }
  </style>
</head>
<body>

  <!-- Header Superior Clínico -->
  <header class="top-bar">
    <div class="brand">
      <span class="brand-icon">&#10010;</span>
    </div>
    <div class="status-badge">
      <span id="status-dot" class="status-dot"></span>
      <span id="status-text">Desconectado</span>
    </div>
  </header>

  <div class="dashboard">
    <!-- Módulo de Navegação -->
    <div class="card">
      <div class="card-header">
        <span>Controle de Movimentação</span>
        <span>&#8644;</span>
      </div>
      <div class="card-body">
        <div class="dpad">
          <button class="btn-cmd btn-diag" id="btn-up-left" onclick="buttonFrenteEsq()">&#8598;</button>
          <button class="btn-cmd" id="btn-up" onclick="buttonFrente()">&uarr;</button>
          <button class="btn-cmd btn-diag" id="btn-up-right" onclick="buttonFrenteDir()">&#8599;</button>

          <button class="btn-cmd" id="btn-left" onclick="buttonEsquerda()">&larr;</button>
          <button class="btn-cmd btn-stop" id="btn-stop" onclick="buttonParado()">&#9632;</button>
          <button class="btn-cmd" id="btn-right" onclick="buttonDireita()">&rarr;</button>

          <button class="btn-cmd btn-diag" id="btn-down-left" onclick="buttonTrasEsq()">&#8601;</button>
          <button class="btn-cmd" id="btn-down" onclick="buttonTras()">&darr;</button>
          <button class="btn-cmd btn-diag" id="btn-down-right" onclick="buttonTrasDir()">&#8600;</button>
        </div>
        
        <div class="kb-hint">Controles: WASD ou Setas | Q E Z C diagonais | Espaço p/ Parar</div>

        <div class="speed-section">
          <div class="speed-label">
            <span>Velocidade</span>
            <span id="valor-velocidade">150</span>
          </div>
          <input type="range" id="slider-velocidade" min="0" max="255" value="150" oninput="mudarVelocidade(this.value)">
        </div>

        <div class="light-section">
          <button id="btn-led" onclick="button()">
            <span>Iluminação Auxiliar: OFF</span>
          </button>
        </div>
      </div>
    </div>

    <!-- Módulo de Visão / Câmera (Expandido) -->
    <div class="card card-video">
      <div class="card-header">
        <span>Monitoramento de Vídeo</span>
        <span>&#128065;</span>
      </div>
      <div class="card-body">
        <div class="video-container">
          <iframe src="http://192.168.4.3:8080/" title="Câmera do Robô"></iframe>
        </div>
      </div>
    </div>
  </div>

  <script>
    var websock;

    function InitWebSocket() {
      websock = new WebSocket('ws://' + window.location.hostname + ':81/');
      
      websock.onopen = function() {
        document.getElementById('status-dot').classList.add('online');
        document.getElementById('status-text').innerText = 'Conectado';
        mudarVelocidade(document.getElementById('slider-velocidade').value);
      };

      websock.onmessage = function(evt) {
        try {
          var JSONobj = JSON.parse(evt.data);
          var btnLed = document.getElementById('btn-led');
          
          if (JSONobj.LEDonoff === 'ON') {
            btnLed.innerHTML = '<span>Iluminação Auxiliar: ON</span>';
            btnLed.classList.add('on');
          } else {
            btnLed.innerHTML = '<span>Iluminação Auxiliar: OFF</span>';
            btnLed.classList.remove('on');
          }
        } catch (e) {
          console.error("Erro no processamento dos dados recebidos:", e);
        }
      };

      websock.onclose = function() {
        document.getElementById('status-dot').classList.remove('online');
        document.getElementById('status-text').innerText = 'Desconectado';
        setTimeout(InitWebSocket, 2000); // Tentativa automática de reconexão
      };
    }

    function button() {
      var statusAtual = document.getElementById('btn-led').classList.contains('on');
      websock.send(statusAtual ? 'LEDonoff=OFF' : 'LEDonoff=ON');
    }

    function buttonFrente()   { websock.send('ComandoDirecao=F'); }
    function buttonEsquerda() { websock.send('ComandoDirecao=E'); }
    function buttonParado()   { websock.send('ComandoDirecao=P'); }
    function buttonDireita()  { websock.send('ComandoDirecao=D'); }
    function buttonTras()     { websock.send('ComandoDirecao=T'); }
    function buttonFrenteEsq() { websock.send('ComandoDirecao=G'); }
    function buttonFrenteDir() { websock.send('ComandoDirecao=I'); }
    function buttonTrasEsq()   { websock.send('ComandoDirecao=H'); }
    function buttonTrasDir()   { websock.send('ComandoDirecao=J'); }

    function mudarVelocidade(valor) {
      document.getElementById('valor-velocidade').innerText = valor;
      if (websock && websock.readyState === 1) {
        websock.send('Velocidade=' + valor);
      }
    }

    // Integração do Teclado (WASD / Setas / Espaço)
    document.addEventListener('keydown', function(event) {
      if (event.repeat) return; // Evita enviar o comando repetidamente se segurar a tecla
      
      const key = event.key.toLowerCase();
      
      switch(key) {
        case 'w':
        case 'arrowup':
          buttonFrente();
          visualFeedback('btn-up');
          break;
        case 'a':
        case 'arrowleft':
          buttonEsquerda();
          visualFeedback('btn-left');
          break;
        case 's':
        case 'arrowdown':
          buttonTras();
          visualFeedback('btn-down');
          break;
        case 'd':
        case 'arrowright':
          buttonDireita();
          visualFeedback('btn-right');
          break;
        case 'q':
          buttonFrenteEsq();
          visualFeedback('btn-up-left');
          break;
        case 'e':
          buttonFrenteDir();
          visualFeedback('btn-up-right');
          break;
        case 'z':
          buttonTrasEsq();
          visualFeedback('btn-down-left');
          break;
        case 'c':
          buttonTrasDir();
          visualFeedback('btn-down-right');
          break;
        case ' ': // Barra de espaço
          event.preventDefault(); // Impede que a página role para baixo
          buttonParado();
          visualFeedback('btn-stop');
          break;
      }
    });

    // Função para adicionar efeito visual no botão virtual ao usar o teclado
    function visualFeedback(btnId) {
      const btn = document.getElementById(btnId);
      if(btn) {
        btn.classList.add('active-press');
        setTimeout(() => btn.classList.remove('active-press'), 150);
      }
    }

    // Controle por gamepad usando a Gamepad API.
    // Mapeamento padrão (layout comum de controle PlayStation/XInput):
    // D-pad: botões 12-15; analógico esquerdo: eixos 0 (horizontal) e 1 (vertical).
    // X/O: parar; Triângulo: iluminação; L1/R1: reduzir/aumentar velocidade.
    // A leitura é contínua, mas os comandos WebSocket só são enviados quando mudam.
    var padUltimoCmd = null;
    var padBtnAnt = {};
    var padIndiceAtivo = null;
    var padZonaMorta = 0.30;
    var padConectadoAntes = false;
    var padUltimoTempoVelocidade = 0;

    function padBotaoPressionado(pad, indice) {
      var botao = pad.buttons && pad.buttons[indice];
      return !!(botao && (botao.pressed || botao.value > 0.65));
    }

    // Retorna true apenas na transição de solto para pressionado.
    function padBotaoNovoPressionamento(pad, indice) {
      var agora = padBotaoPressionado(pad, indice);
      var antes = !!padBtnAnt[indice];
      padBtnAnt[indice] = agora;
      return agora && !antes;
    }

    function padEixo(pad, indice) {
      return pad.axes && typeof pad.axes[indice] === 'number' ? pad.axes[indice] : 0;
    }

    function padEnviarDirecao(comando) {
      if (!websock || websock.readyState !== WebSocket.OPEN) return;
      if (comando !== padUltimoCmd) {
        websock.send('ComandoDirecao=' + comando);
        padUltimoCmd = comando;
      }
    }

    function padAtualizarStatus(pad) {
      var dot = document.getElementById('status-dot');
      var texto = document.getElementById('status-text');
      if (pad) {
        texto.innerText = 'Controle conectado';
        dot.classList.add('online');
      } else if (websock && websock.readyState === WebSocket.OPEN) {
        texto.innerText = 'Robô conectado';
        dot.classList.add('online');
      } else {
        texto.innerText = 'Desconectado';
        dot.classList.remove('online');
      }
    }

    function lerGamepad() {
      var lista = navigator.getGamepads ? navigator.getGamepads() : [];
      var pad = null;

      // Mantém o controle previamente selecionado se ainda estiver conectado.
      if (padIndiceAtivo !== null && lista[padIndiceAtivo] && lista[padIndiceAtivo].connected) {
        pad = lista[padIndiceAtivo];
      } else {
        for (var i = 0; i < lista.length; i++) {
          if (lista[i] && lista[i].connected) {
            pad = lista[i];
            padIndiceAtivo = lista[i].index;
            break;
          }
        }
      }

      if (pad) {
        if (!padConectadoAntes) {
          padBtnAnt = {};
          padUltimoCmd = null;
        }
        padConectadoAntes = true;
        padAtualizarStatus(pad);

        var b = pad.buttons || [];
        var eixoX = padEixo(pad, 0);
        var eixoY = padEixo(pad, 1);

        // D-pad digital e analógico esquerdo. D-pad tem prioridade quando pressionado.
        var frente = padBotaoPressionado(pad, 12) || eixoY < -padZonaMorta;
        var tras = padBotaoPressionado(pad, 13) || eixoY > padZonaMorta;
        var esquerda = padBotaoPressionado(pad, 14) || eixoX < -padZonaMorta;
        var direita = padBotaoPressionado(pad, 15) || eixoX > padZonaMorta;

        // Impede comandos contraditórios se duas direções opostas forem acionadas juntas.
        if (frente && tras) { frente = false; tras = false; }
        if (esquerda && direita) { esquerda = false; direita = false; }

        var comando = 'P';
        if (frente) comando = esquerda ? 'G' : direita ? 'I' : 'F';
        else if (tras) comando = esquerda ? 'H' : direita ? 'J' : 'T';
        else if (esquerda) comando = 'E';
        else if (direita) comando = 'D';

        // Botões 0/1 (X/O em mapeamento comum) funcionam como parada manual.
        // A verificação por borda evita enviar parada repetidamente enquanto segura.
        var pararX = padBotaoNovoPressionamento(pad, 0);
        var pararO = padBotaoNovoPressionamento(pad, 1);
        if (pararX || pararO) {
          comando = 'P';
        }

        padEnviarDirecao(comando);

        // Triângulo (botão 3): alterna iluminação somente ao pressionar.
        if (padBotaoNovoPressionamento(pad, 3)) {
          if (websock && websock.readyState === WebSocket.OPEN) button();
        }

        // L1 (4) reduz e R1 (5) aumenta a velocidade, em passos de 25.
        // Pequena trava para não alterar muitas vezes por uma única pressão.
        var agora = Date.now();
        var reduzir = padBotaoNovoPressionamento(pad, 4);
        var aumentar = padBotaoNovoPressionamento(pad, 5);
        if ((reduzir || aumentar) && agora - padUltimoTempoVelocidade > 140) {
          var slider = document.getElementById('slider-velocidade');
          var delta = (aumentar ? 25 : 0) - (reduzir ? 25 : 0);
          slider.value = Math.max(0, Math.min(255, Number(slider.value) + delta));
          mudarVelocidade(slider.value);
          padUltimoTempoVelocidade = agora;
        }

        // Feedback visual do D-pad/analógico.
        document.querySelectorAll('.btn-cmd').forEach(function(btn) {
          btn.classList.remove('active');
        });
        var idAtivo = {
          F: 'btn-up', T: 'btn-down', E: 'btn-left', D: 'btn-right',
          G: 'btn-up-left', I: 'btn-up-right', H: 'btn-down-left',
          J: 'btn-down-right', P: 'btn-stop'
        }[comando];
        var elementoAtivo = document.getElementById(idAtivo);
        if (elementoAtivo) elementoAtivo.classList.add('active');
      } else {
        if (padConectadoAntes) {
          // Ao perder o controle, ordena parada para evitar que o robô continue andando.
          padEnviarDirecao('P');
          padIndiceAtivo = null;
          padBtnAnt = {};
          padUltimoCmd = 'P';
        }
        padConectadoAntes = false;
        padAtualizarStatus(null);
        document.querySelectorAll('.btn-cmd').forEach(function(btn) {
          btn.classList.remove('active');
        });
      }

      requestAnimationFrame(lerGamepad);
    }

    window.addEventListener('gamepadconnected', function(event) {
      padIndiceAtivo = event.gamepad.index;
      padBtnAnt = {};
      padUltimoCmd = null;
      padConectadoAntes = true;
    });

    window.addEventListener('gamepaddisconnected', function(event) {
      if (padIndiceAtivo === event.gamepad.index) {
        padEnviarDirecao('P');
        padIndiceAtivo = null;
        padBtnAnt = {};
        padUltimoCmd = 'P';
        padConectadoAntes = false;
      }
    });

    window.onload = function() {
      InitWebSocket();
      lerGamepad();
    };
  </script>
</body>
</html>
)=====";
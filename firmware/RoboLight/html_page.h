const char webpageCode[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pt">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>RoboLight - Módulo Clínico</title>
  <style>
    /* ---------- Cores usadas na página (mude aqui para trocar o tema) ---------- */
    :root {
      --bg-color: #f1f5f9;       /* fundo da página */
      --panel-bg: #ffffff;       /* fundo dos cartões */
      --header-bg: #0f172a;      /* azul escuro da barra e dos botões */
      --primary-blue: #0284c7;   /* azul de destaque */
      --primary-hover: #0369a1;  /* azul ao passar o mouse */
      --primary-active: #0c4a6e; /* azul ao apertar */
      --medical-green: #10b981;  /* verde (conectado / LED ligado) */
      --medical-red: #dc2626;    /* vermelho (desconectado / emergência) */
      --uv-purple: #6d28d9;      /* roxo das lâmpadas UVC */
      --text-dark: #1e293b;      /* texto principal */
      --text-muted: #64748b;     /* texto secundário */
      --border-color: #cbd5e1;   /* bordas */
      --btn-bg: #f8fafc;         /* fundo dos botões claros */
      --dpad-size: min(220px, 24vh); /* tamanho da cruz de direção (diminui em telas baixas) */
    }

    /* Zera margens e usa a mesma fonte em tudo; user-select evita selecionar texto ao segurar o botão */
    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Arial, sans-serif;
      user-select: none;
    }

    /* Corpo da página: coluna centralizada */
    body {
      background-color: var(--bg-color);
      color: var(--text-dark);
      display: flex;
      flex-direction: column;
      align-items: center;
      min-height: 100vh;
      padding-bottom: 20px;
    }

    /* ---------- Barra superior: logo | painéis de tempo | status da conexão ---------- */
    .top-bar {
      width: 100%;
      background-color: var(--header-bg);
      color: #ffffff;
      padding: 8px 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-wrap: wrap;
      gap: 8px;
      box-shadow: 0 2px 4px rgba(0,0,0,0.1);
      margin-bottom: 20px;
    }

    .brand-icon {
      color: var(--primary-blue);
      font-size: 1.5rem;
      font-weight: bold;
    }
    
    .paineis-tempo {
      display: flex;
      gap: 12px;
      align-items: center;
      flex-wrap: wrap;
      justify-content: center;
    }

    /* Tempos: grandes, no meio da barra; ficam iluminados com o robô ou UVC ligados */
    #painel-tempo, #painel-uv {
      display: flex;
      align-items: center;
      gap: 14px;
      padding: 4px 24px;
      background: rgba(255,255,255,0.08);
      border: 2px solid rgba(255,255,255,0.25);
      border-radius: 12px;
      transition: all 0.3s ease;
    }
    
    #painel-tempo .rotulo, #painel-uv .rotulo {
      font-size: 0.75rem;
      color: #cbd5e1;
      text-transform: uppercase;
      letter-spacing: 1px;
    }
    
    #tempo-ativo, #tempo-uv {
      font-family: monospace;
      font-size: 2rem;
      font-weight: bold;
      letter-spacing: 3px;
      color: #ffffff;
    }
    
    #painel-tempo.ativo {
      background: rgba(16, 185, 129, 0.2);
      border-color: var(--medical-green);
      box-shadow: 0 0 16px rgba(16, 185, 129, 0.5);
    }
    #painel-tempo.ativo #tempo-ativo { color: #6ee7b7; }

    #painel-uv.ativo {
      background: rgba(109, 40, 217, 0.2);
      border-color: var(--uv-purple);
      box-shadow: 0 0 16px rgba(109, 40, 217, 0.5);
    }
    #painel-uv.ativo #tempo-uv { color: #d8b4fe; } /* Roxo claro */

    /* Etiqueta "Conectado / Desconectado" */
    .status-badge {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      font-size: 0.85rem;
      background: rgba(255,255,255,0.1);
      padding: 4px 10px;
      border-radius: 20px;
    }

    /* Bolinha do status: vermelha por padrão, verde quando tem a classe "online" */
    .status-dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background-color: var(--medical-red);
      transition: background-color 0.3s;
    }
    .status-dot.online { background-color: var(--medical-green); }

    /* No celular estreito some o texto dos cronômetros para caber tudo na barra */
    @media (max-width: 580px) {
      #painel-tempo .rotulo, #painel-uv .rotulo { display: none; }
      .paineis-tempo { gap: 6px; }
      #painel-tempo, #painel-uv { padding: 4px 12px; gap: 8px; }
      #tempo-ativo, #tempo-uv { font-size: 1.5rem; letter-spacing: 1px; }
    }

    /* ---------- Área principal (em telas pequenas os blocos ficam um embaixo do outro) ---------- */
    .app {
      display: flex;
      flex-direction: column;
      gap: 20px;
      width: 95%;
      max-width: 1200px;
    }
    .coluna-esq, .coluna-dir {
      display: flex;
      flex-direction: column;
      gap: 20px;
    }

    /* ---------- Cartões ---------- */
    .card {
      background: var(--panel-bg);
      border-radius: 12px;
      border: 1px solid var(--border-color);
      box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.05);
      flex: none;
      display: flex;
      flex-direction: column;
      overflow: hidden;
    }

    .card-header {
      background: #f8fafc;
      padding: 8px 16px;
      font-size: 0.85rem;
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
      padding: 16px;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      flex: 1;
    }

    /* Mensagem de estado no topo do controle (robô desligado, ligado, sem conexão...) */
    .aviso-controle {
      width: 100%;
      margin-bottom: clamp(6px, 1.5vh, 14px);
      padding: 6px 12px;
      border-radius: 8px;
      border: 1px solid var(--border-color);
      background: #f1f5f9;
      color: var(--text-muted);
      font-size: 0.8rem;
      text-align: center;
    }
    .aviso-controle.ok   { background: #ecfdf5; border-color: #a7f3d0; color: #047857; }
    .aviso-controle.erro { background: #fef2f2; border-color: #fecaca; color: #b91c1c; }

    /* ---------- Vídeo da câmera ---------- */
    .video-container {
      width: 100%;
      aspect-ratio: 16 / 9;
      border-radius: 8px;
      overflow: hidden;
      background: #0f172a;
      border: 1px solid var(--border-color);
      position: relative;
    }

    #camera {
      position: absolute;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      border: 0;
    }

    /* ---------- Botão de emergência: separado, grande e sempre liberado ---------- */
    .btn-emergencia {
      width: 100%;
      min-height: clamp(44px, 8vh, 72px);
      margin-top: clamp(16px, 3vh, 24px); /* Espaço acima do botão, após a velocidade */
      background-color: var(--medical-red);
      color: #ffffff;
      border: 3px solid #7f1d1d;
      border-radius: 12px;
      font-size: 1.1rem;
      font-weight: bold;
      letter-spacing: 1px;
      text-transform: uppercase;
      cursor: pointer;
      display: flex;
      justify-content: center;
      align-items: center;
      gap: 10px;
      box-shadow: 0 0 0 4px rgba(220, 38, 38, 0.2), 0 4px 8px rgba(220, 38, 38, 0.4);
      transition: all 0.15s ease;
      touch-action: none;
    }
    .btn-emergencia:hover { background-color: #b91c1c; }
    .btn-emergencia:active, .btn-emergencia.active-press {
      background-color: #991b1b;
      transform: scale(0.98);
      box-shadow: 0 0 0 4px rgba(220, 38, 38, 0.2);
    }

    /* ---------- Botões de direção: grade 3x3, 4 setas e o botão de parar no centro ---------- */
    .dpad {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      grid-template-rows: repeat(3, 1fr);
      gap: 8px;
      width: var(--dpad-size);
      height: var(--dpad-size);
    }

    .btn-cmd {
      background-color: var(--header-bg);
      color: #ffffff;
      border: 1px solid var(--header-bg);
      border-radius: 10px;
      font-size: 1.6rem;
      font-weight: bold;
      cursor: pointer;
      display: flex;
      justify-content: center;
      align-items: center;
      transition: all 0.15s ease;
      box-shadow: 0 2px 4px rgba(1, 24, 36, 0.3);
      touch-action: none;
    }
    .btn-cmd:hover { background-color: var(--primary-hover); box-shadow: 0 4px 6px rgba(2, 132, 199, 0.4); }
    .btn-cmd:active, .btn-cmd.active-press {
      background-color: var(--primary-active);
      transform: scale(0.96);
      box-shadow: none;
    }

    .btn-stop {
      background-color: var(--medical-red);
      border-color: #7e1010;
      font-size: 1.2rem;
      box-shadow: 0 2px 4px rgba(220, 38, 38, 0.3);
    }
    .btn-stop:hover { background-color: #b91c1c; box-shadow: 0 4px 6px rgba(220, 38, 38, 0.4); }
    .btn-stop:active, .btn-stop.active-press { background-color: #991b1b; }

    .btn-cmd:disabled,
    .btn-cmd:disabled:hover,
    .btn-cmd:disabled:active {
      background-color: var(--header-bg);
      opacity: 0.35;
      transform: none;
      box-shadow: none;
      cursor: not-allowed;
    }

    .kb-hint {
      margin-top: 10px;
      font-size: 0.75rem;
      color: var(--text-muted);
      text-align: center;
    }
    @media (max-height: 760px) {
      .kb-hint { display: none; }
    }

    /* ---------- Barra de velocidade ---------- */
    .speed-section {
      width: 100%;
      max-width: 260px;
      margin-top: clamp(8px, 2vh, 20px);
      padding-top: clamp(6px, 1.5vh, 15px);
      border-top: 1px dashed var(--border-color);
      text-align: center;
    }
    .speed-label {
      font-size: 0.85rem;
      font-weight: 600;
      margin-bottom: 6px;
    }
    #velocidade {
      width: 100%;
      accent-color: var(--primary-blue);
      cursor: pointer;
    }

    /* ---------- Lâmpadas: um botão grande pegando a largura toda ---------- */
    .lamp-grid {
      width: 100%;
      display: flex;
    }

    .lamp-btn {
      width: 100%;
      min-height: clamp(60px, 10vh, 96px);
      padding: 8px;
      border-radius: 12px;
      border: 2px solid var(--border-color);
      background: var(--btn-bg);
      color: var(--text-dark);
      font-size: 1rem;
      font-weight: bold;
      cursor: pointer;
      transition: all 0.25s ease;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      gap: 6px;
    }

    .lamp-estado {
      font-size: 0.8rem;
      padding: 2px 12px;
      border-radius: 20px;
      background: #e2e8f0;
      color: var(--text-muted);
    }

    #btn-uv {
      border-color: var(--uv-purple);
      color: var(--uv-purple);
    }
    #btn-uv.on {
      background: var(--uv-purple);
      color: #ffffff;
      box-shadow: 0 0 14px rgba(109, 40, 217, 0.45);
    }
    #btn-uv.on .lamp-estado { background: #ffffff; color: var(--uv-purple); }
    #btn-uv:disabled {
      opacity: 0.45;
      cursor: not-allowed;
    }

    .uv-aviso {
      margin-top: clamp(8px, 1.5vh, 16px);
      font-size: 0.75rem;
      color: var(--medical-red);
      text-align: center;
      font-weight: 600;
    }

    /* ---------- Checklist de segurança ---------- */
    .checklist {
      width: 100%;
      background: var(--panel-bg);
      border-radius: 12px;
      border: 1px solid var(--border-color);
      box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.05);
      overflow: hidden;
    }

    .cl-corpo {
      display: flex;
      flex-direction: column;
    }
    .cl-itens {
      flex: 1;
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    }

    .cl-item {
      display: flex;
      align-items: center;
      gap: 8px;
      padding: 8px 12px;
      border-bottom: 1px solid var(--border-color);
      font-size: 0.8rem;
      line-height: 1.2;
      cursor: pointer;
    }
    .cl-item input {
      width: 18px;
      height: 18px;
      flex-shrink: 0;
      cursor: pointer;
      accent-color: var(--medical-green);
    }
    .cl-item input:disabled { cursor: not-allowed; }

    .cl-rodape {
      display: flex;
      flex-direction: column;
      justify-content: center;
      gap: 8px;
      padding: 12px;
      background: #f8fafc;
    }
    #contador {
      font-size: 0.75rem;
      color: var(--text-muted);
      text-align: center;
    }

    #btn-ligar {
      width: 100%;
      padding: 12px;
      font-size: 0.95rem;
      font-weight: bold;
      color: #ffffff;
      background: var(--medical-green);
      border: none;
      border-radius: 8px;
      cursor: pointer;
      transition: background 0.2s ease;
    }
    #btn-ligar:disabled {
      background: #9ca3af;
      cursor: not-allowed;
    }
    #btn-ligar.desligar { background: var(--medical-red); }

    /* ---------- Janela de confirmação das lâmpadas UVC ---------- */
    .modal {
      display: none;
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: rgba(15, 23, 42, 0.6);
      align-items: center;
      justify-content: center;
      padding: 16px;
      z-index: 100;
    }
    .modal.aberto { display: flex; }

    .modal-caixa {
      width: 100%;
      max-width: 380px;
      background: var(--panel-bg);
      border: 2px solid var(--uv-purple);
      border-radius: 12px;
      overflow: hidden;
    }
    .modal-titulo {
      background: var(--uv-purple);
      color: #ffffff;
      padding: 12px 16px;
      font-weight: bold;
    }
    .modal-texto {
      padding: 12px 16px;
      font-size: 0.9rem;
    }
    .modal .cl-item { font-size: 0.95rem; padding: 12px 16px; }
    .modal-botoes {
      display: flex;
      gap: 10px;
      padding: 12px 16px;
    }
    .modal-botoes button {
      flex: 1;
      padding: 12px;
      border-radius: 8px;
      border: 1px solid var(--border-color);
      background: var(--btn-bg);
      color: var(--text-dark);
      font-size: 0.95rem;
      font-weight: bold;
      cursor: pointer;
    }
    #btn-uv-confirmar {
      background: var(--uv-purple);
      border-color: var(--uv-purple);
      color: #ffffff;
    }
    #btn-uv-confirmar:disabled {
      background: #c4b5fd;
      border-color: #c4b5fd;
      cursor: not-allowed;
    }

    /* =====================================================================
       TELA ÚNICA SEM ROLAGEM
       ===================================================================== */
    @media (min-width: 1000px) and (min-height: 600px) {
      html, body {
        height: 100%;
        overflow: hidden;
      }
      body {
        height: 100vh;
        height: 100dvh;
        min-height: 0;
        padding-bottom: 0;
      }
      .top-bar {
        flex: none;
        margin-bottom: 0;
      }

      .app {
        flex: 1;
        min-height: 0;
        width: 100%;
        max-width: 1400px;
        padding: 12px;
        flex-direction: row;
        gap: 12px;
        overflow: hidden;
      }
      .coluna-esq {
        width: clamp(300px, 28vw, 380px);
        flex: none;
        min-height: 0;
        gap: 12px;
      }
      .coluna-dir {
        flex: 1;
        min-width: 0;
        min-height: 0;
        gap: 12px;
      }

      .card-mov, .card-video {
        flex: 1;
        min-height: 0;
      }
      .card-body {
        padding: 12px;
        min-height: 0;
      }

      .video-container {
        flex: 1;
        min-height: 0;
        aspect-ratio: auto;
      }

      .cl-corpo { flex-direction: row; }
      .cl-itens { grid-template-columns: repeat(3, 1fr); }
      .cl-rodape {
        width: 210px;
        flex: none;
        border-left: 1px solid var(--border-color);
      }
    }

    @media (min-width: 1000px) and (max-height: 760px) {
      #tempo-ativo, #tempo-uv { font-size: 1.6rem; }
    }

    /* Configuracoes de imagem da ESP32-CAM oficial */
    .camera-acao {
      margin-left: auto;
      padding: 5px 10px;
      font: inherit;
      color: #ffffff;
      background: var(--primary-blue);
      border: 0;
      border-radius: 7px;
      cursor: pointer;
      text-transform: none;
      letter-spacing: normal;
    }
    .camera-acao:hover { background: var(--primary-hover); }
    #camera { display: block; object-fit: contain; }
    .modal-camera-caixa {
      width: min(96vw, 680px);
      max-width: 680px;
      max-height: 90vh;
      max-height: 90dvh;
      overflow-y: auto;
      border-color: var(--primary-blue);
    }
    .modal-camera-caixa .modal-titulo {
      background: var(--header-bg);
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 10px;
    }
    .camera-fechar {
      border: 1px solid rgba(255,255,255,.5);
      border-radius: 6px;
      background: transparent;
      color: white;
      padding: 4px 9px;
      cursor: pointer;
      font-size: 1.05rem;
    }
    .camera-status {
      padding: 10px 14px;
      font-size: .85rem;
      color: var(--text-muted);
      background: #f8fafc;
      border-bottom: 1px solid var(--border-color);
    }
    .camera-status.erro { color: #b91c1c; background: #fef2f2; }
    .camera-grade {
      padding: 16px;
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 13px;
    }
    .camera-campo { display: flex; flex-direction: column; gap: 5px; }
    .camera-campo label { font-size: .83rem; font-weight: 600; }
    .camera-campo select, .camera-campo input {
      width: 100%;
      min-height: 38px;
      padding: 6px 9px;
      border: 1px solid var(--border-color);
      border-radius: 7px;
      color: var(--text-dark);
      background: white;
      font-size: .91rem;
    }
    .camera-campo input[type="range"] { padding: 0; min-height: 32px; accent-color: var(--primary-blue); }
    .camera-campo .camera-leitura { color: var(--text-muted); font-weight: normal; }
    .camera-rodape { display: flex; justify-content: flex-end; gap: 8px; padding: 0 16px 16px; }
    .camera-rodape button {
      cursor: pointer;
      padding: 9px 12px;
      border-radius: 7px;
      border: 1px solid var(--border-color);
      background: var(--btn-bg);
      color: var(--text-dark);
    }
    .camera-campo.largura-total { grid-column: 1 / -1; }
    @media (max-width: 560px) {
      .camera-grade { grid-template-columns: 1fr; }
    }
  </style>
</head>
<body oncontextmenu="return false">

  <!-- Barra superior: logo, tempos e status da conexão -->
  <header class="top-bar">
    <span class="brand-icon">&#10010;</span>

    <div class="paineis-tempo">
      <!-- Tempo de funcionamento geral -->
      <div id="painel-tempo">
        <span class="rotulo">Tempo ativo</span>
        <span id="tempo-ativo">00:00:00</span>
      </div>

      <!-- Tempo de desinfecção (UVC) -->
      <div id="painel-uv">
        <span class="rotulo">Tempo UVC</span>
        <span id="tempo-uv">00:00:00</span>
      </div>
    </div>

    <div class="status-badge">
      <span id="status-dot" class="status-dot"></span>
      <span id="status-text">Desconectado</span>
    </div>
  </header>

  <main class="app">

    <!-- Coluna da esquerda: movimento e lâmpadas -->
    <div class="coluna-esq">

      <!-- Cartão de movimento -->
      <div class="card card-mov">
        <div class="card-header">
          <span>Controle de Movimentação</span>
          <span>&#8644;</span>
        </div>
        <div class="card-body">

          <!-- Mensagem de estado -->
          <p id="aviso-controle" class="aviso-controle">Robô desligado.</p>

          <!-- Setas -->
          <div class="dpad">
            <div></div>
            <button class="btn-cmd controle" id="btn-up"    onpointerdown="iniciar('F')" onpointerup="parar()" onpointerleave="parar()" onpointercancel="parar()">&uarr;</button>
            <div></div>

            <button class="btn-cmd controle" id="btn-left"  onpointerdown="iniciar('E')" onpointerup="parar()" onpointerleave="parar()" onpointercancel="parar()">&larr;</button>
            <button class="btn-cmd btn-stop" id="btn-stop-centro" onpointerdown="pararRobo()">&#9632;</button>
            <button class="btn-cmd controle" id="btn-right" onpointerdown="iniciar('D')" onpointerup="parar()" onpointerleave="parar()" onpointercancel="parar()">&rarr;</button>

            <div></div>
            <button class="btn-cmd controle" id="btn-down"  onpointerdown="iniciar('T')" onpointerup="parar()" onpointerleave="parar()" onpointercancel="parar()">&darr;</button>
            <div></div>
          </div>

          <div class="kb-hint">Segure para andar, solte para parar<br>Teclado: WASD ou Setas | Espaço = emergência</div>

          <!-- Barra de velocidade -->
          <div class="speed-section">
            <div class="speed-label">Velocidade: <span id="velTexto">100</span>%</div>
            <input type="range" id="velocidade" min="0" max="100" step="5" value="100"
                   oninput="document.getElementById('velTexto').innerText = this.value"
                   onchange="enviar('V' + this.value)">
          </div>

          <!-- Emergência: Reposicionada para o fim dos controles de movimento -->
          <button class="btn-emergencia" id="btn-stop" onpointerdown="emergencia()">
            <span>&#9632;</span><span>Parada de emergência</span>
          </button>
        </div>
      </div>

      <!-- Cartão das lâmpadas UVC -->
      <div class="card">
        <div class="card-header">
          <span>Desinfecção UVC</span>
          <span>&#128161;</span>
        </div>
        <div class="card-body">
          <div class="lamp-grid">
            <!-- UVC -->
            <button id="btn-uv" class="lamp-btn controle" onclick="alternaUv()">
              <span>Lâmpadas UVC</span>
              <span class="lamp-estado" id="uv-estado">OFF</span>
            </button>
          </div>
          <p class="uv-aviso">Luz UVC é nociva à pele e aos olhos. Só ligue com a área vazia.</p>
        </div>
      </div>
    </div>

    <!-- Coluna da direita: vídeo em cima e checklist embaixo -->
    <div class="coluna-dir">
      <!-- Cartão da câmera -->
      <div class="card card-video">
        <div class="card-header">
          <span>Monitoramento de Vídeo</span>
          <button type="button" class="camera-acao" id="abrir-camera" aria-haspopup="dialog">&#9881; Ajustar câmera</button>
        </div>
        <div class="card-body">
          <div class="video-container">
            <img id="camera" src="http://192.168.4.100:81/stream" alt="Vídeo ao vivo da câmera do RoboLight">
          </div>
        </div>
      </div>

      <!-- Checklist de segurança -->
      <section class="checklist">
        <div class="card-header">
          <span>Checklist de Segurança</span>
          <span>&#9745;</span>
        </div>
        <div class="cl-corpo">
          <div class="cl-itens">
            <label class="cl-item"><input type="checkbox" class="check"> Área livre de pessoas e obstáculos</label>
            <label class="cl-item"><input type="checkbox" class="check"> Bateria conectada e carregada</label>
            <label class="cl-item"><input type="checkbox" class="check"> Fios e pontes H firmes</label>
            <label class="cl-item"><input type="checkbox" class="check"> Rodas livres, sem travar</label>
            <label class="cl-item"><input type="checkbox" class="check"> Operador sabe onde está a emergência</label>
            <label class="cl-item"><input type="checkbox" class="check"> Ninguém na área de ação das UVC</label>
          </div>
          <div class="cl-rodape">
            <p id="contador">0 de 6 itens marcados</p>
            <button id="btn-ligar" disabled>Ligar robô</button>
          </div>
        </div>
      </section>
    </div>
  </main>

  <!-- Janela de confirmação das lâmpadas UVC -->
  <div id="modal-uv" class="modal">
    <div class="modal-caixa">
      <div class="modal-titulo">Ligar lâmpadas UVC</div>
      <p class="modal-texto">A luz UVC causa queimaduras na pele e danos aos olhos. Confirme antes de ligar:</p>
      <label class="cl-item"><input type="checkbox" class="check-uv"> Não há pessoas nem animais na área</label>
      <label class="cl-item"><input type="checkbox" class="check-uv"> Estou fora da linha de visão das lâmpadas</label>
      <div class="modal-botoes">
        <button id="btn-uv-cancelar">Cancelar</button>
        <button id="btn-uv-confirmar" disabled>Ligar UVC</button>
      </div>
    </div>
  </div>


  <!-- Painel de ajustes da ESP32-CAM. Usa a API oficial, sem iframe. -->
  <div id="modal-camera" class="modal" role="dialog" aria-modal="true" aria-labelledby="titulo-camera">
    <div class="modal-caixa modal-camera-caixa">
      <div class="modal-titulo">
        <span id="titulo-camera">Configurações da câmera</span>
        <button type="button" id="fechar-camera" class="camera-fechar" aria-label="Fechar ajustes da câmera">&#10005;</button>
      </div>
      <p id="camera-status" class="camera-status" role="status" aria-live="polite">Consultando a câmera...</p>
      <div class="camera-grade">
        <div class="camera-campo">
          <label for="cam-resolucao">Resolução</label>
          <select id="cam-resolucao" data-camera-var="framesize">
            <option value="1">QQVGA (160 × 120)</option>
            <option value="3">HQVGA (240 × 176)</option>
            <option value="5">QVGA (320 × 240)</option>
            <option value="6">CIF (400 × 296)</option>
            <option value="8">VGA (640 × 480)</option>
            <option value="9">SVGA (800 × 600)</option>
            <option value="10">XGA (1024 × 768)</option>
            <option value="12">SXGA (1280 × 1024)</option>
            <option value="13">UXGA (1600 × 1200)</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-qualidade">Qualidade JPEG <span id="cam-qualidade-valor" class="camera-leitura">20</span></label>
          <input id="cam-qualidade" type="range" min="10" max="40" value="20" step="1" data-camera-var="quality" data-camera-output="cam-qualidade-valor">
        </div>
        <div class="camera-campo">
          <label for="cam-efeito">Filtro de imagem</label>
          <select id="cam-efeito" data-camera-var="special_effect">
            <option value="0">Normal</option>
            <option value="1">Negativo</option>
            <option value="2">Preto e branco</option>
            <option value="3">Tonalidade vermelha</option>
            <option value="4">Tonalidade verde</option>
            <option value="5">Tonalidade azul</option>
            <option value="6">Sépia</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-wbmode">Iluminação / branco</label>
          <select id="cam-wbmode" data-camera-var="wb_mode">
            <option value="0">Automático</option>
            <option value="1">Ensolarado</option>
            <option value="2">Nublado</option>
            <option value="3">Escritório</option>
            <option value="4">Casa</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-brilho">Brilho <span id="cam-brilho-valor" class="camera-leitura">0</span></label>
          <input id="cam-brilho" type="range" min="-2" max="2" step="1" value="0" data-camera-var="brightness" data-camera-output="cam-brilho-valor">
        </div>
        <div class="camera-campo">
          <label for="cam-contraste">Contraste <span id="cam-contraste-valor" class="camera-leitura">0</span></label>
          <input id="cam-contraste" type="range" min="-2" max="2" step="1" value="0" data-camera-var="contrast" data-camera-output="cam-contraste-valor">
        </div>
        <div class="camera-campo">
          <label for="cam-saturacao">Saturação <span id="cam-saturacao-valor" class="camera-leitura">0</span></label>
          <input id="cam-saturacao" type="range" min="-2" max="2" step="1" value="0" data-camera-var="saturation" data-camera-output="cam-saturacao-valor">
        </div>
        <div class="camera-campo">
          <label for="cam-exposicao">Compensação de exposição</label>
          <select id="cam-exposicao" data-camera-var="ae_level">
            <option value="-2">-2</option><option value="-1">-1</option>
            <option value="0">0</option><option value="1">+1</option><option value="2">+2</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-awb">Balanço de branco automático</label>
          <select id="cam-awb" data-camera-var="awb">
            <option value="1">Ligado</option><option value="0">Desligado</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-aec">Exposição automática</label>
          <select id="cam-aec" data-camera-var="aec">
            <option value="1">Ligada</option><option value="0">Desligada</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-agc">Ganho automático</label>
          <select id="cam-agc" data-camera-var="agc">
            <option value="1">Ligado</option><option value="0">Desligado</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-espelho">Espelhamento horizontal</label>
          <select id="cam-espelho" data-camera-var="hmirror">
            <option value="0">Desligado</option><option value="1">Ligado</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-vertical">Inversão vertical</label>
          <select id="cam-vertical" data-camera-var="vflip">
            <option value="0">Desligada</option><option value="1">Ligada</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-lenc">Correção de lente</label>
          <select id="cam-lenc" data-camera-var="lenc">
            <option value="1">Ligada</option><option value="0">Desligada</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-wpc">Correção de pixels brancos</label>
          <select id="cam-wpc" data-camera-var="wpc">
            <option value="1">Ligada</option><option value="0">Desligada</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-bpc">Correção de pixels pretos</label>
          <select id="cam-bpc" data-camera-var="bpc">
            <option value="0">Desligada</option><option value="1">Ligada</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-gamma">Correção gama</label>
          <select id="cam-gamma" data-camera-var="raw_gma">
            <option value="1">Ligada</option><option value="0">Desligada</option>
          </select>
        </div>
        <div class="camera-campo">
          <label for="cam-dcw">Redução de imagem por hardware</label>
          <select id="cam-dcw" data-camera-var="dcw">
            <option value="1">Ligada</option><option value="0">Desligada</option>
          </select>
        </div>
      </div>
      <div class="camera-rodape">
        <button type="button" id="atualizar-camera">Atualizar valores</button>
        <button type="button" id="fechar-camera-rodape">Fechar</button>
      </div>
    </div>
  </div>

  <script>
    var websock;
    var movendo = false;
    var ligado = false;
    var conectado = false;
    var perdeuConexao = false;

    function InitWebSocket() {
      websock = new WebSocket('ws://' + window.location.hostname + ':81/');

      websock.onopen = function () {
        conectado = true;
        document.getElementById('status-dot').classList.add('online');
        document.getElementById('status-text').innerText = 'Conectado';
        atualizarChecklist();
      };

      websock.onmessage = function (evt) {
        var msg = evt.data;

        if (msg == 'UVON') {
          mostrarUv(true);
        } else if (msg == 'UVOFF') {
          mostrarUv(false);
        } else if (msg[0] == 'V') {
          var vel = msg.substring(1);
          document.getElementById('velocidade').value = vel;
          document.getElementById('velTexto').innerText = vel;
        }
      };

      websock.onclose = function () {
        conectado = false;
        document.getElementById('status-dot').classList.remove('online');
        document.getElementById('status-text').innerText = 'Desconectado';
        if (ligado) {
          perdeuConexao = true;
          desligarRobo();
        }
        mostrarUv(false);
        atualizarChecklist();
        setTimeout(InitWebSocket, 2000);
      };
    }

    function enviar(msg) {
      if (websock && websock.readyState == 1) websock.send(msg);
    }

    function iniciar(cmd) {
      if (!ligado) return;
      movendo = true;
      enviar(cmd);
    }

    function parar() {
      if (movendo) {
        movendo = false;
        enviar('P');
      }
    }

    function pararRobo() {
      movendo = false;
      enviar('P');
    }

    function emergencia() {
      movendo = false;
      enviar('X');   // o robô para sem rampa e apaga as UVC
    }

    function mostrarUv(ativo) {
      document.getElementById('uv-estado').textContent = ativo ? 'ON' : 'OFF';
      if (ativo) {
        document.getElementById('btn-uv').classList.add('on');
        iniciarContadorUv();
      } else {
        document.getElementById('btn-uv').classList.remove('on');
        pararContadorUv();
      }
    }

    var modalUv = document.getElementById('modal-uv');
    var checksUv = document.querySelectorAll('.check-uv');
    var btnUvConfirmar = document.getElementById('btn-uv-confirmar');

    function atualizarModalUv() {
      var todos = true;
      checksUv.forEach(function (c) {
        if (!c.checked) todos = false;
      });
      btnUvConfirmar.disabled = !todos;
    }

    checksUv.forEach(function (c) {
      c.addEventListener('change', atualizarModalUv);
    });

    function abrirModalUv() {
      checksUv.forEach(function (c) { c.checked = false; });
      atualizarModalUv();
      modalUv.classList.add('aberto');
    }

    function fecharModalUv() {
      modalUv.classList.remove('aberto');
    }

    document.getElementById('btn-uv-cancelar').addEventListener('click', fecharModalUv);

    btnUvConfirmar.addEventListener('click', function () {
      fecharModalUv();
      enviar('UVON');
    });

    function alternaUv() {
      if (!ligado) return;
      var uvLigado = document.getElementById('btn-uv').classList.contains('on');
      if (uvLigado) {
        enviar('UVOFF');
      } else {
        abrirModalUv();
      }
    }

    var checks = document.querySelectorAll('.check');
    var btnLigar = document.getElementById('btn-ligar');
    var contador = document.getElementById('contador');

    function atualizarAviso() {
      var aviso = document.getElementById('aviso-controle');
      if (ligado) {
        aviso.textContent = 'Robô ligado: comandos liberados.';
        aviso.className = 'aviso-controle ok';
      } else if (perdeuConexao) {
        aviso.textContent = 'Conexão perdida: o robô foi desligado por segurança. Refaça o checklist para ligar de novo.';
        aviso.className = 'aviso-controle erro';
      } else if (!conectado) {
        aviso.textContent = 'Sem conexão com o robô.';
        aviso.className = 'aviso-controle erro';
      } else {
        aviso.textContent = 'Robô desligado: conclua o checklist (abaixo do vídeo) e clique em Ligar robô.';
        aviso.className = 'aviso-controle';
      }
    }

    function atualizarChecklist() {
      var marcados = 0;
      checks.forEach(function (c) {
        if (c.checked) marcados++;
      });

      var texto = marcados + ' de ' + checks.length + ' itens marcados';
      if (!conectado) texto += ' | sem conexão com o robô';
      contador.textContent = texto;

      if (!ligado) {
        btnLigar.disabled = (marcados !== checks.length) || !conectado;
      }
      atualizarAviso();
    }

    checks.forEach(function (c) {
      c.addEventListener('change', atualizarChecklist);
    });

    function liberarControles(liberado) {
      document.querySelectorAll('.controle').forEach(function (b) {
        b.disabled = !liberado;
      });
    }

    var painelTempo = document.getElementById('painel-tempo');
    var tempoAtivo = document.getElementById('tempo-ativo');
    var inicio = 0;
    var intervalo = null;

    function doisDigitos(n) {
      return n < 10 ? '0' + n : String(n);
    }

    function formatarTempo(total) {
      var h = Math.floor(total / 3600);
      var m = Math.floor((total % 3600) / 60);
      var s = total % 60;
      return doisDigitos(h) + ':' + doisDigitos(m) + ':' + doisDigitos(s);
    }

    function atualizarTempo() {
      tempoAtivo.textContent = formatarTempo(Math.floor((Date.now() - inicio) / 1000));
    }

    function iniciarContador() {
      if (intervalo !== null) return;
      inicio = Date.now();
      atualizarTempo();
      intervalo = setInterval(atualizarTempo, 1000);
      painelTempo.classList.add('ativo');
    }

    function pararContador() {
      clearInterval(intervalo);
      intervalo = null;
      painelTempo.classList.remove('ativo');
    }

    var painelUv = document.getElementById('painel-uv');
    var tempoUv = document.getElementById('tempo-uv');
    var inicioUv = 0;
    var intervaloUv = null;

    function atualizarTempoUv() {
      tempoUv.textContent = formatarTempo(Math.floor((Date.now() - inicioUv) / 1000));
    }

    function iniciarContadorUv() {
      if (intervaloUv !== null) return;
      inicioUv = Date.now();
      atualizarTempoUv();
      intervaloUv = setInterval(atualizarTempoUv, 1000);
      painelUv.classList.add('ativo');
    }

    function pararContadorUv() {
      clearInterval(intervaloUv);
      intervaloUv = null;
      painelUv.classList.remove('ativo');
    }

    function ligarRobo() {
      if (!conectado) return;
      ligado = true;
      perdeuConexao = false;
      btnLigar.textContent = 'Desligar robô';
      btnLigar.classList.add('desligar');
      btnLigar.disabled = false;
      checks.forEach(function (c) { c.disabled = true; });
      liberarControles(true);
      iniciarContador();
      atualizarChecklist();
    }

    function desligarRobo() {
      movendo = false;
      enviar('X');
      fecharModalUv();
      ligado = false;
      pararContador();
      btnLigar.textContent = 'Ligar robô';
      btnLigar.classList.remove('desligar');
      liberarControles(false);
      checks.forEach(function (c) {
        c.disabled = false;
        c.checked = false;
      });
      atualizarChecklist();
    }

    btnLigar.addEventListener('click', function () {
      if (ligado) {
        desligarRobo();
      } else {
        ligarRobo();
      }
      btnLigar.blur();
    });

    var teclas = {
      'w': ['F', 'btn-up'],    'arrowup':    ['F', 'btn-up'],
      's': ['T', 'btn-down'],  'arrowdown':  ['T', 'btn-down'],
      'a': ['E', 'btn-left'],  'arrowleft':  ['E', 'btn-left'],
      'd': ['D', 'btn-right'], 'arrowright': ['D', 'btn-right'],
      ' ': ['P', 'btn-stop']
    };

    var teclasPressionadas = [];   // teclas de direção seguradas, na ordem em que foram apertadas

    document.addEventListener('keydown', function (e) {
      if (modalCamera.classList.contains('aberto')) {
        if (e.key === 'Escape') { e.preventDefault(); fecharModalCamera(); }
        else if (e.key === ' ') { e.preventDefault(); emergencia(); }
        return;
      }
      if (e.target.type == 'checkbox') return;
      var k = e.key.toLowerCase();
      var t = teclas[k];
      if (!t || e.repeat) return;
      if (!ligado && t[0] != 'P') return;
      if (modalUv.classList.contains('aberto') && t[0] != 'P') return;
      e.preventDefault();
      if (t[0] == 'P') {
        emergencia();
      } else {
        if (teclasPressionadas.indexOf(k) < 0) teclasPressionadas.push(k);
        iniciar(t[0]);
      }
      document.getElementById(t[1]).classList.add('active-press');
    });

    document.addEventListener('keyup', function (e) {
      if (modalCamera.classList.contains('aberto')) return;
      if (e.target.type == 'checkbox') return;
      var k = e.key.toLowerCase();
      var t = teclas[k];
      if (!t) return;
      document.getElementById(t[1]).classList.remove('active-press');
      if (t[0] == 'P') return;
      var i = teclasPressionadas.indexOf(k);
      if (i >= 0) teclasPressionadas.splice(i, 1);
      if (teclasPressionadas.length == 0) {
        parar();
      } else {
        // ainda há outra tecla segurada: segue na direção dela
        iniciar(teclas[teclasPressionadas[teclasPressionadas.length - 1]][0]);
      }
    });

    // Se a janela perder o foco com tecla segurada, o keyup nunca chega: para o robô
    window.addEventListener('blur', function () {
      teclasPressionadas = [];
      parar();
    });


    // ==============================================================
    // API OFICIAL DA ESP32-CAM (porta 80 da placa secundaria)
    // Nao altera o WebSocket, as UVC ou os comandos do robo.
    // ==============================================================
    var CAMERA_API = 'http://192.168.4.100';
    var modalCamera = document.getElementById('modal-camera');
    var cameraStatus = document.getElementById('camera-status');
    var filaCamera = Promise.resolve();

    function mensagemCamera(texto, erro) {
      cameraStatus.textContent = texto;
      cameraStatus.classList.toggle('erro', !!erro);
    }

    function atualizarValoresCamera(dados) {
      document.querySelectorAll('[data-camera-var]').forEach(function (campo) {
        var nome = campo.getAttribute('data-camera-var');
        if (Object.prototype.hasOwnProperty.call(dados, nome)) {
          campo.value = String(dados[nome]);
          var saida = campo.getAttribute('data-camera-output');
          if (saida) document.getElementById(saida).textContent = campo.value;
        }
      });
    }

    async function lerConfiguracoesCamera() {
      mensagemCamera('Consultando ESP32-CAM...');
      try {
        var resposta = await fetch(CAMERA_API + '/status', { cache: 'no-store' });
        if (!resposta.ok) throw new Error('HTTP ' + resposta.status);
        var dados = await resposta.json();
        atualizarValoresCamera(dados);
        mensagemCamera('Conectada. Ajustes aplicados diretamente ao vídeo.');
      } catch (erro) {
        mensagemCamera('Não foi possível consultar a câmera em 192.168.4.100. Verifique a conexão com a rede RoboLight e o CameraWebServer.', true);
      }
    }

    function alterarCamera(nome, valor) {
      // Evita requisicoes concorrentes quando varios ajustes sao modificados.
      filaCamera = filaCamera.catch(function () {}).then(async function () {
        mensagemCamera('Aplicando ' + nome + '...');
        try {
          var url = CAMERA_API + '/control?var=' + encodeURIComponent(nome) + '&val=' + encodeURIComponent(valor);
          var resposta = await fetch(url, { cache: 'no-store' });
          if (!resposta.ok) throw new Error('HTTP ' + resposta.status);
          mensagemCamera('Ajuste aplicado.');
        } catch (erro) {
          mensagemCamera('Falha ao aplicar ' + nome + '. Confirme que a ESP32-CAM está conectada.', true);
        }
      });
    }

    function abrirModalCamera() {
      // Para o movimento antes de permitir ajustes de imagem.
      pararRobo();
      teclasPressionadas = [];
      modalCamera.classList.add('aberto');
      document.getElementById('fechar-camera').focus();
      lerConfiguracoesCamera();
    }

    function fecharModalCamera() {
      modalCamera.classList.remove('aberto');
      document.getElementById('abrir-camera').focus();
    }

    document.getElementById('abrir-camera').addEventListener('click', abrirModalCamera);
    document.getElementById('fechar-camera').addEventListener('click', fecharModalCamera);
    document.getElementById('fechar-camera-rodape').addEventListener('click', fecharModalCamera);
    document.getElementById('atualizar-camera').addEventListener('click', lerConfiguracoesCamera);
    modalCamera.addEventListener('click', function (e) {
      if (e.target === modalCamera) fecharModalCamera();
    });

    document.querySelectorAll('[data-camera-var]').forEach(function (campo) {
      campo.addEventListener('input', function () {
        var saida = campo.getAttribute('data-camera-output');
        if (saida) document.getElementById(saida).textContent = campo.value;
      });
      // Envia somente ao soltar o slider, evitando sobrecarregar o Wi-Fi.
      campo.addEventListener('change', function () {
        alterarCamera(campo.getAttribute('data-camera-var'), campo.value);
      });
    });

    window.onload = function () {
      InitWebSocket();
      liberarControles(false);
      atualizarChecklist();
    };
  </script>
</body>
</html>
)=====";
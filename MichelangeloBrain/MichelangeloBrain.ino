// Programação para o robô Desvia obstáculos
//Declarações das portas de sensores 
const int TRIG_A = 13, ECHO_A = 12; // esquerda
const int TRIG_B = 8,  ECHO_B = 7;  // frente
const int TRIG_C = 4,  ECHO_C = 2;  // direita

//Declaração de distâncias - TESTES!
const long DIST_LIMITE_FRENTE  = 10;
const long DIST_LIMITE_LATERAL = 5;

//Declaração de motores 
//Motor esquerdo: IN1/IN2 | Motor direito: IN3/IN4 - Adição
const int IN1 = 10, IN2 = 9;
const int IN3 = 6, IN4 = 5;

//Declaração de velocidade PWM, ideia do duty cycle - TESTES!
const int VEL_ANDAR = 180; // ~70% de duty cycle
const int VEL_RE    = 160; // ~63%
const int VEL_GIRO  = 200; // ~78% (giro precisa de mais torque)

//Declarações de tempo de manobra - TESTES!
const unsigned long TEMPO_RE       = 300; // quanto tempo dá ré
const unsigned long TEMPO_CURVA    = 450; // giro de ~90°
const unsigned long TEMPO_CORRECAO = 150; // ajuste pequeno (obstáculo lateral)
const unsigned long TEMPO_PAUSA    = 100; // pausa entre movimentos

//Funções
//Declarar estados
enum Estado {
  ANDANDO,           // lê sensores e decide
  PARADO_ANTES_RE,   // pausa antes de dar ré
  RE,                // dando ré
  PARADO_APOS_RE,    // pausa e decisão do lado do giro
  GIRANDO,           // giro de desvio
  PARADO_APOS_GIRO,  // pausa antes de voltar a andar
  CORRIGINDO         // correção pequena por obstáculo lateral
};

Estado estado = ANDANDO;
unsigned long tempoInicio = 0; // quando o estado atual
unsigned long duracaoGiro = 0; // quanto tempo o giro atual deve durar 

// Função padrão
long lerDistancia(int pinTrig, int pinEcho) {
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);

  long duracao = pulseIn(pinEcho, HIGH, 25000);
  if (duracao == 0) return 999;
  return duracao / 58;
}

void mudarEstado(Estado novo) { 
  estado = novo;
  tempoInicio = millis();
}

bool passou(unsigned long ms) {
  return millis() - tempoInicio >= ms;
}

// Motores - comportamento
void motorEsquerdo(int velocidade) {
  if (velocidade > 0) {
    analogWrite(IN1, velocidade);
    analogWrite(IN2, 0);
  } else if (velocidade < 0) {
    analogWrite(IN1, 0);
    analogWrite(IN2, -velocidade);
  } else {
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
  }
}

void motorDireito(int velocidade) {
  if (velocidade > 0) {
    analogWrite(IN3, velocidade);
    analogWrite(IN4, 0);
  } else if (velocidade < 0) {
    analogWrite(IN3, 0);
    analogWrite(IN4, -velocidade);
  } else {
    analogWrite(IN3, 0);
    analogWrite(IN4, 0);
  }
}

//Movimentos 
void frente() {
  motorEsquerdo(VEL_ANDAR);
  motorDireito(VEL_ANDAR);
}

void re() {
  motorEsquerdo(-VEL_RE);
  motorDireito(-VEL_RE);
}

// Gira no próprio eixo: roda esquerda para trás, direita para frente
void girarEsquerda() {
  motorEsquerdo(-VEL_GIRO);
  motorDireito(VEL_GIRO);
}

// Gira no próprio eixo: roda esquerda para frente, direita para trás
void girarDireita() {
  motorEsquerdo(VEL_GIRO);
  motorDireito(-VEL_GIRO);
}

void parar() {
  motorEsquerdo(0);
  motorDireito(0);
}

//  Estado ANDANDO - Verificar sensores e decisão 
void verificarSensores() {
  double distA = lerDistancia(TRIG_A, ECHO_A);
  double distB = lerDistancia(TRIG_B, ECHO_B);
  double distC = lerDistancia(TRIG_C, ECHO_C);

  bool obstA = (distA <= DIST_LIMITE_LATERAL);
  bool obstB = (distB <= DIST_LIMITE_FRENTE);
  bool obstC = (distC <= DIST_LIMITE_LATERAL);

  // Prioridade 1: obstáculo à frente
  if (obstB) {
    Serial.println("Obstáculo à frente: analisando.");
    parar();
    mudarEstado(PARADO_ANTES_RE);
  }
  // Prioridade 2: corredor estreito -> segue reto
  else if (obstA && obstC) {
    Serial.println("Corredor estreito: seguindo em frente.");
    frente();
  }
  // Prioridade 3: obstáculo à esquerda -> corrige para a direita
  else if (obstA) {
    Serial.println("Obstáculo à esquerda: corrigindo para a direita.");
    girarDireita();
    mudarEstado(CORRIGINDO);
  }
  // Prioridade 4: obstáculo à direita -> corrige para a esquerda
  else if (obstC) {
    Serial.println("Obstáculo à direita: corrigindo para a esquerda.");
    girarEsquerda();
    mudarEstado(CORRIGINDO);
  }
  // Nada por perto
  else {
    frente();
  }
}

// Estado PARADO_APOS_RE: escolher para onde girar após
void decidirGiro() {
  long distA = lerDistancia(TRIG_A, ECHO_A);
  long distC = lerDistancia(TRIG_C, ECHO_C);

  Serial.print("Analisando -> esquerda: ");
  Serial.print(distA);
  Serial.print(" cm | direita: ");
  Serial.print(distC);
  Serial.println(" cm");

  if (distA <= DIST_LIMITE_LATERAL && distC <= DIST_LIMITE_LATERAL) {
    Serial.println("Sem saída! Meia-volta.");
    girarDireita();
    duracaoGiro = TEMPO_CURVA * 2;
  }
  else if (distA > distC) {
    Serial.println("Mais espaço à esquerda -> girando à esquerda.");
    girarEsquerda();
    duracaoGiro = TEMPO_CURVA;
  }
  else {
    Serial.println("Mais espaço à direita -> girando à direita.");
    girarDireita();
    duracaoGiro = TEMPO_CURVA;
  }
  mudarEstado(GIRANDO);
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_A, OUTPUT); pinMode(ECHO_A, INPUT);
  pinMode(TRIG_B, OUTPUT); pinMode(ECHO_B, INPUT);
  pinMode(TRIG_C, OUTPUT); pinMode(ECHO_C, INPUT);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  parar();
  
  Serial.println("Robô iniciado...");
  delay(1000);
  mudarEstado(ANDANDO);
}

void loop() {
  switch (estado) {
        case ANDANDO:
      verificarSensores();
      break;

    case PARADO_ANTES_RE:
      if (passou(TEMPO_PAUSA)) {
        re();
        mudarEstado(RE);
      }
      break;

    case RE:
      if (passou(TEMPO_RE)) {
        parar();
        mudarEstado(PARADO_APOS_RE);
      }
      break;

    case PARADO_APOS_RE:
      if (passou(TEMPO_PAUSA)) {
        decidirGiro(); // já inicia o giro e muda para GIRANDO
      }
      break;

    case GIRANDO:
      if (passou(duracaoGiro)) {
        parar();
        mudarEstado(PARADO_APOS_GIRO);
      }
      break;

    case PARADO_APOS_GIRO:
      if (passou(TEMPO_PAUSA)) {
        mudarEstado(ANDANDO);
      }
      break;

    case CORRIGINDO:
      if (passou(TEMPO_CORRECAO)) {
        frente();
        mudarEstado(ANDANDO);
      }
      break;
  }
}

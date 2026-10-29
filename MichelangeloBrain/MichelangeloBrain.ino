// Programação para o robô Desvia obstáculos
//Declarações das portas de sensores 
const int TRIG_A = 13, ECHO_A = 12; // esquerda
const int TRIG_B = 8,  ECHO_B = 7;  // frente
const int TRIG_C = 4,  ECHO_C = 2;  // direita

//Declaração de distâncias - TESTES 
const double dist_limitB = 10.00; // cm
const double dist_limitLat = 5.00;

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

//Declarar estados
enum Estado {
  ANDANDO,           // lê sensores e decide
  PARADO_ANTES_RE,   // pausa antes de dar ré
  RE,                // dando ré
  PARADO_APOS_RE,    // pausa e decisão do lado do giro
  GIRANDO,           // giro de desvio
  PARADO_APOS_GIRO,  // pausa antes de voltar a andar
  CORRIGINDO         // correção pequena por obstáculo lateral
}

Estado estado = ANDANDO;
unsigned long tempoInicio = 0; // quando o estado atual
const long duracaoGiro = 0; // quanto tempo o giro atual deve durar 

void mudarEstado(Estado novo) { 
  estado = novo;
  tempoInicio = milis();
}

bool passou(unsigned long ms) {
  return milis() - tempoInicio >= ms;
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_A, OUTPUT); pinMode(ECHO_A, INPUT);
  pinMode(TRIG_B, OUTPUT); pinMode(ECHO_B, INPUT);
  pinMode(TRIG_C, OUTPUT); pinMode(ECHO_C, INPUT);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  Serial.println("Servidor iniciado. Monitorando sensores...");
}

void loop() {
  double distA = lerDistancia(TRIG_A, ECHO_A);
  double distB = lerDistancia(TRIG_B, ECHO_B);
  double distC = lerDistancia(TRIG_C, ECHO_C);

  bool obst_detecA = false;
  bool obst_detecB = false;
  bool obst_detecC = false;
  
  // Detecção
  if (distA <= dist_limitLat) {
    Serial.print("Obstáculo detectado com sucesso à esquerda: ");
    Serial.println("-Vira a direita!");
  }

  else if (distB <= dist_limitB) {
    Serial.print("Obstáculo detectado com sucesso à frente: ");
    Serial.println("-Analisa!");
  }

  else if (distC <= dist_limitLat) {
    Serial.print("Obstáculo detectado com sucesso à direita: ");
    Serial.println("-Vira a esquerda!");;
  }
}

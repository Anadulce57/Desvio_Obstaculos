// Programação para o robô Desvia obstáculos
const int TRIG_A = 11, ECHO_A = 10; // esquerda
const int TRIG_B = 9,  ECHO_B = 6;  // frente
const int TRIG_C = 5,  ECHO_C = 3;  // direita

const double dist_limitB = 10.00; // cm
const double dist_limitLat = 5.00;
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

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_A, OUTPUT); pinMode(ECHO_A, INPUT);
  pinMode(TRIG_B, OUTPUT); pinMode(ECHO_B, INPUT);
  pinMode(TRIG_C, OUTPUT); pinMode(ECHO_C, INPUT);

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

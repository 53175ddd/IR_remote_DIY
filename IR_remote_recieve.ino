#define DEBUG true

#define SENSOR_INPUT_PIN     5
#define MAX_DATA_LENGTH    128
#define TIMEOUT_WIDTH    10000

#define NEC_FORMAT_LEADER_LENGTH  16
#define AEHA_FORMAT_LEADER_LENGTH  8
#define SONY_FORMAT_LEADER_LENGTH  4

#define NONE -1
#define NEC  0
#define AEHA 1
#define SONY 2

typedef struct {
  int16_t on;
  int16_t off;
} signal_t;

signal_t raw_data[MAX_DATA_LENGTH];

void setup() {
  /* ピンの入出力設定 */
  pinMode(SENSOR_INPUT_PIN, INPUT);

  /* シリアル通信の初期設定 */
  Serial.begin(115200);
}

void loop() {
  /* 変数の宣言 */
  int8_t type = -1;
  float T_width = 0.0f;
  int16_t step = 0;
  uint32_t leader_low = 0, leader_high = 0, hex_code = 0;
  bool in_loop = true;
  char buffer[64];

  /* 待機メッセージ表示 */
  Serial.print("Waiting data...\n\n");

  /* 信号が来るまで待機 */
  while(digitalRead(SENSOR_INPUT_PIN) == HIGH);  // LOW の信号が来るまで待つ
  get_elapsed_time();

  while(in_loop) {
    while(digitalRead(SENSOR_INPUT_PIN) ==  LOW);  // HIGH の信号が来るまで待つ
    leader_low = get_elapsed_time();
    raw_data[step].on = leader_low;

    if(leader_low > TIMEOUT_WIDTH) {
      in_loop = false;
    }else {
      while(digitalRead(SENSOR_INPUT_PIN) == HIGH);  // LOW の信号が来るまで待つ
      leader_high = get_elapsed_time();
      raw_data[step].off = leader_high;
    }

    step++;
  }

  if(DEBUG) {
    sprintf(buffer, "Leader Low width : %d / High width : %d\n", raw_data[0].on, raw_data[0].off);
    Serial.print(buffer);
  }

  /* 変なデータを受信しているっぽいので削除 */
  raw_data[step - 2].off = 0;
  raw_data[step - 1].on  = 0;

  step -= 2;  // 余分に足した分を差っ引く

  /* 1T の長さを計算 */
  for(int16_t i = 1; i < step; i++) {
    T_width += (float)raw_data[i].on / (float)step;
  }

  float leader_pulse_rate = leader_low / T_width;

  if(DEBUG) {
    Serial.print("1T width : ");
    Serial.print(T_width);
    Serial.print("\nLeader pulse rate : ");
    Serial.print(leader_pulse_rate);
    Serial.print('\n');
  }

  /* 信号種別をレポート */
  if(((NEC_FORMAT_LEADER_LENGTH  / 1.25) < leader_pulse_rate) && (leader_pulse_rate < (NEC_FORMAT_LEADER_LENGTH  * 1.25))) {
    Serial.print("NEC format signal detected\n");
    type = NEC;
  }
  if(((AEHA_FORMAT_LEADER_LENGTH / 1.25) < leader_pulse_rate) && (leader_pulse_rate < (AEHA_FORMAT_LEADER_LENGTH * 1.25))) {
    Serial.print("AEHA format signal detected\n");
    type = AEHA;
  }
  if(((SONY_FORMAT_LEADER_LENGTH / 1.25) < leader_pulse_rate) && (leader_pulse_rate < (SONY_FORMAT_LEADER_LENGTH * 1.25))) {
    Serial.print("SONY format signal detected\n");
    type = SONY;
  }

  if(type == NONE) {
    Serial.print("Unknown signal received\n");
  }

  if(type == NEC) {
    if(DEBUG) {
      for(int16_t i = 0; i < 32; i++) {
        Serial.print(raw_data[i].on);
        Serial.print(' ');
        Serial.print(raw_data[i].off);
        Serial.print('\n');
      }
    }

    Serial.print("code (BIN) : ");
    for(int16_t i = 0; i < 32; i++) {
      if(((i % 8) == 0) && (i != 0)) Serial.print('-');
      Serial.print(((raw_data[i + 1].off / raw_data[i + 1].on) > 1.5) ? 1 : 0);
    }
    Serial.print('\n');

    Serial.print("code (HEX) : 0x");
    for(int16_t i = 0; i < 32; i++) {
      hex_code = (hex_code << 1) + (((raw_data[i + 1].off / raw_data[i + 1].on) > 1.5) ? 1 : 0);
    }
    print_uint32(hex_code);
    Serial.print('\n');  
  }

  Serial.print('\n');
  delay(100);
}

uint32_t get_elapsed_time(void) {
  static uint32_t previous_time = 0;  // 前回の時間を保持
  uint32_t current_time = micros();   // 現在の時間を取得
  uint32_t elapsed_time = current_time - previous_time; // 経過時間を計算
  previous_time = current_time;       // 前回時間を更新
  return elapsed_time; 
}

void print_uint32(uint32_t code) {
  for(int8_t i = 7; i >= 0; i--) {
    uint8_t digit = (code >> (i * 4)) & 0xF;
    Serial.print(digit, HEX);
  }
}

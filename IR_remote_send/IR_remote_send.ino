#define IR_OUT_PIN 6
#define SW_ON  3
#define SW_OFF 2

#define CARRIER_FREQ 38000

#define NEC_T_LENGTH 580

#define NEC_LEADER_ON_LENGTH  (NEC_T_LENGTH * 16)
#define NEC_LEADER_OFF_LENGTH (NEC_T_LENGTH *  8)
#define NEC_DATA_ON_LENGTH    (NEC_T_LENGTH *  1)
#define NEC_DATA_0_OFF_LENGTH (NEC_T_LENGTH *  1)
#define NEC_DATA_1_OFF_LENGTH (NEC_T_LENGTH *  3)

#define NEC  0
#define AEHA 1
#define SONY 2

uint32_t on_signal  = 0x01A359A6;
uint32_t off_signal = 0x01A310EF;

void setup() {
  pinMode(IR_OUT_PIN, OUTPUT);

  pinMode(SW_ON , INPUT_PULLUP);
  pinMode(SW_OFF, INPUT_PULLUP);

  Serial.begin(115200);
}

void loop() {
  static uint8_t sw_on_status = 0, sw_off_status = 0;

  sw_on_status  = (sw_on_status  << 1) + ((digitalRead(SW_ON ) == HIGH) ? 0 : 1);
  sw_off_status = (sw_off_status << 1) + ((digitalRead(SW_OFF) == HIGH) ? 0 : 1);

  if((sw_on_status & 0b11) == 0b01) {
    Serial.print("sending Light ON signal\n");

    send_leader(NEC);
    for(int8_t i = 31; i >= 0; i--) {
      uint16_t wait = (on_signal & (1 << i)) == 0 ? NEC_DATA_0_OFF_LENGTH : NEC_DATA_1_OFF_LENGTH;
      tone(IR_OUT_PIN, CARRIER_FREQ);
      delayMicroseconds(NEC_DATA_ON_LENGTH);
      noTone(IR_OUT_PIN);
      delayMicroseconds(wait);
    }
    send_trailer(NEC);
  }

  if((sw_off_status & 0b11) == 0b01) {
    Serial.print("sending Light OFF signal\n");

    send_leader(NEC);
    for(int8_t i = 31; i >= 0; i--) {
      uint16_t wait = (off_signal & (1 << i)) == 0 ? NEC_DATA_0_OFF_LENGTH : NEC_DATA_1_OFF_LENGTH;
      tone(IR_OUT_PIN, CARRIER_FREQ);
      delayMicroseconds(NEC_DATA_ON_LENGTH);
      noTone(IR_OUT_PIN);
      delayMicroseconds(wait);
    }
    send_trailer(NEC);
  }

  delay(10);
}

void send_leader(int8_t type) {
  switch(type) {
    case NEC:
      tone(IR_OUT_PIN, CARRIER_FREQ);
      delayMicroseconds(NEC_LEADER_ON_LENGTH);
      noTone(IR_OUT_PIN);
      delayMicroseconds(NEC_LEADER_OFF_LENGTH);
      break;
  }
}

void send_trailer(int8_t type) {
  switch(type) {
    case NEC:
      tone(IR_OUT_PIN, CARRIER_FREQ);
      delayMicroseconds(NEC_T_LENGTH);
      noTone(IR_OUT_PIN);
      break;
  }
}

const int shift_data = 11;   //SER/DS - dados do shift register 74HC595
const int shift_latch = 10;  //RCLK / ST_CP latch do shift register 74HC595
const int shift_clk = 13;    //SRCLK / SH_CP - clock do shift register 74HC595
const int pin_we = A0;       //Write Enable (#WE) = sinal de escrita para EEPROM
const int pin_ce = A1;       //Chip Enable (#CE)  = sinal para ativar eprom
const int pin_oe = A3;       //Output Enable (#OE) = configura IO do barramento de dados

const int pinSequence[] = { 8, 9, 2, 3, 4, 5, 6, 7 };  //Sequencia dos pinos de dados da eeprom: D0, D1, D2...
const unsigned long eeprom_size = 262144;              //512*1024
const int sector_size = 256;

void setAddress(unsigned long address);                               //função para seleção do endereço
void setDataBus(int mode);                                            //função para setar barramento de dados como entrada/saída
byte readEEPROM(unsigned long address);                                         //função para leitura de um byte
byte writeByte(unsigned long address, byte dataByte);                           //função para escrever um byte
void fillBlock(unsigned long address, byte dataByte);                           //função para preencher um bloco com um byte
void writeBlock(unsigned long address, byte dataArray[sector_size]);  //função para escrever um bloco
void dataPolling(byte dataByte);                                      //função de polling de dados
void printRawData();                                                  //função para imprimir dados brutos sem formatação
void printData();                                                     //função para imprimir os dados formatados no monitor serial
void eraseEEPROM();                                                   //função para apagar a EEPROM


const byte testData[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
  0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
  0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
  0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
  0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
  0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

void setup() {

  pinMode(shift_latch, OUTPUT);  //saída para latch
  pinMode(shift_data, OUTPUT);   //saída para dados
  pinMode(shift_clk, OUTPUT);    //saída para clock

  digitalWrite(pin_we, HIGH);  //desativa a escrita
  pinMode(pin_we, OUTPUT);
  pinMode(pin_oe, OUTPUT);
  pinMode(pin_ce, OUTPUT);

  Serial.begin(250000);  //inicializa Serial em 250000 bits por segundo

  //writeByte(0x100,0x11);
  // fillBlock(0x00,0x99);
  // writeBlock(0x100,testData);

  // printData();  //imprime o conteúdo da EEPROM no monitor serial
}


void loop() {
  while (!Serial.available());
  char cmd = Serial.read();

  switch (cmd) {
    case 'd':
      printRawData();
      break;

    case 'r':
      printData();
      break;

    case 'w':

      byte block[sector_size];
      unsigned long receivedBytes = 0;
      unsigned long offset = 0;

      while (offset <= eeprom_size) {
        // Espera receber os 256 bytes do bloco
        while (receivedBytes < sector_size) {
          while (!Serial.available());
          block[receivedBytes] = Serial.read();
          receivedBytes++;
        }

        // Processa o bloco recebido
        writeBlock(offset, block);
        receivedBytes = 0;
        offset += 256;
        // Serial.write('@');
        Serial.println(offset);
      }

      break;
  }
}

void setAddress(unsigned long address) {
  shiftOut(shift_data, shift_clk, MSBFIRST, (address >> 16) & 0xFF);
  shiftOut(shift_data, shift_clk, MSBFIRST, (address >> 8) & 0xFF);
  shiftOut(shift_data, shift_clk, MSBFIRST, address & 0xFF);

  //gera pulso de latch para escrever dados nas saídas dos shift registers
  digitalWrite(shift_latch, LOW);
  digitalWrite(shift_latch, HIGH);
  digitalWrite(shift_latch, LOW);
}

void setDataBus(int mode) {
  for (int i = 0; i < 8; i++) {
    int pin = pinSequence[i];
    pinMode(pin, mode);
  }
}

void dataPolling(byte dataByte) {

  setDataBus(INPUT);  //configura pinos de dados como entrada

  delayMicroseconds(1);
  byte b1 = 0, b2 = 0;

  for (unsigned readCount = 1; (readCount < 1000); readCount++) {
    digitalWrite(pin_oe, LOW);  //habilita leitura do barramento de dados
    delayMicroseconds(1);
    //realiza a leitura dos dados
    for (int i = 7; i >= 0; i--) {
      int pin = pinSequence[i];
      b1 = (b1 << 1) + digitalRead(pin);
    }
    digitalWrite(pin_oe, HIGH);  //desabilita leitura do barramento de dados
    digitalWrite(pin_ce, HIGH);  //desativa a eeprom
    digitalWrite(pin_ce, LOW);   //ativa a eeprom
    digitalWrite(pin_oe, LOW);   //habilita leitura do barramento de dados
    delayMicroseconds(1);
    //realiza a leitura dos dados
    for (int i = 7; i >= 0; i--) {
      int pin = pinSequence[i];
      b2 = (b2 << 1) + digitalRead(pin);
    }
    digitalWrite(pin_oe, HIGH);  //desabilita leitura do barramento de dados
    digitalWrite(pin_ce, HIGH);  //desativa a eeprom
    if ((b1 == b2) && (b1 == dataByte)) {
      return true;
    }
  }
}

byte readEEPROM(unsigned long address) {

  setDataBus(INPUT);  //configura pinos de dados como entrada

  digitalWrite(pin_we, HIGH);  //desativa a escrita
  digitalWrite(pin_ce, LOW);   //ativa a eeprom
  digitalWrite(pin_oe, LOW);   //habilita leitura do barramento de dados

  setAddress(address);  //seleciona endereço para leitura

  //realiza a leitura dos dados
  byte dataByte = 0;  //variável local para armazenar dados

  for (int i = 7; i >= 0; i--) {
    int pin = pinSequence[i];
    dataByte = (dataByte << 1) + digitalRead(pin);
  }

  digitalWrite(pin_oe, HIGH);
  digitalWrite(pin_ce, HIGH);  //desativa a eeprom

  return dataByte;  //retorna o dado lido
}

byte writeByte(unsigned long address, byte dataByte) {

  setDataBus(OUTPUT);  //configura pinos de dados como saída

  digitalWrite(pin_oe, HIGH);  //desativa entrada no barramento de dados
  digitalWrite(pin_we, HIGH);  //desativa escrita
  digitalWrite(pin_ce, LOW);   //ativa a eeprom

  setAddress(address);  // Configura o endereço nos pinos de endereço

  //Latch dos dados
  for (int i = 0; i < 8; i++) {
    int pin = pinSequence[i];
    digitalWrite(pin, dataByte & 1);
    dataByte = dataByte >> 1;
  }

  delayMicroseconds(1);
  digitalWrite(pin_we, LOW);
  delayMicroseconds(1);
  digitalWrite(pin_we, HIGH);

  dataPolling(dataByte);  //verifica fim do ciclo de escrita em memórias flash
}

void fillBlock(unsigned long address, byte dataByte) {

  setDataBus(OUTPUT);  //configura pinos de dados como saída

  digitalWrite(pin_oe, HIGH);  //desativa entrada no barramento de dados
  digitalWrite(pin_we, HIGH);  //desativa escrita
  digitalWrite(pin_ce, LOW);   //ativa a eeprom

  for (unsigned long ix = 0; ix < sector_size; ix++) {
    setAddress(address + ix);  // Configura o endereço nos pinos de endereço

    //Latch dos dados
    byte byteValue = dataByte;
    for (int i = 0; i < 8; i++) {
      int pin = pinSequence[i];
      digitalWrite(pin, byteValue & 1);
      byteValue = byteValue >> 1;
    }

    delayMicroseconds(1);
    digitalWrite(pin_we, LOW);
    delayMicroseconds(1);
    digitalWrite(pin_we, HIGH);
  }

  dataPolling(dataByte);  //verifica fim do ciclo de escrita em memórias flash
}

void writeBlock(unsigned long address, byte dataArray[sector_size]) {

  setDataBus(OUTPUT);  //configura pinos de dados como saída

  digitalWrite(pin_oe, HIGH);  //desativa entrada no barramento de dados
  digitalWrite(pin_we, HIGH);  //desativa escrita
  digitalWrite(pin_ce, LOW);   //ativa a eeprom

  for (unsigned long ix = 0; ix < sector_size; ix++) {

    setAddress(address + ix);  // Configura o endereço nos pinos de endereço

    //Latch dos dados
    for (int i = 0; i < 8; i++) {
      int pin = pinSequence[i];
      digitalWrite(pin, dataArray[ix] & 1);
      dataArray[ix] = dataArray[ix] >> 1;
    }

    delayMicroseconds(1);
    digitalWrite(pin_we, LOW);
    delayMicroseconds(1);
    digitalWrite(pin_we, HIGH);
  }

  dataPolling(dataArray[sector_size - 1]);  //verifica fim do ciclo de escrita em memórias flash
}

void printRawData() {
  for (unsigned long offset = 0; offset <= eeprom_size; offset++) {
    Serial.write(readEEPROM(offset));
  }
}

void printData() {
  //imprime os primeiros N endereços de dados
  for (unsigned long base = 0; base <= eeprom_size; base += 16) {
    byte data[16];

    for (unsigned long offset = 0; offset <= 15; offset++) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x: %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}

void eraseEEPROM() {
}

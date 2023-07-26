const int shift_data = 11;   //SER/DS - dados do shift register 74HC595
const int shift_latch = 10;  //RCLK / ST_CP latch do shift register 74HC595
const int shift_clk = 13;    //SRCLK / SH_CP - clock do shift register 74HC595
const int pin_we = A0;     //Write Enable (#WE) = sinal de escrita para EEPROM
const int pin_ce = A1;    //Chip Enable (#CE)  = sinal para ativar eprom 
const int pin_oe = A3;    //Output Enable (#OE) = configura IO do barramento de dados

int pinSequence[] = { 8, 9, 2, 3, 4, 5, 6, 7 };  //Sequencia dos pinos de dados da eeprom: D0, D1, D2...
const int eeprom_size = 512; //*1024

void setAddress(int address);              //função para seleção do endereço
byte readEEPROM(int address);              //função para leitura da EEPROM
byte writeEEPROM(int address, byte data);  //função para escrita da EEPROM
void printData();                          //função para imprimir os dados no monitor serial
void eraseEEPROM();                        //função para apagar EEPROM


const byte data[] = {
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

int data_length = sizeof(data) / sizeof(data[0]);  //tamanho do vetor de dados, quantidade de bytes

void setup() {
  
  pinMode(shift_latch, OUTPUT);  //saída para latch
  pinMode(shift_data, OUTPUT);   //saída para dados
  pinMode(shift_clk, OUTPUT);    //saída para clock

  digitalWrite(pin_we, HIGH);   //desativa a escrita
  pinMode(pin_we, OUTPUT);  
  pinMode(pin_oe, OUTPUT);
  pinMode(pin_ce, OUTPUT);

  Serial.begin(250000);  //inicializa Serial em 250000 bits por segundo

  //eraseEEPROM();                  //apaga EEPROM
  
  // writeEEPROM(0x555, 0xAA);
  // writeEEPROM(0x2AA, 0x55);
  // writeEEPROM(0x555, 0xA0);

  for (int address = 0; address <= data_length; address++) {    //escreve nos endereços da EEPROM
    writeEEPROM(address, data[address]);
  }

  printData();  //imprime o conteúdo da EEPROM no monitor serial

}


void loop() {

} 

void setAddress(int address) {
  shiftOut(shift_data, shift_clk, MSBFIRST, address >> 16);
  shiftOut(shift_data, shift_clk, MSBFIRST, address >> 8);
  shiftOut(shift_data, shift_clk, MSBFIRST, address);

  //gera pulso de latch para escrever dados nas saídas dos shift registers
  digitalWrite(shift_latch, LOW);
  digitalWrite(shift_latch, HIGH);
  digitalWrite(shift_latch, LOW);

}  


byte readEEPROM(int address) {
  //configura pinos de dados como entrada
  for (int i = 0; i < 8; i++) {
    int pin = pinSequence[i];
    pinMode(pin, INPUT);
  }

  digitalWrite(pin_we, HIGH);   //desativa a escrita
  digitalWrite(pin_ce, LOW);   //ativa a eeprom  
  digitalWrite(pin_oe, LOW);    //habilita leitura do barramento de dados

  setAddress(address);  //seleciona endereço para leitura


  byte data = 0;  //variável local para armazenar dados

  //realiza a leitura dos dados
  //for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin--)
  for (int i = 7; i >= 0; i--) {
    int pin = pinSequence[i];
    data = (data << 1) + digitalRead(pin);
  }

  digitalWrite(pin_oe, HIGH);
  digitalWrite(pin_ce, HIGH);   //desativa a eeprom  

  return data;  //retorna o dado lido

}


byte writeEEPROM(int address, byte data) {
  //configura os pinos de dados como saída
  for (int i = 0; i < 8; i++) {
    int pin = pinSequence[i];
    pinMode(pin, OUTPUT);
  }

  digitalWrite(pin_ce, LOW);   //ativa a eeprom  
  digitalWrite(pin_oe, HIGH);   //desativa entrada no barramento de dados
  digitalWrite(pin_we, HIGH);   //desativa escrita

  // Configura o endereço nos pinos de endereço
  setAddress(address);  

  //Latch dos dados
  for (int i = 0; i < 8; i++) {
    int pin = pinSequence[i];
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }

  digitalWrite(pin_we, LOW);
  delayMicroseconds(1);
  digitalWrite(pin_we, HIGH);   
  
  digitalWrite(pin_ce, HIGH);   //desativa a eeprom  

  //delay(10); //tempo de escrita máximo 10ms. não funciona para flash.
  //data polling (verifica fim do ciclo de escrita em memórias flash)
  // readEEPROM(address);


  
}  


void printData() {
  //imprime os primeiros x endereços de dados
  for (int base = 0; base <= eeprom_size; base += 16) {
    byte data[16];

    for (int offset = 0; offset <= 15; offset++) {
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

  for (int address = 0; address <= eeprom_size; address++) {  //apaga EEPROM escrevendo FFh em
    writeEEPROM(address, 0xFF);                               //todos os endereços
  }

}


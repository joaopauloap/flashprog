import serial
import tkinter as tk
from tkinter import filedialog
from serial.tools.list_ports import comports
import time
import os
import sys

BAUD_RATE = 250000
EEPROM_SIZE = 65536;
SECTOR_SIZE = 256;

def listar_portas_seriais():
    return [port.device for port in comports()]

def procurar_arquivo():
    root = tk.Tk()
    root.withdraw()
    arquivo = filedialog.askopenfilename()
    return arquivo

def menu_opcoes():
    print("\nOpções:")
    print("d: Realizar dump de dados")
    print("r: Realizar leitura de dados")
    print("w: Realizar escrita de dados")
    print("q: Sair")

    escolha = input("Informe o número da opção desejada: ")
    return escolha

def realizar_dump_dados(porta_serial):
    try:
        conexao_serial = serial.Serial(porta_serial, baudrate=BAUD_RATE, bytesize=8, parity='N', stopbits=1, timeout=300)
        time.sleep(3)
        print("Conexão serial estabelecida. Aguarde...")
        print("Imprimindo os primeiros 2048 bytes...")

        # # Envia um comando para solicitar os dados ao dispositivo
        conexao_serial.write(b'd')

        # Leitura da resposta do dispositivo
        while True:
            data = conexao_serial.read(2048) # Leitura dos dados da porta serial
            if(data):
                  for i in range(0, len(data), 16):
                    chunk = data[i:i + 16]
                    hex_string = ' '.join(['{:02X}'.format(byte) for byte in chunk])
                    print('{:08X}: {}'.format(i, hex_string))
            break 
    
    except serial.SerialException as e:
        print(f"Erro na comunicação serial: {e}")

def realizar_leitura_dados(porta_serial):
    try:
        conexao_serial = serial.Serial(porta_serial, baudrate=BAUD_RATE, bytesize=8, parity='N', stopbits=1, timeout=300)
        time.sleep(3)
        print("Conexão serial estabelecida. Aguarde...")

        # # Envia um comando para solicitar os dados ao dispositivo
        conexao_serial.write(b'r')

        # Leitura da resposta do dispositivo
        while True:
            data = conexao_serial.read(EEPROM_SIZE) # Leitura dos dados da porta serial
            if(data):
                nome_arquivo = "dump.bin"
                with open(nome_arquivo, "wb") as arquivo:
                    arquivo.write(data)
            break 
        
    except serial.SerialException as e:
        print(f"Erro na comunicação serial: {e}")

def realizar_escrita_dados(porta_serial, nome_arquivo):
    try:
        with open(nome_arquivo, 'rb') as arquivo:
            conexao_serial = serial.Serial(porta_serial, baudrate=BAUD_RATE, bytesize=8, parity='N', stopbits=1, timeout=500)
            time.sleep(3)
            print("Conexão serial estabelecida. Aguarde...")
            
            tamanho = os.path.getsize(arquivo.name)
            byteCount = 0

            if tamanho>EEPROM_SIZE:
                print("\nArquivo maior que a capacidade da EEPROM.")
                return

            # # Envia um comando para solicitar os dados ao dispositivo
            conexao_serial.write(b'w')
            
            while byteCount<tamanho:
                bloco = arquivo.read(SECTOR_SIZE)
                if not bloco:
                    break

                conexao_serial.write(bloco)

                # Aguarda receber o caractere de controle antes de enviar o bloco
                while True:
                        resposta = conexao_serial.readline().decode('utf-8').strip()
                        byteCount+=SECTOR_SIZE
                        sys.stdout.write(f"\r{resposta} / {tamanho} bytes")
                        sys.stdout.flush()
                        break

            
            conexao_serial.close()
            print("Escrita de dados realizada com sucesso.")
    
    except FileNotFoundError:
        print(f"O arquivo '{nome_arquivo}' não foi encontrado.")
    except serial.SerialException as e:
        print(f"Erro na comunicação serial: {e}")

def main():
    portas_seriais = listar_portas_seriais()

    if not portas_seriais:
        print("Nenhuma porta serial disponível.")
        return

    print("Portas seriais disponíveis:")
    for i, porta in enumerate(portas_seriais, 1):
        print(f"{i}. {porta}")

    escolha_porta = int(input("Escolha o número da porta serial que deseja utilizar: "))

    if escolha_porta < 1 or escolha_porta > len(portas_seriais):
        print("Escolha inválida.")
        return

    porta_serial = portas_seriais[escolha_porta - 1]
    print(f"Porta serial selecionada: {porta_serial}")

    while True:
        opcao = menu_opcoes()
        if opcao == "d":
            realizar_dump_dados(porta_serial)
        elif opcao == "r":
            realizar_leitura_dados(porta_serial)
        elif opcao == "w":
            nome_arquivo = procurar_arquivo()
            if nome_arquivo:
                realizar_escrita_dados(porta_serial, nome_arquivo)
        elif opcao =="q":
            quit()
        else:
            print("Opção inválida.")

if __name__ == "__main__":
    main()

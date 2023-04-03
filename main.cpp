#include "mbed.h"
#include <cstdint>

Thread thread_slave;

//master

SPI spi(D11, D12, D13); // mosi, miso, sclk
DigitalOut cs(D9);

SPISlave device(PD_4, PD_3, PD_1, PD_0); //mosi, miso, sclk, cs; PMOD pins

DigitalOut led(LED3);

int commandGenerate(bool rw, uint8_t addr)
{
    uint32_t cmd;
    cmd = addr;
    cmd += rw << 8;
    return cmd;
}

int slave()
{
    uint16_t storedValue[256];
    device.format(16, 3);
    device.frequency(1000000);
    //device.reply(0x00); // Prime SPI with first reply
    while (true){
        if (device.receive()){
            int cmd = device.read(); // Read byte from master
            bool rw = cmd >> 8;
            uint8_t addr = cmd & 0xff;
            uint32_t value;
            if (rw == 1){
                value = device.read();
                storedValue[addr] = value;
                printf("Device: Store 0x%0x in address 0x%0x.\n", storedValue[addr], addr);
            }else{
                printf("Device: Reply 0x%0x from address %0x.\n", storedValue[addr], addr);
                device.reply(storedValue[addr]);
                device.read();
                led = !led;
            }
        }
    }
}

int main()
{
    thread_slave.start(slave);

    int cmd;
    int response;
    uint8_t addr;
    uint16_t value;
    spi.format(16, 3);
    spi.frequency(1000000);

    cs = 1;

    // Write 0xabcd into address 0x1f
    cs = 0;
    addr = 0x1f;
    printf("Main: Write a value in address 0x%0x. \n", addr);
    cmd = commandGenerate(1, addr);
    spi.write(cmd);
    ThisThread::sleep_for(100ms);
    value = 0xabcd;
    printf("Main: The value is 0x%0x. \n", value);
    spi.write(value);
    ThisThread::sleep_for(100ms);
    cs = 1;
    printf("\n");

    // Write 0x34d into 0x43
    cs = 0;
    addr = 0x43;
    printf("Main: Write a value in address 0x%0x. \n", addr);
    cmd = commandGenerate(1, addr);
    spi.write(cmd);
    ThisThread::sleep_for(100ms);
    value = 0x34d;
    printf("Main: The value is 0x%0x. \n", value);
    spi.write(value);
    ThisThread::sleep_for(100ms);
    cs = 1;
    printf("\n");
    
    // Receive 0xabcd from 0x1f
    cs = 0;
    addr = 0x1f;
    printf("Main: Read from address 0x%0x. \n", addr);
    cmd = commandGenerate(0, addr);
    spi.write(cmd);
    ThisThread::sleep_for(100ms);
    response = spi.write(0);
    ThisThread::sleep_for(100ms);
    printf("Main: Receive 0x%0x. \n", response);
    cs = 1;
    printf("\n");

    // Receive 0x34d from 0x43
    cs = 0;
    addr = 0x43;
    printf("Main: Read from address 0x%0x. \n", addr);
    cmd = commandGenerate(0, addr);
    spi.write(cmd);
    ThisThread::sleep_for(100ms);
    response = spi.write(0);
    ThisThread::sleep_for(100ms);
    printf("Main: Receive 0x%0x. \n", response);
    cs = 1;
    printf("\n");

    // Write 0x65ed into 0x43
    cs = 0;
    addr = 0x43;
    printf("Main: Write a value in address 0x%0x. \n", addr);
    cmd = commandGenerate(1, addr);
    spi.write(cmd);
    ThisThread::sleep_for(100ms);
    value = 0x65ed;
    printf("Main: The value is 0x%0x. \n", value);
    spi.write(value);
    ThisThread::sleep_for(100ms);
    cs = 1;
    printf("\n");

    // Receive 0x65ed from 0x43
    cs = 0;
    addr = 0x43;
    printf("Main: Read from address 0x%0x. \n", addr);
    cmd = commandGenerate(0, addr);
    spi.write(cmd);
    ThisThread::sleep_for(100ms);
    response = spi.write(0);
    ThisThread::sleep_for(100ms);
    printf("Main: Receive 0x%0x. \n", response);
    cs = 1;
    printf("\n");
}
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
                printf("Device: Store %0x in address %0x.\n", storedValue[addr], addr);
            }else{
                printf("Device: Reply %0x in address %0x.\n", storedValue[addr], addr);
                device.reply(storedValue[addr]);
                device.read();
                led = !led;
            }

            /*if (v == 0xAA)
            {                      //Verify the command
            v = device.read(); // Read another byte from master
            printf("Second Read from master: v = %d\n", v);
            v = v + 10;
            device.reply(v); // Make this the next reply
            v = device.read(); // Read again to allow master read back
            led = !led;      // led turn blue/orange if device receive
            }
            else
            {
            printf("Default reply to master: 0x00\n");
            device.reply(0x00); //Reply default value
            };*/
        }
    }
}

int main()
{
    thread_slave.start(slave);

    int cmd;
    int response;
    spi.format(16, 3);
    spi.frequency(1000000);

    cs = 1;
    cs = 0;
    printf("Main: Write a value in address 1f. \n");
    cmd = commandGenerate(1, 0x1f);
    spi.write(cmd);
    ThisThread::sleep_for(100ms);
    printf("Main: value = abcd. \n");
    spi.write(0xabcd);
    ThisThread::sleep_for(100ms);
    cs = 1;

    cs = 0;
    printf("Main: Read from address 1f. \n");
    cmd = commandGenerate(0, 0x1f);
    spi.write(cmd);
    ThisThread::sleep_for(100ms);
    response = spi.write(0);
    ThisThread::sleep_for(100ms);
    printf("Main: Receive %0x. \n", response);

    /*
    // Setup the spi for 9 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    spi.format(, 3);
    spi.frequency(1000000);

    cs = 1;
    cs = 0;

    for(int i=0; i<5; ++i){ //Run for 5 times
        // Chip must be deselected
        cs = 1;
        // Select the device by seting chip select low
        cs = 0;

        printf("Send handshaking codes.\n");

        int response = spi.write(0xAA); //Send ID
        cs = 1;                       // Deselect the device
        ThisThread::sleep_for(100ms); //Wait for debug print
        printf("First response from slave = %d\n", response);

        // Select the device by seting chip select low
        cs = 0;
        printf("Send number = %d\n", number);

        spi.write(number); //Send number to slave
        ThisThread::sleep_for(100ms); //Wait for debug print
        response = spi.write(number); //Read slave reply
        ThisThread::sleep_for(100ms); //Wait for debug print
        printf("Second response from slave = %d\n", response);
        cs = 1; // Deselect the device
        number += 1;
    }*/
}
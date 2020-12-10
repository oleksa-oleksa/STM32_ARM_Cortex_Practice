# How to use Putty to get log output

Run the follwing to open the terminal:
```bash
sudo putty /dev/ttyUSB0 -serial -sercfg <baud-rate>,8,n,1,N
```

The exercise sheet 4 shows how USART2 can be configured:
From usart2_init.c:
baud rate: 921600 
word length: 8
flow control: None

Therefore the following command opens a terminal that shows the log output correctly:

```bash
sudo putty /dev/ttyUSB0 -serial -sercfg 921600,8,n,1,N
```
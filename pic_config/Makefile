
pic_config: pic_config.c
	sdcc pic_config.c --use-non-free -mpic14  -p12f629 
	./hex2bin/hex2bin -r 2046 0 pic_config.hex pic_config.code.bin
	./hex2bin/hex2bin -r 2 0x400E pic_config.hex pic_config.config.bin

flash: pic_config.hex
	minipro -p PIC12F629 -c code -w pic_config.code.bin
	minipro -p PIC12F629 -e -c config -w pic_config.config.bin


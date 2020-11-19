OBJS += MPP/CC3100/CC3100_Board.o
OBJS += MPP/CC3100/CC3100.o
OBJS += MPP/CC3100/CC3100_EventHandler.o
OBJS += MPP/CC3100/CC3100_HttpServer.o
-include MPP/CC3100/*.d
include MPP/CC3100/simplelink/source/targets.mk

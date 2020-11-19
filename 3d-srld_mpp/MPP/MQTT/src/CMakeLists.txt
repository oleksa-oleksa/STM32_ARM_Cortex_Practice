target_sources(STM32F415RG.elf PRIVATE 
    MQTTConnectClient.c 
    MQTTConnectServer.c 
    MQTTDeserializePublish.c 
    MQTTFormat.c 
    MQTTPacket.c 
    MQTTSerializePublish.c 
    MQTTSubscribeClient.c 
    MQTTSubscribeServer.c 
    MQTTUnsubscribeClient.c 
    MQTTUnsubscribeServer.c)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE ../inc)

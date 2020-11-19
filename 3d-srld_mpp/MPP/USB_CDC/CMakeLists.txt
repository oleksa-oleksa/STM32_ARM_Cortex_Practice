target_sources(STM32F415RG.elf PRIVATE 
    usb_bsp.c 
    usb_core.c 
    usb_dcd.c 
    usbd_cdc_core.c 
    usbd_cdc_vcp.c 
    usb_dcd_int.c 
    usbd_core.c 
    usbd_desc.c 
    usbd_ioreq.c 
    usbd_req.c 
    usbd_usr.c)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE .)

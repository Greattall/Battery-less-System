#include <BackupRestoreHandler.h>

void Backup_or_Restore ( void )
{
    if ( function_switch == 'b' )
    {
        memcpy(&FRAM_buffer , ((void*) 0x1C00) , 0x2000);

        asm(" MOVA R1, &0xFE84");
        asm(" MOVA R2, &0xFE88");
        asm(" MOVA R3, &0xFE8C");
        asm(" MOVA R4, &0xFE90");
        asm(" MOVA R5, &0xFE94");
        asm(" MOVA R6, &0xFE98");
        asm(" MOVA R7, &0xFE9C");
        asm(" MOVA R8, &0xFEA0");
        asm(" MOVA R9, &0xFEA4");
        asm(" MOVA R10, &0xFEA8");
        asm(" MOVA R11, &0xFEAC");
        asm(" MOVA R12, &0xFEB0");
        asm(" MOVA R13, &0xFEB4");
        asm(" MOVA R14, &0xFEB8");
        asm(" MOVA R15, &0xFEBC");
        asm(" MOVA R0, &0xFE80");
        __no_operation();
        if ( function_switch == 'r' )
        {
            //printf("Restore completed\n");
            return;
        }
        //printf("Backup completed\n");
        return;
    }
    else if ( function_switch == 'r' )
    {
        //printf("Restore start\n");
        //printf("current time:%d\n",CurrentTime);
        if ( sending == true ) UCA1TXBUF = 'H';

        asm(" MOVA &0xFE84, R1");
        asm(" MOVA &0xFE88, R2");
        asm(" MOVA &0xFE8C, R3");
        asm(" MOVA &0xFE90, R4");
        asm(" MOVA &0xFE94, R5");
        asm(" MOVA &0xFE98, R6");
        asm(" MOVA &0xFE9C, R7");
        asm(" MOVA &0xFEA0, R8");
        asm(" MOVA &0xFEA4, R9");
        asm(" MOVA &0xFEA8, R10");
        asm(" MOVA &0xFEAC, R11");
        asm(" MOVA &0xFEB0, R12");
        asm(" MOVA &0xFEB4, R13");
        asm(" MOVA &0xFEB8, R14");
        asm(" MOVA &0xFEBC, R15");
        memcpy(((void*) 0x1C00) , &FRAM_buffer , 0x2000);
        asm(" MOVA &0xFE80, R0");
    }
    else if ( function_switch == 's' )
    {
        memcpy(&FRAM_buffer_empty , ((void*) 0x1C00) , 0x2000);

        asm(" MOVA R1, &0xFD84");
        asm(" MOVA R2, &0xFD88");
        asm(" MOVA R3, &0xFD8C");
        asm(" MOVA R4, &0xFD90");
        asm(" MOVA R5, &0xFD94");
        asm(" MOVA R6, &0xFD98");
        asm(" MOVA R7, &0xFD9C");
        asm(" MOVA R8, &0xFDA0");
        asm(" MOVA R9, &0xFDA4");
        asm(" MOVA R10, &0xFDA8");
        asm(" MOVA R11, &0xFDAC");
        asm(" MOVA R12, &0xFDB0");
        asm(" MOVA R13, &0xFDB4");
        asm(" MOVA R14, &0xFDB8");
        asm(" MOVA R15, &0xFDBC");
        asm(" MOVA R0, &0xFD80");
        __no_operation();

       if ( function_switch == 'e' )
       {
            //printf("Restore completed\n");
            return;
       }
       //printf("Backup completed\n");
       return;
    }
    else if ( function_switch == 'e' )
    {
        asm(" MOVA &0xFD84, R1");
        asm(" MOVA &0xFD88, R2");
        asm(" MOVA &0xFD8C, R3");
        asm(" MOVA &0xFD90, R4");
        asm(" MOVA &0xFD94, R5");
        asm(" MOVA &0xFD98, R6");
        asm(" MOVA &0xFD9C, R7");
        asm(" MOVA &0xFDA0, R8");
        asm(" MOVA &0xFDA4, R9");
        asm(" MOVA &0xFDA8, R10");
        asm(" MOVA &0xFDAC, R11");
        asm(" MOVA &0xFDB0, R12");
        asm(" MOVA &0xFDB4, R13");
        asm(" MOVA &0xFDB8, R14");
        asm(" MOVA &0xFDBC, R15");
        memcpy(((void*) 0x1C00) , &FRAM_buffer_empty, 0x2000);
        asm(" MOVA &0xFD80, R0");
    }
}

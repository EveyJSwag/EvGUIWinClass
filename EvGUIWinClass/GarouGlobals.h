#pragma once
/*
Garou.exe+85178:
~~~~~~~~~~~~~~~~
movdqa xmm0,[esi]
movdqa xmm1,[esi+10]
movdqa xmm2,[esi+20]
movdqa xmm3,[esi+30]
movdqa [edi],xmm0
movdqa [edi+10],xmm1
movdqa [edi+20],xmm2
movdqa [edi+30],xmm3
movdqa xmm4,[esi+40]
movdqa xmm5,[esi+50]
movdqa xmm6,[esi+60]
movdqa xmm7,[esi+70]
movdqa [edi+40],xmm4
movdqa [edi+50],xmm5
movdqa [edi+60],xmm6
movdqa [edi+70],xmm7
*/
//Garou.exe + 85182 - 
//E9 79AEFA00 - jmp 01430000
// 144001C: E9 | 66 51 04 FF
static unsigned int pixel_proc_offest_addr = 0x8517E;
static const unsigned char original_pixel_proc[] ={ 
	0x66, 0x0F, 0x6F, 0x06, 
	0x66, 0x0F, 0x6F, 0x4E, 0x10, 
	0x66, 0x0F, 0x6F, 0x56, 0x20, 
	0x66, 0x0F, 0x6F, 0x5E, 0x30, 
	0x66, 0x0F, 0x7F, 0x07, 
	0x66, 0x0F, 0x7F, 0x4F, 0x10, 
	0x66, 0x0F, 0x7F, 0x57, 0x20, 
	0x66, 0x0F, 0x7F, 0x5F, 0x30, 
	0x66, 0x0F, 0x6F, 0x66, 0x40, 
	0x66, 0x0F, 0x6F, 0x6E, 0x50,
	0x66, 0x0F, 0x6F, 0x76, 0x60, 
	0x66, 0x0F, 0x6F, 0x7E, 0x70, 
	0x66, 0x0F, 0x7F, 0x67, 0x40, 
	0x66, 0x0F, 0x7F, 0x6F, 0x50, 
	0x66, 0x0F, 0x7F, 0x77, 0x60, 
	0x66, 0x0F, 0x7F, 0x7F, 0x70};


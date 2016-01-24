// linux 系统编译通过
// gcc filename.c -o filename
// ./filename

#include <stdlib.h>
#include <stdio.h>

unsigned char ip_hdr_8[] =
{
	0x1F,0x0E, 0x30, 0xD7, 0x51
};

unsigned short ip_hdr_16[] =
{
	0x4500,
	0x003c,
	0x0000,
	0x0000,
	0x4001,
	0x0000,		// checksum
	0xc0a8,
	0x0125,
	0xda3c,
	0x0682
};

unsigned int checksum_8(unsigned int cksum, void *pBuffer, unsigned int size)
{
    char num = 0;
    unsigned char *p = (unsigned char *)pBuffer;

    if ((NULL == pBuffer) || (0 == size))
    {
        return cksum;
    }

    while (size > 1)
    {
        cksum += ((unsigned short)p[num] << 8 & 0xff00) | ((unsigned short)p[num + 1] & 0x00FF);
        size  -= 2;
        num   += 2;
    }

	if (size > 0)
    {
        cksum += ((unsigned short)p[num] << 8) & 0xFFFF;
        num += 1;
    }

    while (cksum >> 16)
    {
        cksum = (cksum & 0xFFFF) + (cksum >> 16);
    }

    return ~cksum;
}


unsigned short checksum_16(unsigned short *addr,int len)
{
	unsigned short chksum;
	unsigned int sum = 0;

	while(len > 1)
	{
		sum += *addr++;
		len -= 2;
	}

	if(len == 1)
		sum += *(unsigned char*)addr;

	sum = (sum>>16) + (sum & 0xffff);
	sum += (sum>>16);
	chksum = ~sum;
	return (chksum);
}

int main()
{
	unsigned short chksum;

	printf("--------------------------------------------\n");

	chksum = checksum_8(0,ip_hdr_8,sizeof(ip_hdr_8));
	printf("checksum_8 = 0x%04x \n",chksum);


	printf("--------------------------------------------\n");

	chksum = checksum_16(ip_hdr_16,sizeof(ip_hdr_16));
	printf("checksum_16 = 0x%04x \n",chksum);
	ip_hdr_16[5] = chksum;

	chksum = checksum_16(ip_hdr_16,20);
	if(chksum)
		printf("Checksum 16 is incorrect! \n");
	else
		printf("Checksum 16 is correct! \n");

	printf("--------------------------------------------\n");

    return 0;
}

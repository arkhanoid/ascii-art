struct bmph {
//    unsigned short id;    	// BM
    unsigned long flen;		// 3E14 0000
    unsigned long reserved;	// 0000 0000
    unsigned long offs;		// 0436 0000
    unsigned long H_SIZE; 	// 0028 0000
    unsigned long IMAGE_WIDTH;	// 0060 0000
    unsigned long IMAGE_HEIGHT;	// 0060 0000
    unsigned short PLANES ;	// 0001
    unsigned short BITPERPIXEL ;// 0008
    unsigned long COMPRESSION; 	// 0001
    unsigned long IMAGE_SIZE;   // 39DE 0000
    unsigned long V_RES;	// 0B6D 0000
    unsigned long  H_RES;       // 0B6D 0000
    unsigned long COLORINDEX;   // 0100 0000
    unsigned long IMPORTINDEX;  // 0100 0000
};

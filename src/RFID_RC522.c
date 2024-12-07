#include "RFID_RC522.h"

/*
 * Function name: RC522_SPI_Transfer.
 * Function: Send a byte and receive a byte using SPI. 
 * Input: data-- Data to write.
 * Return: Byte received from RFID module.
 */
uint8_t RC522_SPI_Transfer(u8 data)
{
	while(SPI_I2S_GetFlagStatus(MFRC522_SPI, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(MFRC522_SPI,data);

	while(SPI_I2S_GetFlagStatus(MFRC522_SPI, SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(MFRC522_SPI);
}

/*
 * Function name: Write_MFRC5200.
 * Function: Write a byte to RC522's register. 
 * Input: addr-- Adddresss of register to write data to.
		  val-- Data to write.
 * Return: none.
 */
void Write_MFRC522(uchar addr, uchar val)
{
	/* CS LOW */
	GPIO_ResetBits(MFRC522_CS_GPIO, MFRC522_CS_PIN);
	RC522_SPI_Transfer((addr<<1)&0x7E);	// bit 0 (0:write,1:read) bit 7:0
	RC522_SPI_Transfer(val);
	
	/* CS HIGH */
	GPIO_SetBits(MFRC522_CS_GPIO, MFRC522_CS_PIN);
}

/*
 * Function name: Read_MFRC522.
 * Function: Read a byte to RC522's register. 
 * Input: addr-- Adddresss of register to read data.
 * Return: Data read from register.
 */
/*Thao tác	Ð?a ch? du?c g?i qua SPI	Gi?i thích
GHI (WRITE)	0xxxxxx0 (MSB = 0, LSB = 0)	Bit 7 = 0: Thao tác ghi. Bit 0 = 0: Ðia ch hop le
Ð?C (READ)	1xxxxxx0 (MSB = 1, LSB = 0)	Bit 7 = 1: Thao tác d?c. Bit 0 = 0: Ðia chi hop le.*/
uchar Read_MFRC522(uchar addr)
{
	uchar val;

	/* CS LOW */
	GPIO_ResetBits(MFRC522_CS_GPIO, MFRC522_CS_PIN);

	//Adddresss fomat:1XXXXXX0
	RC522_SPI_Transfer(((addr<<1)&0x7E) | 0x80);	// bit 7 set len 1 : doc , 0 : write , bit 0 : 0 
	val = RC522_SPI_Transfer(0x00);
	
	/* CS HIGH */
	GPIO_SetBits(MFRC522_CS_GPIO, MFRC522_CS_PIN);
	
	return val;	
	
}

/*
 * Function name: SetBitMask.
 * Function: Set a bit in a RC522's register. 
 * Input: reg-- Adddresss of register.
		  mask-- Value to set.
 * Return: none
 */
void SetBitMask(uchar reg, uchar mask)  
{
	// vd :reg = 1111000; mask = 1111 1111
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp | mask);  // set bit mask
}

/*
 * Function name: ClearBitMask.
 * Function: Clear a bit in a RC522's register. 
 * Input: reg-- Adddresss of register.
		  mask-- Value to clear.
 * Return: none
 */
void ClearBitMask(uchar reg, uchar mask)  
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & (~mask));  // clear bit mask
} 

/*
 * Function name: AntennaOn.
 * Function: Turn on the Antenna, should wait about 1ms. 
 * Input: none
 * Return: none
 */
void AntennaOn(void)
{
	uchar temp;

	temp = Read_MFRC522(TxControlReg);

	SetBitMask(TxControlReg, 0x03); // bit 0 & bit 1 set = 1 bat anten
}


/*
 * Function name: AntennaOff.
 * Function: Turn off the Antenna, should wait about 1ms. 
 * Input: none
 * Return: none
 */
void AntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}

/*
 * Function name: ResetMFRC522.
 * Function: Reset the RC522. 
 * Input: none
 * Return: none
 */
void MFRC522_Reset(void)
{
    Write_MFRC522(CommandReg, PCD_RESETPHASE);
}

/*
 * Function name: InitMFRC522.
 * Function: Initiate the RC522. 
 * Input: none
 * Return: none
 */
void MFRC522_Init(void)
{
	
	MFRC522_Reset(); 
	 	
	//Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
	//0x8D: 10001101 trong do bit 7(tauto =1 tu dong khoi dong lai bo dem) bit 0-3 : cau hinh tan so timer
	Write_MFRC522(TModeReg, 0x8D);		//Tauto=1; f(Timer) = 6.78MHz/TPreScaler
	Write_MFRC522(TPrescalerReg, 0x3E);	//TModeReg[3..0] + TPrescalerReg 0x3E = 62
	Write_MFRC522(TReloadRegL, 30);            //treloadreg = L+H= 30 + 0
	Write_MFRC522(TReloadRegH, 0);
  //timer = (62*30)/6.78 =274µs : la khoang time phu hop cho cac tac vu rc522  
	
	
	Write_MFRC522(TxAutoReg, 0x40);		//0x40 = 0100 0000  Bit 6 = 1 100% ASK (tin hieu hoan toan tat trong cac gian doan khong gui du lieu)
	Write_MFRC522(ModeReg, 0x3D);		//Bit [3 - 0] =0xD tuong duong CRC =0x6363 theo tieu chuan iso 14443

	//ClearBitMask(Status2Reg, 0x08);		//MFCrypto1On=0
	//Write_MFRC522(RxSelReg, 0x86);		//RxWait = RxSelReg[5..0]
	//Write_MFRC522(RFCfgReg, 0x7F);   		//RxGain = 48dB

	AntennaOn();		//Mo Anten
}

/*
 * Function name: MFRC522_ToCard.
 * Function: giao tiep RC522 and ISO14443???. 
 * Input: command-- lenh gui den MF522.
		  sendData-- Du lieu gui den the bang MFRC522.
		  sendLen-- Chieu dai du lieu gui .
		  backdata-- Du lieu nhan duoc tro lai.
		  backlen-- Tra ve do dai bit cua du lieu.
 * Return: MI_OK neu thanh cong.
 */
uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen)
{
    uchar status = MI_ERR; // luu trang thai
    uchar irqEn = 0x00;  // bit bat hoac tat ngat tuy thuoc vao thanh ghi lenh MF522 se co gia tri khac nhau (CommIEnReg)
     
	
	/*waitIRq : bien nay dung de so sanh voi thanh ghi CommIEnReg xem da giao tiep thanh cong chua
	 neu khong thay cac bit mong doi trong khoang thoi gian thi se la timeout*/
	  uchar waitIRq = 0x00;// bit xac dinh co ngat mà vi dieu khien cho rc522 thiet lap khi giao tiep thanh cong
    uchar lastBits; // luu tru thong tin ve so bit cuoi cung cua du lieu phan hoi rc522, xu ly du lieu phan hoi tu rc522 lay gia tri tu thanh ghi ControlReg 
    uchar n; // xem co nhan du du lieu gui hoac nhan dc tu rc522 , so byte doc duoc tu thanh ghi FIFODataReg duoc luu vao bien n
    uint i; // bien couter kiem tra trang thai ngat rc522, neu giao tiep vuot qua dieu kien timeout thì giao se that bai

    switch (command)
    {
        case PCD_AUTHENT:		//Xac nhan the gan
		{
			/*0x12 : 0001 0010 ; bit 4 :bao hieu ket thuc qua trinh; bit 1 : bao loi neu xac thuc that bai */
			irqEn = 0x12;
			/*0x10: 0001 0000 ; bit 4 : bao hieu hoan tat xac thuc*/
			waitIRq = 0x10; 
			break;
		}
		case PCD_TRANSCEIVE:	// Gui du lieu FIFO
		{
			/*0x77 : 0111 0111 
			bit 6 : bao hoan thanh gui du lieu
			bit 5: bao nhan du lieu xong
			bit 4 : bao ket thuc qua trinh 
			bt 3 : bao hieu FIFO gan day
			bit 2 : bao hieu FIFO gan trong
			bit 1 : bao loi qua trinh giao tiep*/
			irqEn = 0x77;
			/*0x30: 0011 0000
			bit 5 :bao nhan du lieu tu the thanh cong
      bit 4 : bao ket thuc truyen hoac nhan du lieu	*/
			waitIRq = 0x30;
			break;
		}
		default:
			break;
    }
    // 0x80 :1000 0000
    Write_MFRC522(CommIEnReg, irqEn|0x80);	//bit 7 = 1  cho phep cac ngat duoc bat
    ClearBitMask(CommIrqReg, 0x80);//Clear tat ca cac bit yeu cau ngat de khong con ngat cu ton tai khi bat dau giao tiep
    SetBitMask(FIFOLevelReg, 0x80);			//FlushBuffer=1 dung de xoa du lieu truoc do co trong FIFO, Khoi tao FIFO
    
	Write_MFRC522(CommandReg, PCD_IDLE);	//NO action; Huy bo lenh hien hanh, dua thiet bi vao trang thai cho lenh moi	

	// Ghi du lieu vao FIFO
    for (i=0; i<sendLen; i++)
    {   
		Write_MFRC522(FIFODataReg, sendData[i]);    
	}

	//chay
	Write_MFRC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {    
		SetBitMask(BitFramingReg, 0x80);		//StartSend=1, qua trinh truyen du lieu bat dau
	}   
    
	//Cho doi de nhan duoc du lieu day du
	i = 2000;	//i tuy thuoc tan so thach anh, thoi gian toi da cho the M1 la 25ms
    do 
    {
		//CommIrqReg[7..0]
		/*Set1(bao trang thai cac bit ngat) TxIRq(hoan tat gui du lieu) RxIRq(nhan du lieu) IdleIRq(nhan roi)
			HiAlerIRq LoAlertIRq ErrIRq(ngat do loi xay ra) TimerIRq(thoi gian het han)*/
        n = Read_MFRC522(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq)); // khi i = 0 & TimerIRq = 1 & doi 1 trong 2 ngat la (0x10 hoac 0x30)

    ClearBitMask(BitFramingReg, 0x80);			//StartSend=0
	
    if (i != 0)
    {    
			/*0x1B = 0b00011011 
			bit 4 (BufferOvfl): tran bo dem FIFO, bit 3(Collerr):loi va cham, bit 1(CRCErr):loi CRC, bit 0 (ProtecolErr):loi giao thuc*/
			if(!(Read_MFRC522(ErrorReg) & 0x1B)) // neu khong co cac loi tren thi dua trang thai ok
        {
            status = MI_OK;
					if (n & irqEn & 0x01) //0x01: 0000 0001 /  bit 0 (TimerIRq) : neu khong co the hoac khong co du lieu
            {   
				status = MI_NOTAGERR;			//khong phat hien the hoac du lieu khi da het timeout  
			}

            if (command == PCD_TRANSCEIVE)
            {
               	n = Read_MFRC522(FIFOLevelReg); // doc so luong byte du lieu trong FIFO
							/*lastBits:neu du lieu day du 8 bit thi last bit = 0,neu last bit khong du 8 bit thi last bit khac 0 */
              	lastBits = Read_MFRC522(ControlReg) & 0x07; // lay 3 bit cuoi cung 
                if (lastBits) //khong du byte (khac 0)
                {   
					*backLen = (n-1)*8 + lastBits;   
				}
                else
                {   
					*backLen = n*8;   
				}

                if (n == 0)// du byte
                {   
					n = 1;    
				}
                if (n > MAX_LEN)
                {   
					n = MAX_LEN;   
				}
				
				//Doc FIFO trong cac du lieu nhan duoc
                for (i=0; i<n; i++)
                {   
					backData[i] = Read_MFRC522(FIFODataReg);    
				}
            }
        }
        else
        {   
			status = MI_ERR; 
		}
        
    }
	
    //SetBitMask(ControlReg,0x80);           //timer stops
    //Write_MFRC522(CommandReg, PCD_IDLE); 

    return status;
}

/*
 * Ten ham:MFRC522_Request
 * Chuc nang:Phat hien the, doc loai the
 * Input:reqMode--Phat hien co the,
 *			 TagType--Loai the tra ve
 *			 	0x4400 = Mifare_UltraLight
 *				0x0400 = Mifare_One(S50)
 *				0x0200 = Mifare_One(S70)
 *				0x0800 = Mifare_Pro(X)
 *				0x4403 = Mifare_DESFire
 * Return: MI_OK neu thanh cong
 */
uchar MFRC522_Request(uchar reqMode, uchar *TagType)
{
	uchar status;  
	uint backBits;			//cac bit du lieu nhan duoc

	/*truyen 7 bit cuoi cua byte cuoi cung trong FIFO*/
	Write_MFRC522(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]	
	
	TagType[0] = reqMode; // chua 2 lenh : Request(kiem tra the trong vung doc) va Wake-Up(danh thuc the)
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MI_OK) || (backBits != 0x10)) //kiem tra giao tiep cua the va phan hoi tu the co du 2 byte khong (16bit)
	{    
		status = MI_ERR;
	}
   
	return status; 
}


/*
 * Ten ham:MFRC522_Anticoll
 * Chuc nang:Phat hien chong va cham, chon the va doc so serial the
 * Input:serNum--Tra ve serial the 4 byte, byte 5 la ma checksum
 * Tra ve: MI_OK neu thanh cong
 */
uchar MFRC522_Anticoll(uchar *serNum)
{
    uchar status;
    uchar i; // bien vong lap
		uchar serNumCheck=0; // kiem tra so serial
    uint unLen; // do dai du lieu nhan tu the
    

    //ClearBitMask(Status2Reg, 0x08);		//TempSensclear
    //ClearBitMask(CollReg,0x80);			//ValuesAfterColl
	Write_MFRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0] = 0, truyen toan bo du lieu bao gom byte cuoi cung
 
    serNum[0] = PICC_ANTICOLL; // gan lenh chong xung dot vao vi tri dau tien cua mang de gui toi rfrc522
    serNum[1] = 0x20; // 0010 0000 // xu ly 5 byte uid
	
	/*yeu cau gui du lieu , sernum 1 chong xung dot,2 byte cua sernum, sau khi xu ly xong data se luu vao bien sernum 2,
   va ghi do dai du lieu vao unlen  */
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
	{
		//Kiem tra so serial the
		for (i=0; i<4; i++)
		{   
		 	serNumCheck ^= serNum[i]; // luu so sernum vao sernumcheck bang phep xor
		}
		if (serNumCheck != serNum[i]) // kiem tra lai sernum
		{   
			status = MI_ERR;    
		}
    }

    //SetBitMask(CollReg, 0x80);		//ValuesAfterColl=1

    return status;
} 


/*
 * Ten Ham:CalulateCRC
 * Chuc nang:MFRC522 tinh toan RC522
 * Input:pIndata--Du lieu CRC vao can tinh toan,len--Chieu dai du lieu,pOutData--Ket qua tinh toan CRC
 * Tra ve: Khong
 */
void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData)
{
    uchar i, n;

    ClearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0 chuan bi cho viec tinh toan crc moi	
    SetBitMask(FIFOLevelReg, 0x80);			//xoa du lieu trong bo dem FIFO va dat con tro ve trang thai dau
    //Write_MFRC522(CommandReg, PCD_IDLE);

	//Ghi du lieu vao FIFO
    for (i=0; i<len; i++)
    {   
		Write_MFRC522(FIFODataReg, *(pIndata+i));   
	}
    Write_MFRC522(CommandReg, PCD_CALCCRC);

	// Cho cho viec tinh toan CRC hoan tat
    i = 0xFF;
    do 
    {
        n = Read_MFRC522(DivIrqReg); // doc tung gia tri cua thanh ghi
        i--;
    }
    while ((i!=0) && !(n&0x04));			//khi i het timeout va CRCIrq = 1

	//Doc ket qua tinh toan CRC
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}


/*
 * Ten ham:MFRC522_SelectTag
 * Chuc nang:Lua chon the, doc dung luong bo nho the
 * Input:serNum--So serial the
 * Tra ve:Dung luong the tra ve thanh cong
 */
uchar MFRC522_SelectTag(uchar *serNum)
{
	uchar i;
	uchar status;
	uchar size;
	uint recvBits;
	uchar buffer[9]; 

	//ClearBitMask(Status2Reg, 0x08);			//MFCrypto1On=0

    buffer[0] = PICC_SElECTTAG; // chon the
	buffer[1] = 0x70;// do dai du lieu gui di (7 byte: lenh + uid + crc)
    for (i=0; i<5; i++) 
    {
    	buffer[i+2] = *(serNum+i);// gui 5 byte vao uid buffer
    }
	CalulateCRC(buffer, 7, &buffer[7]);		//tinh toan crc 7 byte va luu vao buffer[7]
		/*gui du lieu tu buffer den the, nhan phan hoi tu the luu vao buffer va so bit phan hoi vao recvbit*/
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    
    if ((status == MI_OK) && (recvBits == 0x18)) // neu satus ok va the phan hoi dung 24 bit
    {   
		size = buffer[0]; // gan loai the da chon vao size
	}
    else
    {   
		size = 0;// (loi)
	}

    return size;
}


/*
 * Ten Ham:MFRC522_Auth
 * Chuc nang:Xac nhan mat khau the
 * Input:authMode--Che do xac thuc mat khau
                 0x60 = Xac nhan phim A
                 0x61 = Xac nhan phim B
             BlockAddr--Cac khoi dia chi
             Sectorkey--Khu vuc mat khau
             serNum--So serial the, 4 byte
 * Tra ve:MI_OK neu thanh cong
 */
uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum)
{
    uchar status;
    uint recvBits;
    uchar i;
	uchar buff[12]; 

	//Xac nhan lenh + Khoi dia chi + mat khau + so nhanh
    buff[0] = authMode; //xac thuc key A hoac B 
    buff[1] = BlockAddr; // dia chi can xac thuc
    for (i=0; i<6; i++)
    {    
		buff[i+2] = *(Sectorkey+i);// luu mat khau vao buff(2..7)  
	}
    for (i=0; i<4; i++)
    {    
		buff[i+8] = *(serNum+i);// luu uid cua the vao buff(8..11)   
	}
		/*gui lenh xac thuc key va so sanh voi buff, nhan lai phan hoi rfid vao buff va luu so bit vao recvbit*/
    status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))// neu bit 0x08 khong duoc bat sau khi xac thuc
    {   
		status = MI_ERR;   
	}
    
    return status;
}


/*
 * Ten ham:MFRC522_Read
 * Chuc nang: Doc khoi du lieu
 * Input:blockAddr--Cac khoi dia chi;recvData--Khoi du lieu doc ra
 * Tra ve: MI_OK neu thanh cong
 */
uchar MFRC522_Read(uchar blockAddr, uchar *recvData)
{
    uchar status;
    uint unLen;

    recvData[0] = PICC_READ;// doc du lieu the
    recvData[1] = blockAddr;// dia chi block can doc
    CalulateCRC(recvData,2, &recvData[2]);// tinh toan crc cho lenh doc sau do crc se luu vao recvData[2] va[3]
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))// khong nhan du lieu dung tu the
    {
        status = MI_ERR;
    }
    
    return status;
}


/*
 * Ten ham:MFRC522_Write
 * Chuc nang:Viet khoi du lieu
 * Input:blockAddr--cac khoi dia chi;writeData--du lieu ghi
 * Tra ve: MI_OK neu thanh cong
 */
uchar MFRC522_Write(uchar blockAddr, uchar *writeData)
{
    uchar status;
    uint recvBits;
    uchar i;
		uchar buff[18]; 
    
    buff[0] = PICC_WRITE;//lenh ghi
    buff[1] = blockAddr;// dia chi can ghi
    CalulateCRC(buff, 2, &buff[2]);// tinh toan crc cho 2 byte tren va luu vao buff[2]
	/*truyen di du lieu buff toi rfid voi 4 byte va cho phan hoi va so bit se luu vao recvbits*/
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))// neu khac ok hoac so bit nhan khong dung hoac ma tra ve khac 0x0A trong byte dau cua buff
    {   
		status = MI_ERR;   
	}
        
    if (status == MI_OK)
    {
        for (i=0; i<16; i++)		//16 byte FIFO ghi du lieu vao
        {    
        	buff[i] = *(writeData+i);   
        }
        CalulateCRC(buff, 16, &buff[16]);// tinh toan crc cua 16 byte du lieu va luu vao buff[16]
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);// gui du lieu vao the voi buff voi 18 byte va luu so bit phan hoi lai vao recvbits
        
		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) // kiem tra lai lan nua
        {   
			status = MI_ERR;   
		}
    }
    
    return status;
}


/*
 * Ten ham:MFRC522_Halt
 * CHuc nang: Dua the vao ngu dong ngung giao tiep
 * Input: Khong
 * Tra ve: Khong
 */
void MFRC522_Halt(void)
{
	uint unLen;
	uchar buff[4]; 

	buff[0] = PICC_HALT;
	buff[1] = 0;
	CalulateCRC(buff, 2, &buff[2]);// tinh toan crc va luu ket qua vao buff[2] va [3]
 
	MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);// gui du lieu 4 byte tu mfrc522 toi the va doi phan hoi,mfrc522 nhan du lieu va luu vao buff so bit se luu vao unlen
}
void MFRC522_StopCrypto1(void){
	// Clear MFCrypto1On bit
	ClearBitMask(Status2Reg, 0x08); 
}

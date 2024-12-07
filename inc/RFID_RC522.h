
// the sensor communicates using SPI, so include the library:
#ifndef __RFID_RC522_H__
#define __RFID_RC522_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f10x.h"
#include "spi.h"

#define	uchar	uint8_t
#define	uint	uint32_t 

//Chieu dai toi da cua mang
#define MAX_LEN 16


// Dinh nghia thanh ghi lenh cho MF522
#define PCD_IDLE              0x00               //NO action; Huy bo lenh hien hanh
#define PCD_AUTHENT           0x0E               //Xac thuc KEY
#define PCD_RECEIVE           0x08               //Nhan du Lieu
#define PCD_TRANSMIT          0x04               //Gui du lieu
#define PCD_TRANSCEIVE        0x0C               //Gui va nhan du lieu
#define PCD_RESETPHASE        0x0F               //Reset
#define PCD_CALCCRC           0x03               //Tinh toan CRC

// Dinh nghia thanh ghi lenh cho Mifare_One
#define PICC_REQIDL           0x26               //Anten hoat dong, khong vao che do ngu dong
#define PICC_REQALL           0x52               //Tat ca cac loai the
#define PICC_ANTICOLL         0x93               //Chong va cham the
#define PICC_SElECTTAG        0x93               //Tuy chon the
#define PICC_AUTHENT1A        0x60               //Xac minh chinh xac
#define PICC_AUTHENT1B        0x61               //Xac minh phim B
#define PICC_READ             0x30               //Khoi doc
#define PICC_WRITE            0xA0               //Khoi viet
#define PICC_DECREMENT        0xC0               //Ghi no
#define PICC_INCREMENT        0xC1               //Nap tien
#define PICC_RESTORE          0xC2               //Chuyen khoi du lieu vao bo dem
#define PICC_TRANSFER         0xB0               //Duoc luu tru trong bo dem du lieu
#define PICC_HALT             0x50               //Vao che do ngu


//Bang ma loi giao tiep va phan hoi lai
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2


//------------------Thanh ghi MFRC522---------------
//Page 0:Command and Status
#define     Reserved00            0x00    //Khong su dung , danh rieng cho muc dich noi bo
#define     CommandReg            0x01    // dung de gui lenh dieu khien ( Reset, Transceive, Authenticate)
#define     CommIEnReg            0x02    // bat hoac tat cac ngat giao tiep
#define     DivlEnReg             0x03    // bat hoac tat ngat chia tan so
#define     CommIrqReg            0x04    // xem cac trang thai ngat giao tiep
#define     DivIrqReg             0x05    // xem cac trang thai ngat tu bo chia tan so
#define     ErrorReg              0x06    // chi ra loi xay ra trong giao tiep (CRC)
#define     Status1Reg            0x07    // bao trang thai tong quat ( dang hoat dong, FIFO day...)
#define     Status2Reg            0x08    //Báo trang thái chi tiet hon (toc do truyen...)
#define     FIFODataReg           0x09    //Thanh ghi truy cap du lieu FIFO
#define     FIFOLevelReg          0x0A     // Báo so byte hien tai trong FIFO
#define     WaterLevelReg         0x0B    // Nguong kích hoat canh báo FIFO d?y
#define     ControlReg            0x0C     // luu tru bit cuoi cung va che do tiet kiem nang luong
#define     BitFramingReg         0x0D    // Ðieu chinh dinh dang bit cho truy?n/nh?n
#define     CollReg               0x0E     // Báo va cham khi nhieu the duoc phát hien cùng lúc
#define     Reserved01            0x0F    // khong su dung
//Page 1:Command     
#define     Reserved10            0x10  // khong su dung
#define     ModeReg               0x11  //Chon che do hoat dong cua RC522
#define     TxModeReg             0x12  //Thiet lap che do truyen (toc do, chuan giao thuc).
#define     RxModeReg             0x13  //Thiet lap che do nhan (tuong tu TxModeReg)
#define     TxControlReg          0x14  //Bat/tat các chân phát sóng RF.
#define     TxAutoReg             0x15   // Ðieu khien tu dong hóa truyen RF
#define     TxSelReg              0x16   // Chon dau ra RF
#define     RxSelReg              0x17   // Chon dau vào RF
#define     RxThresholdReg        0x18   // Nguong nhan tín hieu (phát hien the)
#define     DemodReg              0x19   // Ðieu chinh bo giai dieu che
#define     Reserved11            0x1A   // khong su dung
#define     Reserved12            0x1B  // khong su dung
#define     MifareReg             0x1C  // Cài d?t giao th?c MIFARE
#define     Reserved13            0x1D  // khong su dung
#define     Reserved14            0x1E  // khong su dung
#define     SerialSpeedReg        0x1F  //  Thiet lap toc do truyen du lieu.
//Page 2:CFG    
#define     Reserved20            0x20 // khong su dung  
#define     CRCResultRegM         0x21 // Byte cao cua ket qua CRC
#define     CRCResultRegL         0x22 // Byte thap cua ket qua CRC.
#define     Reserved21            0x23 // khong su dung
#define     ModWidthReg           0x24 // Ðieu chinh do rong tín hieu RF
#define     Reserved22            0x25 // khong su dung 
#define     RFCfgReg              0x26 //dieu chinh do manh tin hieu RF
#define     GsNReg                0x27 // Cài dat do loi (gain)
#define     CWGsPReg	          0x28  //Ðieu chinh cuong do sóng mang RF
#define     ModGsPReg             0x29 //Ðieu chinh cuong do tín hieu dieu che
#define     TModeReg              0x2A //Che do hen gio
#define     TPrescalerReg         0x2B //Cài dat bo chia tan so cho hen gio	
#define     TReloadRegH           0x2C //Byte cao cua giá tri tai lai hen gio.
#define     TReloadRegL           0x2D // Byte thap cua giá tri tai lai hen gio.
#define     TCounterValueRegH     0x2E // Byte cao cua bo dem hen gio
#define     TCounterValueRegL     0x2F // Byte thap cua bo dem hen gio
//Page 3:TestRegister     
#define     Reserved30            0x30 // khong su dung
#define     TestSel1Reg           0x31 // Chon tín hieu kiem tra.
#define     TestSel2Reg           0x32 // Chon tín hieu kiem tra bo sung
#define     TestPinEnReg          0x33 // Kích hoat tín hieu kiem tra ra chân pin
#define     TestPinValueReg       0x34 // Gán giá tri tín hieu kiem tra ra pin
#define     TestBusReg            0x35 // kiem tra bus tin hieu
#define     AutoTestReg           0x36 //Kích hoat kiem tra tu d?ng
#define     VersionReg            0x37 // Phiên ban cua RC522.
#define     AnalogTestReg         0x38 // Ki?m tra tín hieu tuong tu
#define     TestDAC1Reg           0x39   // Cài dat cho bo DAC 1.
#define     TestDAC2Reg           0x3A   // Cài dat cho bo DAC 2.
#define     TestADCReg            0x3B   //Kiem tra ADC
#define     Reserved31            0x3C   // khong su dung
#define     Reserved32            0x3D   // khong su dung
#define     Reserved33            0x3E   // khong su dung
#define     Reserved34			  		0x3F   // khong su dung

void MFRC522_Init(void);
uchar MFRC522_Request(uchar reqMode, uchar *TagType);
uchar MFRC522_Anticoll(uchar *serNum);
uchar MFRC522_SelectTag(uchar *serNum);
uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum);
uchar MFRC522_Write(uchar blockAddr, uchar *writeData);				
uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum);
uchar MFRC522_Read(uchar blockAddr, uchar *recvData);
void MFRC522_Halt(void);															 
void MFRC522_StopCrypto1(void);
#ifdef __cplusplus
}
#endif

#endif													 
															 
															 
															 
															 
															 
															 
															 


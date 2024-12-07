
#define MAX_LEN 16
#define MAX_CARDS 5

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*----------------------------------------------------------------------------*/
uchar status;
uchar str[MAX_LEN]; // bo dem du lieu khi giao tiep the
uchar serNum[5]; // nhan dang the
char myString[100];
uchar check,check2;// check: kiem tra xem the co hop le khong, check2:kiem tra cac trang thai khac
uchar Key_Cards[MAX_CARDS][5]; //luu tru du lieu tu sdcard
uchar test [5] ={0xfd, 0x1c, 0x29, 0x40, 0x88}; // ma test uid tung  the
FATFS FatFs;
FIL fil;
FRESULT fr;

//so sanh 2 mang
int compareArrays(const uchar *arr1, const uchar *arr2, int size) {
    for (int i = 0; i < size; ++i) {
        if (arr1[i] != arr2[i]) {
            return 0; //Not correct.
        }
    }
    return 1; //Correct.
}

// hien thi noi dung cua tep
void showFileContent() {
    f_rewind(&fil); // Di chuyen con tro ve dau tep
    char ch;
    UINT bytesRead;
    while (f_read(&fil, &ch, 1, &bytesRead) == FR_OK && bytesRead > 0) { // Doc tung ky tu va hien thi.
        USART_SendChar(ch); // Gui ky tu qua UART
    }
}

int main() {
    // Khoi tao phan cung

    SPIRC552_Config(); 
    TIMDelay_Config();  
    UART1_Config(); 
    MFRC522_Init(); 

    // Mount he thong tep
    fr = f_mount(&FatFs, "", 1);
    if (fr != FR_OK) {
        return 0;
    }

    //  Mo tep "cards.txt" de doc
    fr = f_open(&fil, "cards.txt", FA_READ);
    if (fr != FR_OK) {
        return 0;
    }

    // Hien thi noi dung cua tep "cards.txt"
    showFileContent();

    // Doc du lieu the tu tep
    for (int i = 0; i < MAX_CARDS; ++i) {
        UINT bytesRead; // So byte da doc tu file 
        fr = f_read(&fil, Key_Cards[i], sizeof(serNum), &bytesRead);
        if (fr != FR_OK || bytesRead != sizeof(serNum)) 
					{
            break; // Dung viec doc neu co loi hoac EOF
        }
    }
	
    f_close(&fil);//dong file

    while (1) {
        // Cho the duoc dua vao
        Delay_ms(1000); 
        status = MFRC522_Request(PICC_REQIDL, str);// kiem tra xem co the trong pham vi khong
        if (status == MI_OK) {	
            sprintf(myString,"Your card's number are: %x, %x, %x, %x, %x \r\n",serNum[0], serNum[1], serNum[2], serNum[3],serNum[4]);
						USART_SendString(myString);
            status = MFRC522_Anticoll(str); // chong va cham doc ma the uid gan nhat
            memcpy(serNum, str, sizeof(serNum)); // coppy gia tri vung temp uid va luu vao sernum
						check = 1;
						check2 = 1;
            // Reset bien cardMatched
            int cardMatched = 0;

            USART_SendString("The ID: ");
            for (int k = 0; k < sizeof(serNum); ++k) {
                sprintf(myString, "%02X ", serNum[k]); // Chuyen tu so sang dang hex
                USART_SendString(myString);
            }
            USART_SendString("\n");
 
            // So sanh ID the voi cac ID trong the SD
             for (int j = 0; j < MAX_CARDS; ++j) {
                if (compareArrays(serNum, Key_Cards[j], sizeof(serNum))) {
                    
                    sprintf(myString, "ID the khop tai vi tri %d\n", j);
                    USART_SendString(myString);
                    cardMatched = 1;  
                    break; 
                }
								if (!cardMatched) {
                USART_SendString("ID the khong khop voi du lieu SD\n");
								}
            }

            
        }
		if(check == 1||check2 ==1)
				{
				
					check = 0;
					check2 = 0;
					Delay_ms(1000);
					set_pwm(1500);
					Delay_ms(2000);
					set_pwm(500);
					Delay_ms(1000);
				}
		MFRC522_StopCrypto1();
    }
	
    return 0;
}

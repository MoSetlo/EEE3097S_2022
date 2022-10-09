/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "stm32f0xx_hal_msp.h"
#define CBC 1

#include "aes.h"
#include "pkcs7_padding.h"
#include "icm20948.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
int i=0;
char buffer[1000];
int len;

axises my_gyro;
axises my_accel;

//axises my_mag;

char Rx_indx, Rx_data[2], Rx_Buffer[200], Transfer_cplt;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
//#ifdef __GNUC__
//  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
//     set to 'Yes') calls __io_putchar() */
//  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif /* __GNUC__ */
//
///**
//  * @brief  Retargets the C library printf function to the USART.
//  * @param  None
//  * @retval None
//  */
//PUTCHAR_PROTOTYPE
//{
//    /* Place your implementation of fputc here */
//    /* e.g. write a character to the USART */
//    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 100);
//
//
//    return ch;
//}



static char * test_encrypt_cbc(char *keyIn, char *ptr);
static char * test_decrypt_cbc(char *keyIn, char *ptr);

//size_t convert_hex(uint8_t *dest, size_t count, const char *src) {
//    char buf[3];
//    size_t i;
//    for (i = 0; i < count && *src; i++) {
//        buf[0] = *src++;
//        buf[1] = '\0';
//        if (*src) {
//            buf[1] = *src++;
//            buf[2] = '\0';
//        }
//        if (sscanf(buf, "%hhx", &dest[i]) != 1)
//            break;
//    }
//    return i;
//}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t i;
    if (huart->Instance == USART2)  //current UART
        {
        if (Rx_indx==0) {for (i=0;i<200;i++) Rx_Buffer[i]=0;}   //clear Rx_Buffer before receiving new data

        if (Rx_data[0]!=13) //if received data different from ascii 13 (enter)
            {
            Rx_Buffer[Rx_indx++]=Rx_data[0];    //add data to Rx_Buffer
            }
        else            //if received data = 13
            {
            Rx_indx=0;
            Transfer_cplt=1;//transfer complete, data is ready to read
            }

        HAL_UART_Receive_IT(&huart2, Rx_data, 1);   //activate UART receive interrupt every time
        }

}


int hex_to_int(char c)
{
    if (c >= 97)
        c = c - 32;
    int first = c / 16 - 3;
    int second = c % 16;
    int result = first * 10 + second;
    if (result > 9) result--;
    return result;
}

int hex_to_ascii(char c, char d){
        int high = hex_to_int(c) * 16;
        int low = hex_to_int(d);
        return high+low;
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//int _write(int file, char *ptr, int len)
//{
//	 HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
//	 return len;
//}

//uint8_t *hexarray = NULL ;
//uint8_t *kexarray= NULL;
//struct AES_ctx ctx;
//uint8_t iv[]  = { 0x75, 0x52, 0x5f, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x65, 0x73, 0x74, 0x69, 0x6e, 0x67, 0x21, 0x21 };

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//int exit=0;





	   // test_encrypt_cbc();

	   // return exit
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  icm20948_init();
//int counter=0;
//float number= 12.45f;
//int yescounter=0;
//int stop=5;
char* key = "EEE3097SPASSWORD";// password


HAL_UART_Receive_IT(&huart2, Rx_data, 1); //activate uart rx interrupt every time receiving 1 byte
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  icm20948_gyro_read_dps(&my_gyro);
	  icm20948_accel_read_g(&my_accel);

	  sprintf(buffer,
	  				"(Ax: %f | Ay: %f | Az: %f)   "
	  				"(Gx: %f | Gy: %f | Gz: %f)   "
	  				"\r\n",
	  				my_accel.x, my_accel.y, my_accel.z,
	  				my_gyro.x, my_gyro.y, my_gyro.z);
	  	  	  	  	  len=strlen(buffer);
	  	  	  	  	  HAL_UART_Transmit(&huart2, buffer, len, 1000);
	  	  	  	  	  HAL_Delay(50);
//	  printf("Testing AES128\n\n");
//	  test_encrypt_cbc();
//	  printf("Hello: %d\r\n", counter++);
//	  printf("Yoo: %f\r\n", number);
//	  HAL_Delay(500);

	  if(Transfer_cplt)
	  {
//		  sprintf(buffer,"%s\r\n", Rx_Buffer);
//		  len=strlen(buffer);
//		  HAL_UART_Transmit(&huart2, buffer, len, 1000);
		  //printf("THE PLAIN kEY piece STRING = \n ");
		  char * test= test_encrypt_cbc(key,Rx_Buffer);

		  sprintf(buffer, "%s\r\n", test);
		                    len=strlen(buffer);
		                    HAL_UART_Transmit(&huart2, buffer, len, 1000);


		//  printf("%d", sizeof(test));
//		  int bufferlen=0;
//		  //
//		      for (i=0; i<strlen(test);i++){
//		      	bufferlen+=sprintf(buffer + bufferlen,"%c", test[i]);
//
//
//		      }

//		          len=strlen(buffer);
//		          HAL_UART_Transmit(&huart2, buffer, len, 1000);

		     char * detest= test_decrypt_cbc(key,test);
		     sprintf(buffer, "%s\r\n", detest);
		     		                    len=strlen(buffer);
		     		                    HAL_UART_Transmit(&huart2, buffer, len, 1000);



//		     sprintf(buffer,"No hex Again %s \r\n ", detest);
//
//		     		     len=strlen(buffer);
//		     		     HAL_UART_Transmit(&huart2, buffer, len, 1000);


//		      for (i=0; i<strlen(test);i++)
//		      {
//		          printf("%c", test[i]);
//		      }
//		      printf("\n");


		  Transfer_cplt=0;
		  HAL_Delay(500);
	  }

	  //yescounter++;

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }


  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LD4_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
static char * test_encrypt_cbc(char *keyIn, char *ptr)
{
    //Initialization Vector
    uint8_t iv[]  = { 0x75, 0x52, 0x5f, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x65, 0x73, 0x74, 0x69, 0x6e, 0x67, 0x21, 0x21 };

  //  uint8_t i;
    char* report = ptr;

    char* key = keyIn;
    int dlen = strlen(report);
    int klen = strlen(key);


    sprintf(buffer,"THE PLAIN TEXT STRING = %s \r\n", report);
    len=strlen(buffer);
    //HAL_UART_Transmit(&huart2, buffer, len, 1000);



//    for (i=0; i<dlen;i++){
//    	buffer[i]= (char)report[i];
//        sprintf(buffer,"%c\r", report[i]);
//    }
//
//    len=strlen(buffer);
//    HAL_UART_Transmit(&huart2, buffer, len, 1000);
//    printf("\n");

    sprintf(buffer, "THE PLAIN kEY STRING = %s\r\n ", key);
//    for (i=0; i<klen;i++){
//        printf("%c", key[i]);
//    }
//    printf("\n");
//    sprintf(buffer,"%s\r\n", key);
           len=strlen(buffer);
           //HAL_UART_Transmit(&huart2, buffer, len, 1000);



    //Proper Length of report
    int dlenu = dlen;
    if (dlen % 16) {
        dlenu += 16 - (dlen % 16);
        sprintf(buffer,"The original length of the STRING = %d and the length of the padded STRING = %d\r\n", dlen, dlenu);
        len=strlen(buffer);
       // HAL_UART_Transmit(&huart2, buffer, len, 1000);
    }

    //Proper length of key
    int klenu = klen;
    if (klen % 16) {
        klenu += 16 - (klen % 16);
        sprintf(buffer,"The original length of the KEY = %d and the length of the padded KEY = %d\r\n", klen, klenu);
        len=strlen(buffer);
      //  HAL_UART_Transmit(&huart2, buffer, len, 1000);
    }

    // Make the uint8_t arrays
//    hexarray = malloc(sizeof(int)*dlenu);
//    kexarray = malloc(sizeof(int)*dlenu);
    uint8_t hexarrayE[dlenu];
    uint8_t kexarrayE[klenu];

    // Initialize them with zeros
    memset( hexarrayE, 0, dlenu );
    memset( kexarrayE, 0, klenu );

    // Fill the uint8_t arrays
    for (int i=0;i<dlen;i++) {
        hexarrayE[i] = (uint8_t)report[i];
    }


    for (int i=0;i<klen;i++) {
        kexarrayE[i] = (uint8_t)key[i];
    }

    int reportPad = pkcs7_padding_pad_buffer( hexarrayE, dlen, sizeof(hexarrayE), 16 );
    int keyPad = pkcs7_padding_pad_buffer( kexarrayE, klen, sizeof(kexarrayE), 16 );

    sprintf(buffer,"The padded STRING in hex is = ");

    len=strlen(buffer);
   // HAL_UART_Transmit(&huart2, buffer, len, 1000);


//    			sprintf(buffer,"%02x\r", hexarrayp[i]);
//               len=strlen(buffer);
//               HAL_UART_Transmit(&huart2, buffer, len, 1000);


    int bufferlen=0;
//
    for (i=0; i<dlenu;i++){
    	bufferlen+=sprintf(buffer + bufferlen,"%02x", hexarrayE[i]);


    }

        len=strlen(buffer);
      //  HAL_UART_Transmit(&huart2, buffer, len, 1000);



//    printf("\n");
//
//    printf("The padded key in hex is = ");
//    for (i=0; i<klenu;i++){
//        printf("%02x",kexarray[i]);
//    }
//    printf("\n");
//
    // In case you want to check if the padding is valid
    int valid = pkcs7_padding_valid( hexarrayE, dlen, sizeof(hexarrayE), 16 );
    int valid2 = pkcs7_padding_valid( kexarrayE, klen, sizeof(kexarrayE), 16 );
    sprintf(buffer,"\r\nIs the pkcs7 padding valid  report = %d  |  key = %d\r\n", valid, valid2);
    len=strlen(buffer);
     //HAL_UART_Transmit(&huart2, buffer, len, 1000);

    //start the encryption
   struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, kexarrayE, iv);

    // encrypt
    AES_CBC_encrypt_buffer(&ctx, hexarrayE, dlenu);
    sprintf(buffer,"the encrypted STRING = ");
    len=strlen(buffer);
 //   HAL_UART_Transmit(&huart2, buffer, len, 1000);



//    char reportNew[len];
//
//        for (int i=0;i<len;i++)
//            {
//                reportNew[i] = buffer[i];
//            }


    	bufferlen=0;

    	//bufferlen2=0;
    	//char reportNew[dlenu];

    	//char * checks;
        for (i=0; i<dlenu;i++)
        {
            	bufferlen+=sprintf(buffer + bufferlen,"%02x", hexarrayE[i]);




        }

                len=strlen(buffer);

//                sprintf(buffer, "\r\nCheck len = %i\r\n", len);
//                  len=strlen(buffer);
//                  HAL_UART_Transmit(&huart2, buffer, len, 1000);


                //check = buffer;



               // HAL_UART_Transmit(&huart2, buffer, len, 1000);




                //char reportNew[100];
                char * reportNew;
                reportNew = malloc(sizeof(char)*len);

               // strcpy(reportNew, buffer);

                bufferlen=0;
                    for (i=0; i<len;i++)
                    {
                        bufferlen+=sprintf(reportNew+bufferlen,"%c",buffer[i]);
                    }







                sprintf(buffer, "Check len = %i and %s\r\n", len, reportNew);
                                  len=strlen(buffer);
                                //  HAL_UART_Transmit(&huart2, buffer, len, 1000);

                //checks=reportNew;


//    for (i=0; i<dlenu;i++){
//        printf("%c",hexarray[i]);
//    }
//
//
//    printf("\n");

    // reset the iv !! important to work!
    AES_ctx_set_iv(&ctx,iv);

    // start decryption
    AES_CBC_decrypt_buffer(&ctx, hexarrayE, dlenu);

  size_t actualDataLength = pkcs7_padding_data_length( hexarrayE, dlenu, 16);
  sprintf(buffer, "The actual data length (without the padding) = %ld\r\n", actualDataLength);
  len=strlen(buffer);
 // HAL_UART_Transmit(&huart2, buffer, len, 1000);
    sprintf(buffer,"the decrypted STRING in hex = ");
    len=strlen(buffer);
         //   HAL_UART_Transmit(&huart2, buffer, len, 1000);
//    for (i=0; i<actualDataLength;i++){
//        printf("%02x",hexarray[i]);
//    }
//    printf("\n");

    bufferlen=0;
            for (i=0; i<actualDataLength;i++)
            {
                	bufferlen+=sprintf(buffer + bufferlen,"%02x", hexarrayE[i]);


            }

                    len=strlen(buffer);
                //    HAL_UART_Transmit(&huart2, buffer, len, 1000);

//

    sprintf(buffer,"\r\nTHE RETURN TExt STRING = %s\r\n", reportNew);
    len=strlen(buffer);
   // HAL_UART_Transmit(&huart2, buffer, len, 1000);
//    for (i=0; i<dlen;i++){
//        printf("%c", reportNew[i]);
//    }
//  // printf("\n");

   return reportNew;
}


static char * test_decrypt_cbc(char *keyIn, char *ptr)
{


//	    char buf[3];
//
//	    size_t i;








    //Initialization Vector
    uint8_t iv[]  = { 0x75, 0x52, 0x5f, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x65, 0x73, 0x74, 0x69, 0x6e, 0x67, 0x21, 0x21 };








   // uint8_t i;
   //char  report[100];



    char* key = keyIn;


    const char * reportHex = ptr;


    size_t dlenHex = strlen(reportHex);

//    sprintf(buffer,"ptr%s  size %i  other size %i\r\n", reportHex, dlenHex, dlen);
//               len=strlen(buffer);
//               HAL_UART_Transmit(&huart2, buffer, len, 1000);





    uint8_t girl[dlenHex];




    char buf[3];
        size_t i;
        int value;
        for (i = 0; i < dlenHex && *ptr; i++) {
            buf[0] = *ptr++;
            buf[1] = '\0';
            if (*ptr) {
                buf[1] = *ptr++;
                buf[2] = '\0';
            }
            if (sscanf(buf, "%x", &value) != 1)
                break;
            girl[i] = value;
        }



        size_t dlen = i;






        sprintf(buffer,"ptr%s  size %i  other size %ld\r\n", reportHex, dlenHex, dlen);
                      len=strlen(buffer);
                      //HAL_UART_Transmit(&huart2, buffer, len, 1000);




//     char* st = ptr;
//            int length = strlen(st);
//
//    int i;
//            char buf = 0;
//
//            int bufferlen=0;
//            for(i = 0; i < length; i++){
//                    if(i % 2 != 0){
//                            bufferlen+=sprintf(buffer+bufferlen,"%c", hex_to_ascii(buf, st[i]));
//                    }else{
//                            buf = st[i];
//                    }
//            }
//
//            strcpy(report,buffer);

    //int dlen = strlen(report);

    int klen = strlen(key);
    char reportNew[dlen];

    sprintf(buffer,"THE PLAIN TEXT STRING in Dec = %s\r\n", reportHex);
       len=strlen(buffer);
      // HAL_UART_Transmit(&huart2, buffer, len, 1000);



//
//    printf("THE PLAIN TEXT STRING = ");
//    for (i=0; i<dlen;i++){
//        printf("%c", report[i]);
//    }
//    printf("\n");
//
//    printf("THE PLAIN kEY STRING = ");
//    for (i=0; i<klen;i++){
//        printf("%c", key[i]);
//    }
//    printf("\n");
//
//
//    //Proper Length of report
//    int dlenu = dlen;
//    if (dlen % 16) {
//        dlenu += 16 - (dlen % 16);
//        printf("The original length of the STRING = %d and the length of the padded STRING = %d\n", dlen, dlenu);
//    }
//
    //Proper length of key
    int klenu = klen;
    if (klen % 16) {
        klenu += 16 - (klen % 16);
        //printf("The original length of the KEY = %d and the length of the padded KEY = %d\n", klen, klenu);
    }

    // Make the uint8_t arrays
//    hexarray = malloc(sizeof(int)*dlen);
//    kexarray = malloc(sizeof(int)*dlen);
    uint8_t hexarrayD[dlen];
    uint8_t kexarrayD[klenu];




                for (int i = 0; i < dlen; i++)
                {
                   hexarrayD[i]=girl[i];
                }


//    // Initialize them with zeros
  //memset( hexarrayD, 0, dlen );
    memset( kexarrayD, 0, klenu );

//    // Fill the uint8_t arrays
//    for (int i=0;i<dlen;i++)
//    {
//        hexarrayD[i] = (uint8_t)report[i];
//    }


    for (int i=0;i<klenu;i++) {
        kexarrayD[i] = (uint8_t) key[i];
    }

    sprintf(buffer,"Prencrypt  hex is = %i ", dlen);

    len=strlen(buffer);
   // HAL_UART_Transmit(&huart2, buffer, len, 1000);


//    			sprintf(buffer,"%02x\r", hexarrayp[i]);
//               len=strlen(buffer);
//               HAL_UART_Transmit(&huart2, buffer, len, 1000);


    int nbufferlen=0;
//
    for (i=0; i<dlen;i++){
    	nbufferlen+=sprintf(buffer + nbufferlen,"%02x", hexarrayD[i]);


    }

        len=strlen(buffer);
       // HAL_UART_Transmit(&huart2, buffer, len, 1000);


//    int reportPad = pkcs7_padding_pad_buffer( hexarray, dlen, sizeof(hexarray), 16 );
//    int keyPad = pkcs7_padding_pad_buffer( kexarray, klen, sizeof(kexarray), 16 );

//    printf("The padded STRING in hex is = ");
//    for (i=0; i<dlenu;i++){
//        printf("%02x",hexarray[i]);
//    }
//    printf("\n");
//
//    printf("The padded key in hex is = ");
//    for (i=0; i<klenu;i++){
//        printf("%02x",kexarray[i]);
//    }
//    printf("\n");

//    // In case you want to check if the padding is valid
//    int valid = pkcs7_padding_valid( hexarray, dlen, sizeof(hexarray), 16 );
//    int valid2 = pkcs7_padding_valid( kexarray, klen, sizeof(kexarray), 16 );
//    printf("Is the pkcs7 padding valid  report = %d  |  key = %d\n", valid, valid2);

    //start the encryption
    struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, kexarrayD, iv);

    // encrypt
//    AES_CBC_encrypt_buffer(&ctx, hexarray, dlenu);
//    printf("the encrypted STRING = ");
//    for (i=0; i<dlenu;i++){
//        printf("%c",hexarray[i]);
//    }


//    printf("\n");

    // reset the iv !! important to work!
    AES_ctx_set_iv(&ctx,iv);

    // start decryption
    AES_CBC_decrypt_buffer(&ctx, hexarrayD, dlen);

    size_t actualDataLength = pkcs7_padding_data_length( hexarrayD, dlen, 16);
     sprintf(buffer, "The actual data length (without the padding) = %ld\r\n", actualDataLength);
     len=strlen(buffer);
    //HAL_UART_Transmit(&huart2, buffer, len, 1000);
       sprintf(buffer,"the decrypted STRING in hex = ");
       len=strlen(buffer);
        //HAL_UART_Transmit(&huart2, buffer, len, 1000);
   //    for (i=0; i<actualDataLength;i++){
   //        printf("%02x",hexarray[i]);
   //    }
   //    printf("\n");

      int bufferlen=0;
               for (i=0; i<actualDataLength;i++)
               {
                   	bufferlen+=sprintf(buffer + bufferlen,"%c", hexarrayD[i]);


               }

                       len=strlen(buffer);
                       //HAL_UART_Transmit(&huart2, buffer, len, 1000);

                       char * finalReturn;
                       finalReturn = malloc(sizeof(char)*len);

                       bufferlen=0;
                                           for (i=0; i<len;i++)
                                           {
                                               bufferlen+=sprintf(finalReturn+bufferlen,"%c",hexarrayD[i]);
                                           }


                                           sprintf(buffer,"\r\nTHE RETURN TExt STRING = %s %i\r\n", finalReturn, strlen(finalReturn));
                                               len=strlen(buffer);
                                              //HAL_UART_Transmit(&huart2, buffer, len, 1000);


//    printf("\r\nTHE RETURN TExt STRING = %s \r\n ", reportNew);
////    for (i=0; i<dlen;i++){
////        printf("%c", reportNew[i]);
////    }
////   printf("\r\n");
//
    return  finalReturn;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

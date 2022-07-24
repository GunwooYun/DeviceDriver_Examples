# Device_Driver_Examples   
## Develop Environment   
### 개발환경   
* Ubuntu 20.04.4 LTS   
* Kernel 5.15   
### 타겟보드   
* UDOO QUAD Cortex™-A9   
* Ubuntu 14.04.5 LTS   
* Kernel 3.14.56   
## Reference Book
* IT EXPERT 리눅스 드라이버 유영창 저 (한빛미디어)   
## Directories   
### module   
* module_basic_arm : ARM 모듈 기본 예제 / p.106   
* module_basic_x86 : X86 모듈 기본 예제 / p.106   
* module_parameter : 모듈 매개변수 예제 / p.119   
* module_parameter_LED_arm : UDOO 보드에서 모듈(매개변수)을 이용한 LED 컨트롤   
### Device Driver
* deviceDriver_basic : 디바이스드라이버 기본 예제, file_operations / p.170   
* deviceDriver_LED : 디바이스드라이버를 이용한 LED 제어 on UDOO / p.218   
* deviceDriver_LED_KEY : 키스위치를 눌러 LED 제어 on UDOO / p.236   
* deviceDriver_LED_KEY_copy : 시스템콜함수 copy_from_user(write()), copy_to_user(read()) 으로 LED, key 제어 / p.250   
* deviceDriver_ioctl : 디바이스를 ioctl() 커맨드 제어 / p.294   
* deviceDriver_ioctl_example : ioctl() 을 이용한 실습   
* deviceDriver_string : read(), write() 함수에 문자열 처리 실습   
* deviceDriver_minor : minor 번호에 따라 operation 변경 실행 / p.264   
* deviceDrvier_interrupt_basic : 인터럽트를 이용한 GPIO 값 read / p.343   
* deviceDriver_proc_basic : proc 파일시스템 기본 예제 / p.517   
* deviceDriver_proc_LED_KEY : proc 파일시스템 / p.527   
### Timer
* kernelTimer_basic : 커널타이머 기본 예제 / p.319   
* kernelTimer_dev : 커널타이머 디바이스드라이버 / p.333   
* kernelTimer_global_ptrmng : ptrmng 전역변수 선언 / p.328   
### etc
* localtimer : 로컬 타이머 예제   

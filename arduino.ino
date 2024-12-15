// 세그먼트 핀 정의 (a, b, c, d, e, f, g, dp)
const int segmentPins[] = {5, 6, 7, 8, 9, 10, 11, 12};

// 자릿수 핀 정의 (D1, D2, D3, D4)
const int digitPins[] = {A3, A2, A1, A4}; // "1234" 순서에 맞게 수정

// 각 숫자에 대한 세그먼트 값 (0~9)
const byte numbers[10] = {
  0b11000000, // 0
  0b11111001, // 1
  0b10100100, // 2
  0b10110000, // 3
  0b10011001, // 4
  0b10010010, // 5
  0b10000010, // 6
  0b11111000, // 7
  0b10000000, // 8
  0b10010000  // 9
};

// 조이스틱 핀 정의
const int VRx = A5; // X축 입력
const int VRy = A0; // Y축 입력
const int SW = 2;   // 버튼 입력

// 타이머 설정 (초 단위)
int remainingTime = 0;       // 초기 남은 시간
bool timerRunning = false;   // 타이머 실행 상태
unsigned long previousMillis = 0; // 이전 시간 기록

void setup() {
  // 세그먼트 핀 설정
  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], HIGH); // COMMON ANODE: HIGH = OFF
  }

  // 자릿수 핀 설정
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW); // COMMON ANODE: LOW = OFF
  }

  // 조이스틱 핀 설정
  pinMode(SW, INPUT_PULLUP); // SW 핀을 풀업 입력으로 설정

  Serial.begin(9600);
  Serial.println("Joystick Timer Initialized");
}

void loop() {
  // 조이스틱으로 타이머 값 조정 (항상 호출)
  handleJoystick();

  if (!timerRunning) {
    // 현재 타이머 값 디스플레이
    displayNumber(remainingTime);

    // 타이머 값이 0보다 크면 자동으로 시작
    if (remainingTime > 0) {
      Serial.println("Timer Started!");
      timerRunning = true;
      delay(300); // 디바운싱을 위해 약간의 지연
    }
  } else {
    // 타이머 실행 중
    handleTimer();
  }
}



// 조이스틱 입력 처리 함수
void handleJoystick() {
  int xValue = analogRead(VRx); // X축 값 읽기

  // X축 값으로 타이머 증가
  if (xValue > 600) {
    remainingTime += 10; // 타이머 10초 증가
    if (remainingTime > 9999) remainingTime = 9999; // 최대값 제한
    Serial.println("Time Increased: " + String(remainingTime));
    delay(200); // 너무 빠르게 증가하지 않도록 지연
  }

  // X축 값으로 타이머 감소
  if (xValue < 400) {
    remainingTime -= 10; // 타이머 10초 감소
    if (remainingTime < 0) remainingTime = 0; // 최소값 제한
    Serial.println("Time Decreased: " + String(remainingTime));
    delay(200); // 너무 빠르게 감소하지 않도록 지연
  }
}

// 타이머 동작 처리 함수
void handleTimer() {
  unsigned long currentMillis = millis(); // 현재 시간 기록

  // 1초마다 남은 시간을 감소
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis; // 이전 시간 갱신
    remainingTime--; // 남은 시간 감소

    if (remainingTime <= 0) {
      remainingTime = 0; // 남은 시간을 0으로 고정
      Serial.println("Timer Complete!");
      timerRunning = false; // 타이머 정지
    }

    Serial.print("Remaining Time: ");
    Serial.println(remainingTime);
  }

  // 현재 남은 시간 디스플레이
  displayNumber(remainingTime);
}

// 숫자를 각 자릿수에 출력하는 함수
void displayNumber(int number) {
  int digits[4] = {0, 0, 0, 0};
  digits[0] = number / 1000;           // 천의 자리
  digits[1] = (number / 100) % 10;    // 백의 자리
  digits[2] = (number / 10) % 10;     // 십의 자리
  digits[3] = number % 10;            // 일의 자리

  for (int i = 0; i < 4; i++) {
    clearAll();
    displayDigit(digits[i], i);
    delay(5);
  }
}

// 모든 세그먼트와 자릿수를 초기화하는 함수
void clearAll() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(segmentPins[i], HIGH); // COMMON ANODE: HIGH = OFF
  }
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], LOW); // COMMON ANODE: LOW = OFF
  }
}

// 특정 자릿수에 숫자를 표시하는 함수
void displayDigit(int number, int digitIndex) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], LOW); // 모든 자릿수를 OFF
  }

  for (int i = 0; i < 8; i++) {
    digitalWrite(segmentPins[i], bitRead(numbers[number], i) ? HIGH : LOW);
  }

  digitalWrite(digitPins[digitIndex], HIGH); // 선택한 자릿수 ON
}
